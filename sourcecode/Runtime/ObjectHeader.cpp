#include "ObjectHeader.h"
#include "RTClass.h"
#include "llvm/IR/DerivedTypes.h"
#include "RTClassType.h"
#include "NomJIT.h"
#include "NomClass.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "GlobalNameAddressLookupList.h"
#include "StringClass.h"
#include "RTInstanceType.h"
#include "FloatClass.h"
#include "IntClass.h"
#include "BoolClass.h"
#include "NomClassType.h"
#include "CompileHelpers.h"
#include "TypeOperations.h"
#include "RTDictionary.h"
#include "RTOutput.h"
#include "RefValueHeader.h"
#include "RTVTable.h"
#include "CallingConvConf.h"
#include "RTCompileConfig.h"
#include "CastStats.h"
#include "IMT.h"
#include "PWRefValue.h"
#include "PWVTable.h"
#include "PWClass.h"
#include "PWObject.h"
#include "PWType.h"

using namespace llvm;

namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* ObjectHeader::GetLLVMType()
		{
			static llvm::StructType* llvmtype = llvm::StructType::create(LLVMCONTEXT, "RT_NOM_ObjectHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				llvmtype->setBody(arrtype(TYPETYPE, 0), RefValueHeader::GetLLVMType(), arrtype(REFTYPE, 0));
			}
			return llvmtype;
		}
		llvm::StructType* ObjectHeader::GetLLVMType(int fieldcount, int typeargcount, bool rawInvoke)
		{
			return llvm::StructType::get(arrtype(TYPETYPE, typeargcount), RefValueHeader::GetLLVMType(), arrtype(REFTYPE, fieldcount + ((rawInvoke && (NomLambdaOptimizationLevel > 0)) ? 1 : 0)));
		}
		llvm::Constant* ObjectHeader::GetConstant(llvm::Constant* clsdesc, llvm::Constant* fields, llvm::Constant* typeargs, llvm::Constant* invokeptr)
		{
			if (fields == nullptr)
			{
				fields = llvm::ConstantArray::get(arrtype(REFTYPE, 0), {});
			}
			if (typeargs == nullptr)
			{
				typeargs = llvm::ConstantArray::get(arrtype(NLLVMPointer(RTTypeHead::GetLLVMType()), 0), {});
			}
			llvm::Constant* invokesarr;
			if (invokeptr == nullptr)
			{
				invokesarr = llvm::ConstantArray::get(arrtype(POINTERTYPE, 0), {});
			}
			else
			{
#ifdef _DEBUG
				if (NomLambdaOptimizationLevel == 0)
				{
					throw new std::exception();
				}
#endif
				invokesarr = llvm::ConstantArray::get(arrtype(POINTERTYPE, 1), ArrayRef<llvm::Constant*>(invokeptr));
			}

			auto nptr = llvm::ConstantPointerNull::get(POINTERTYPE);

			auto strct = MakeLLVMStruct(llvm::ArrayRef<llvm::Constant*>({ typeargs, ConstantStruct::get(RefValueHeader::GetLLVMType(), ConstantExpr::getPointerCast(clsdesc, RTVTable::GetLLVMType()->getPointerTo()), invokesarr), fields }));
			return strct;
		}
		llvm::Constant* ObjectHeader::GetGlobal(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::Twine name, llvm::Constant* clsdesc, llvm::Constant* fields, llvm::Constant* typeargs, bool isConstant)
		{
			llvm::SmallVector<char, 64> objNameBuf;
			auto nameStr = name.toStringRef(objNameBuf);
			llvm::GlobalVariable* obj = mod.getGlobalVariable(nameStr);

			if (obj == nullptr)
			{
				auto strct = GetConstant(clsdesc, fields, typeargs);
				obj = new llvm::GlobalVariable(mod, strct->getType(), isConstant, linkage, strct, nameStr);
				RegisterGlobalForAddressLookup(nameStr.str());
			}
			return llvm::ConstantExpr::getGetElementPtr(obj->getValueType(), obj, llvm::ArrayRef<llvm::Constant*>({MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::RefValueHeader)}));
		}
		llvm::Constant* ObjectHeader::FindGlobal(llvm::Module& mod, const std::string &name)
		{
			llvm::GlobalVariable* obj = mod.getGlobalVariable(name);
			if (obj == nullptr)
			{
				return obj;
			}
			return llvm::ConstantExpr::getGetElementPtr(obj->getValueType(), obj, llvm::ArrayRef<llvm::Constant*>({MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::RefValueHeader)}));
		}
		llvm::Value* ObjectHeader::GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* objPointer, int32_t argindex)
		{
			return PWObject(objPointer).ReadTypeArgument(builder, argindex);
		}
		void ObjectHeader::GenerateWriteTypeArgument(NomBuilder& builder, llvm::Value* objPointer, int32_t argindex, llvm::Value* val)
		{
			PWObject(objPointer).WriteTypeArgument(builder, argindex, val);
		}
		llvm::Value* ObjectHeader::GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* objPointer)
		{
			return PWObject(objPointer).PointerToTypeArguments(builder);
		}
		llvm::Value* ObjectHeader::ReadField(NomBuilder& builder, llvm::Value* objPointer, int32_t fieldindex, bool targetHasRawInvoke)
		{
			return PWObject(objPointer).ReadField(builder, fieldindex, targetHasRawInvoke);
		}
		llvm::Value* ObjectHeader::ReadField(NomBuilder& builder, llvm::Value* objPointer, llvm::Value* fieldindex, bool targetHasRawInvoke)
		{
			return PWObject(objPointer).ReadField(builder, fieldindex, targetHasRawInvoke);
		}
		void ObjectHeader::WriteField(NomBuilder& builder, llvm::Value* objPointer, int32_t fieldindex, llvm::Value* val, bool targetHasRawInvoke)
		{
			PWObject(objPointer).WriteField(builder, fieldindex, val, targetHasRawInvoke);
		}
		void ObjectHeader::WriteField(NomBuilder& builder, llvm::Value* objPointer, llvm::Value* fieldindex, llvm::Value* val, bool targetHasRawInvoke)
		{
			PWObject(objPointer).WriteField(builder, fieldindex, val, targetHasRawInvoke);
		}
		llvm::Value* ObjectHeader::GenerateReadVTablePointer(NomBuilder& builder, llvm::Value* objPointer)
		{
			return PWObject(objPointer).ReadVTable(builder);
		}
		llvm::Value* ObjectHeader::GenerateSetClassDescriptor(NomBuilder& builder, llvm::Value* ohref, size_t fieldCount, llvm::Value* desc)
		{
			RefValueHeader::GenerateWriteVTablePointer(builder, ohref, builder->CreatePointerCast(desc, RTVTable::GetLLVMType()->getPointerTo()));
			return ohref;
		}
		llvm::Value* ObjectHeader::CreateDictionaryLoad(NomBuilder& builder, CompileEnv* env, NomValue& receiver, llvm::ConstantInt* key, const llvm::Twine keyName)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncDynamicFieldLookups(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			auto recType = receiver->getType();
			if (recType->isIntegerTy(1))
			{
				throw new std::exception();
			}
			else if (recType->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()))
			{
				throw new std::exception();
			}
			else if (recType->isFloatingPointTy())
			{
				throw new std::exception();
			}
			else if (!(recType->isPointerTy()))
			{
				throw new std::exception();
			}
			auto vTableVar = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
			//BasicBlock* classBlock = nullptr, * lambdaBlock = nullptr, * structBlock = nullptr, * partialAppBlock = nullptr;
			//Value* vTableVar = nullptr, *sTableVar = nullptr;
			//Function* fun = builder->GetInsertBlock()->getParent();

			//RefValueHeader::GenerateRefValueKindSwitch(builder, receiver, &vTableVar, &classBlock, &lambdaBlock, &structBlock, &partialAppBlock);
			//BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "DictLoadOut", fun);

			//builder->SetInsertPoint(outBlock);
			//auto outPHI = builder->CreatePHI(REFTYPE, 2, "dictLoadValue");
			//
			//if (classBlock != nullptr)
			//{
			//	builder->SetInsertPoint(classBlock);
			//	vTableVar->setName("classDescriptor");
				auto fieldLookupFun = RTVTable::GenerateReadReadFieldFunction(builder, vTableVar);
				auto fieldValue = builder->CreateCall(GetFieldReadFunctionType(), fieldLookupFun, { key, receiver }, "fieldValue");
				fieldValue->setCallingConv(NOMCC);
			//	outPHI->addIncoming(fieldValue, builder->GetInsertBlock());
			//	builder->CreateBr(outBlock);
			//}

			//if (structBlock != nullptr)
			//{
			//	builder->SetInsertPoint(structBlock);
			//	auto structFieldLookupFun = RTVTable::GenerateReadReadFieldFunction(builder, vTableVar);
			//	auto structFieldValue = builder->CreateCall(GetFieldReadFunctionType(), structFieldLookupFun, { key, receiver }, "structFieldValue");
			//	structFieldValue->setCallingConv(NOMCC);
			//	outPHI->addIncoming(structFieldValue, builder->GetInsertBlock());
			//	builder->CreateBr(outBlock);
			//}

			//static const char* structnotImplementedMsg = "Wrong kind of entity for dictionary lookup!";
			//BasicBlock* wrongTag = RTOutput_Fail::GenerateFailOutputBlock(builder, structnotImplementedMsg);

			//if (lambdaBlock != nullptr)
			//{
			//	builder->SetInsertPoint(lambdaBlock);
			//	builder->CreateBr(wrongTag);
			//}
			//if (partialAppBlock != nullptr)
			//{
			//	builder->SetInsertPoint(partialAppBlock);
			//	builder->CreateBr(wrongTag);
			//}

			//builder->SetInsertPoint(outBlock);
			return fieldValue;

	
		}
		void ObjectHeader::EnsureExternalReadWriteFieldFunctions(llvm::Module* mod)
		{
			static llvm::Function* readfun = llvm::Function::Create(llvm::FunctionType::get(REFTYPE, { REFTYPE, inttype(32) }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_READFIELD", mod);
			static llvm::Function* writefun = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { REFTYPE, inttype(32), REFTYPE }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_WRITEFIELD", mod);
			static llvm::Function* fieldaddrfun = llvm::Function::Create(llvm::FunctionType::get(REFTYPE->getPointerTo(), { REFTYPE, inttype(32) }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_GETFIELDADDR", mod);
			
			static llvm::Function* readfun_forinvoke = llvm::Function::Create(llvm::FunctionType::get(REFTYPE, { REFTYPE, inttype(32) }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_READFIELD_FORINVOKABLE", mod);
			static llvm::Function* writefun_forinvoke = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { REFTYPE, inttype(32), REFTYPE }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_WRITEFIELD_FORINVOKABLE", mod);
			static llvm::Function* readtargfun = llvm::Function::Create(llvm::FunctionType::get(TYPETYPE, { REFTYPE, inttype(32) }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_READTYPEARG", mod);
			static llvm::Function* writetargfun = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { REFTYPE, inttype(32), TYPETYPE }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_WRITETYPEARG", mod);
			static llvm::Function* writeVtableFun = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { REFTYPE, POINTERTYPE }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_WRITEVTABLE", mod);
			static bool onlyonce = true;
			if (onlyonce)
			{
				onlyonce = false;
				NomBuilder builder;

				llvm::BasicBlock* readBlock = llvm::BasicBlock::Create(LLVMCONTEXT, "", readfun);
				auto readargs = readfun->arg_begin();
				llvm::Argument* readrec = readargs;
				readargs++;
				llvm::Argument* readindex = readargs;
				builder->SetInsertPoint(readBlock);
				//llvm::LoadInst* load = MakeLoad(builder, *mod, builder->CreateGEP(builder->CreatePointerCast(readrec, ObjectHeader::GetLLVMType()->getPointerTo()), { {MakeInt32(0), MakeInt32((unsigned int)ObjectHeaderFields::Fields), builder->CreateNeg(builder->CreateAdd(MakeInt32(1), readindex))} }));
				llvm::Value* load = ObjectHeader::ReadField(builder, readrec, readindex, false);
				builder->CreateRet(load);

				llvm::BasicBlock* writeBlock = llvm::BasicBlock::Create(LLVMCONTEXT, "", writefun);
				auto writeargs = writefun->arg_begin();
				llvm::Argument* writerec = writeargs;
				writeargs++;
				llvm::Argument* writeindex = writeargs;
				writeargs++;
				llvm::Argument* writevalue = writeargs;
				builder->SetInsertPoint(writeBlock);
				/*llvm::StoreInst* store =*/ //MakeStore(builder, *mod, writevalue, builder->CreateGEP(builder->CreatePointerCast(writerec, ObjectHeader::GetLLVMType()->getPointerTo()), { {MakeInt32(0), MakeInt32((unsigned int)ObjectHeaderFields::Fields), builder->CreateNeg(builder->CreateAdd(MakeInt32(1), writeindex))} }));
				ObjectHeader::WriteField(builder, writerec, writeindex, writevalue, false);
				builder->CreateRetVoid();


				llvm::BasicBlock* fieldAddrBlock = llvm::BasicBlock::Create(LLVMCONTEXT, "", fieldaddrfun);
				auto fieldaddrargs = fieldaddrfun->arg_begin();
				llvm::Argument* fieldAddrRec = fieldaddrargs;
				fieldaddrargs++;
				llvm::Argument* fieldAddrIndex = fieldaddrargs;
				builder->SetInsertPoint(fieldAddrBlock);
				llvm::Value* fieldAddr = builder->CreateGEP(ObjectHeader::GetLLVMType(), fieldAddrRec, { MakeInt32(0), MakeInt32(ObjectHeaderFields::Fields), fieldAddrIndex });
				builder->CreateRet(fieldAddr);

				llvm::BasicBlock* readBlock_forinvoke = llvm::BasicBlock::Create(LLVMCONTEXT, "", readfun_forinvoke);
				auto readargs_forinvoke = readfun_forinvoke->arg_begin();
				llvm::Argument* readrec_forinvoke = readargs_forinvoke;
				readargs_forinvoke++;
				llvm::Argument* readindex_forinvoke = readargs_forinvoke;
				builder->SetInsertPoint(readBlock_forinvoke);
				//llvm::LoadInst* load = MakeLoad(builder, *mod, builder->CreateGEP(builder->CreatePointerCast(readrec, ObjectHeader::GetLLVMType()->getPointerTo()), { {MakeInt32(0), MakeInt32((unsigned int)ObjectHeaderFields::Fields), builder->CreateNeg(builder->CreateAdd(MakeInt32(1), readindex))} }));
				llvm::Value* load_forinvoke = ObjectHeader::ReadField(builder, readrec_forinvoke, readindex_forinvoke, (NomLambdaOptimizationLevel>0));
				builder->CreateRet(load_forinvoke);


				llvm::BasicBlock* writeBlock_forinvoke = llvm::BasicBlock::Create(LLVMCONTEXT, "", writefun_forinvoke);
				auto writeargs_forinvoke = writefun_forinvoke->arg_begin();
				llvm::Argument* writerec_forinvoke = writeargs_forinvoke;
				writeargs_forinvoke++;
				llvm::Argument* writeindex_forinvoke = writeargs_forinvoke;
				writeargs_forinvoke++;
				llvm::Argument* writevalue_forinvoke = writeargs_forinvoke;
				builder->SetInsertPoint(writeBlock_forinvoke);
				/*llvm::StoreInst* store =*/ //MakeStore(builder, *mod, writevalue, builder->CreateGEP(builder->CreatePointerCast(writerec, ObjectHeader::GetLLVMType()->getPointerTo()), { {MakeInt32(0), MakeInt32((unsigned int)ObjectHeaderFields::Fields), builder->CreateNeg(builder->CreateAdd(MakeInt32(1), writeindex))} }));
				ObjectHeader::WriteField(builder, writerec_forinvoke, writeindex_forinvoke, writevalue_forinvoke, (NomLambdaOptimizationLevel > 0));
				builder->CreateRetVoid();

				llvm::BasicBlock* readtargBlock = llvm::BasicBlock::Create(LLVMCONTEXT, "", readtargfun);
				auto readtargargs = readtargfun->arg_begin();
				llvm::Argument* readtargrec = readtargargs;
				readtargargs++;
				llvm::Argument* readtargindex = readtargargs;
				builder->SetInsertPoint(readtargBlock);
				llvm::LoadInst* loadtarg = MakeLoad(builder, *mod, TYPETYPE, builder->CreateGEP(ObjectHeader::GetLLVMType(), readtargrec, { {MakeInt32(0), MakeInt32((unsigned int)ObjectHeaderFields::TypeArgs), builder->CreateSub(MakeInt32(-1),readtargindex)} }));
				builder->CreateRet(loadtarg);


				llvm::BasicBlock* writetargBlock = llvm::BasicBlock::Create(LLVMCONTEXT, "", writetargfun);
				auto writetargargs = writetargfun->arg_begin();
				llvm::Argument* writetargrec = writetargargs;
				writetargargs++;
				llvm::Argument* writetargindex = writetargargs;
				writetargargs++;
				llvm::Argument* writetargvalue = writetargargs;
				builder->SetInsertPoint(writetargBlock);
				/*llvm::StoreInst* store =*/ MakeStore(builder, *mod, writetargvalue, builder->CreateGEP(ObjectHeader::GetLLVMType(), writetargrec, { {MakeInt32(0), MakeInt32((unsigned int)ObjectHeaderFields::TypeArgs), builder->CreateSub(MakeInt32(-1),writetargindex)} }));
				builder->CreateRetVoid();

				llvm::BasicBlock* writevtBlock = llvm::BasicBlock::Create(LLVMCONTEXT, "", writeVtableFun);
				auto writevtargs = writeVtableFun->arg_begin();
				llvm::Argument* writevtrec = writevtargs;
				writevtargs++;
				llvm::Argument* writevtvtable = writevtargs;
				builder->SetInsertPoint(writevtBlock);
				MakeStore(builder, writevtvtable, NLLVMValue(builder->CreateGEP(RefValueHeader::GetLLVMType(), writevtrec, { MakeInt32(0), MakeInt32(RefValueHeaderFields::InterfaceTable) }), POINTERTYPE->getPointerTo()));
				builder->CreateRetVoid();

				if (verifyFunction(*readfun))
				{
					std::cout << "Could not verify read-field function!";
					llvm::raw_os_ostream out(std::cout);
					readfun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
				if (verifyFunction(*writefun))
				{
					std::cout << "Could not verify write-field function!";
					llvm::raw_os_ostream out(std::cout);
					writefun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
				if (verifyFunction(*fieldaddrfun))
				{
					std::cout << "Could not verify field-addr function!";
					llvm::raw_os_ostream out(std::cout);
					fieldaddrfun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
				if (verifyFunction(*readtargfun))
				{
					std::cout << "Could not verify read-type-arg function!";
					llvm::raw_os_ostream out(std::cout);
					readfun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
				if (verifyFunction(*writetargfun))
				{
					std::cout << "Could not verify write-type-arg function!";
					llvm::raw_os_ostream out(std::cout);
					writefun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
				if (verifyFunction(*writeVtableFun))
				{
					std::cout << "Could not verify write-vtable function!";
					llvm::raw_os_ostream out(std::cout);
					writeVtableFun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
		}

		/**
		* Calculates the address of the function pointer to the method in the method table. This address needs to be cast to the correct type
		* (a pointer to the correct function pointer type) and then loaded to obtain the actual function pointer.
		*/
		Value* ObjectHeader::GetDispatchMethodPointer(NomBuilder& builder, CompileEnv* env, RegIndex reg, int lineno, NomInstantiationRef<const NomMethod> method)
		{
			PWRefValue objectHeader = PWRefValue((*env)[reg]);
			PWVTable clsdescptr = objectHeader.ReadVTable(builder);
			PWClass clsdescptr_typed = PWClass::FromVTable(clsdescptr);
			Value* funaddr = RTVTable::GenerateReadMethodTableEntry(builder, clsdescptr_typed, MakeInt32(method.Elem->GetOffset()));

			Value* funaddr_typed = builder->CreatePointerCast( funaddr, method.Elem->GetLLVMFunctionType()->getPointerTo());

			return funaddr_typed;

		}


		llvm::Value* ObjectHeader::GetInterfaceMethodTableFunction(NomBuilder& builder, CompileEnv* env, RegIndex reg, llvm::Constant * index, int lineno)
		{
			return RefValueHeader::GetInterfaceMethodTableFunction(builder, env, reg, index, lineno);
		}
	}
}