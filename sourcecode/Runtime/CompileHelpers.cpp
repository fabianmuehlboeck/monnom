#include "CompileHelpers.h"
#include "CompileEnv.h"
#include "NomJIT.h"
#include "NomJITLight.h"
#include "CallingConvConf.h"
#include "Metadata.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		CallInst* GenerateFunctionCall(NomBuilder& builder, Module& mod, Value* fun, FunctionType* ftype, ArrayRef<Value*>& args, bool fastcall)
		{
			size_t argsize = args.size();
			Value** pargs = makealloca(Value*, argsize);
			for (size_t i = 0; i != argsize; i++)
			{
				Type* argtype = args[i]->getType();
				Type* paramtype = ftype->getParamType(i);
				if (argtype == paramtype)
				{
					pargs[i] = args[i];
					continue;
				}
				if (paramtype == POINTERTYPE)
				{
					if (!argtype->isPointerTy())
					{
						throw new exception();
					}
					pargs[i] = builder->CreatePointerCast(args[i], POINTERTYPE);
				}
				else if (paramtype == INTTYPE)
				{
					if (argtype != REFTYPE)
					{
						throw new exception();
					}
					pargs[i] = UnpackInt(builder, args[i]);
					continue;
				}
				else if (paramtype == FLOATTYPE)
				{
					if (argtype != REFTYPE)
					{
						throw new exception();
					}
					pargs[i] = UnpackFloat(builder, args[i]);
					continue;
				}
				else if (paramtype->isIntegerTy(1))
				{
					if (argtype == REFTYPE)
					{
						pargs[i] = builder->CreateTrunc(UnpackInt(builder, args[i]), paramtype);
						continue;
					}
					else if (argtype == INTTYPE)
					{
						pargs[i] = builder->CreateTrunc(args[i], paramtype);
						continue;
					}
					throw new exception();
				}
				else if (paramtype == REFTYPE)
				{
					if (argtype == INTTYPE)
					{
						pargs[i] = PackInt(builder, args[i]);
						continue;
					}
					else if (argtype == FLOATTYPE)
					{
						pargs[i] = PackFloat(builder, args[i]);
						continue;
					}
					else if (argtype->isPointerTy())
					{
						pargs[i] = builder->CreatePointerCast(args[i], REFTYPE);
						continue;
					}
					throw new exception();
				}
			}
			CallInst* ci = builder->CreateCall(ftype, fun, llvm::ArrayRef<llvm::Value*>(pargs, argsize));
			if (fastcall)
			{
				ci->setCallingConv(NOMCC);
			}
			return ci;
		}

		CallInst* GenerateFunctionCall(NomBuilder& builder, Module& mod, Function* fun, ArrayRef<Value*>& args, bool fastcall)
		{
			size_t argsize = args.size();
			Value** pargs = makealloca(Value*, argsize);
			llvm::FunctionType* ftype = fun->getFunctionType();
			for (size_t i = 0; i != argsize; i++)
			{
				Type* argtype = args[i]->getType();
				Type* paramtype = ftype->getParamType(i);
				if (argtype == paramtype)
				{
					pargs[i] = args[i];
					continue;
				}
				if (paramtype == TYPETYPE)
				{
					if (!argtype->isPointerTy())
					{
						throw new exception();
					}
					pargs[i] = builder->CreatePointerCast(args[i], TYPETYPE);
				}
				else if (paramtype == INTTYPE)
				{
					if (argtype != REFTYPE)
					{
						throw new exception();
					}
					pargs[i] = UnpackInt(builder, args[i]);
					continue;
				}
				else if (paramtype == FLOATTYPE)
				{
					if (argtype != REFTYPE)
					{
						throw new exception();
					}
					pargs[i] = UnpackFloat(builder, args[i]);
					continue;
				}
				else if (paramtype->isIntegerTy(1))
				{
					if (argtype == REFTYPE)
					{
						pargs[i] = builder->CreateTrunc(UnpackInt(builder, args[i]), paramtype);
						continue;
					}
					else if (argtype == INTTYPE)
					{
						pargs[i] = builder->CreateTrunc(args[i], paramtype);
						continue;
					}
					throw new exception();
				}
				else if (paramtype == REFTYPE)
				{
					if (argtype == INTTYPE)
					{
						pargs[i] = PackInt(builder, args[i]);
						continue;
					}
					else if (argtype == FLOATTYPE)
					{
						pargs[i] = PackFloat(builder, args[i]);
						continue;
					}
					else if (argtype->isPointerTy())
					{
						pargs[i] = builder->CreatePointerCast(args[i], REFTYPE);
						continue;
					}
					throw new exception();
				}
			}
			CallInst* ci = builder->CreateCall(fun, llvm::ArrayRef<llvm::Value*>(pargs, argsize));
			if (fastcall)
			{
				ci->setCallingConv(NOMCC);
			}
			return ci;
		}
		llvm::Value* GenerateIsNull(NomBuilder& builder, llvm::Value* val)
		{
			if (!val->getType()->isPointerTy())
			{
				throw new std::exception();
			}
			return builder->CreateICmpEQ(builder->CreatePtrToInt(val, numtype(intptr_t)), ConstantExpr::getPtrToInt(ConstantPointerNull::get((PointerType*)val->getType()), numtype(intptr_t)), "ISNULL");
		}
		void CreateDummyReturn(NomBuilder& builder, llvm::Function* fun)
		{
			auto rettype = fun->getReturnType();
			if (rettype->isVoidTy())
			{
				builder->CreateRetVoid();
				return;
			}
			if (rettype->isIntegerTy())
			{
				builder->CreateRet(ConstantInt::get(rettype, 0));
				return;
			}
			if (rettype->isFloatingPointTy())
			{
				builder->CreateRet(ConstantFP::get(rettype, 0.0));
				return;
			}
			if (rettype->isPointerTy())
			{
				builder->CreateRet(ConstantPointerNull::get((PointerType*)rettype));
				return;
			}
			if (rettype->isStructTy())
			{
				builder->CreateRet(UndefValue::get(rettype));
				return;
			}
			throw new std::exception();

		}

		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Module& mod, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering)
		{
			//std::cout << "Storing value of type";
			//llvm::raw_os_ostream out(std::cout);
			//val->getType()->print(out);
			//out.flush();
			//std::cout << "into field of type";
			//ptr->getType()->print(out);
			//out.flush();
			//std::cout.flush();
			/*llvm::Value* pval = val;
			if (pval->getType()->isIntegerTy()&&pval->getType()!=INTTYPE)
			{
				pval = builder->CreateSExtOrTrunc(pval, INTTYPE);
			}
			if (pval->getType() == INTTYPE)
			{
				pval = builder->CreateIntToPtr(pval, POINTERTYPE);
			}
			else if (pval->getType() == FLOATTYPE)
			{
				pval = builder->CreateIntToPtr(builder->CreateBitCast(pval, INTTYPE), POINTERTYPE);
			}
			builder->CreateCall(mod.getFunction("RT_NOM_PRINT_STORE"), { {builder->CreatePointerCast(pval,POINTERTYPE),builder->CreatePointerCast(ptr,POINTERTYPE)} });*/
			llvm::StoreInst* store = builder->CreateStore(val, ptr);
			store->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(val->getType())));
			store->setAtomic(ordering);
			return store;
		}

		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering)
		{
			return MakeStore(builder, *builder->GetInsertBlock()->getParent()->getParent(), val, ptr, ordering);
		}

		//Adds a zero index in front of the given indices list
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering)
		{
			auto indexcount = indices.size();
			llvm::Value** indexbuf = makealloca(llvm::Value*, indexcount + 1);
			indexbuf[0] = MakeInt32(0);
			for (size_t i = 0; i < indexcount; i++)
			{
				indexbuf[i + 1] = builder->CreateZExtOrTrunc(indices[i], inttype(32));
			}
			llvm::Value* actualpointer = builder->CreateGEP(ptr, llvm::ArrayRef<llvm::Value*>(indexbuf, indexcount + 1));
			llvm::StoreInst* store = builder->CreateStore(val, actualpointer);
			store->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(((PointerType*)actualpointer->getType())->getElementType())));
			store->setAtomic(ordering);
			return store;
		}

		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::PointerType* asType, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering)
		{
			return MakeStore(builder, val, builder->CreatePointerCast(ptr, asType), indices, ordering);
		}

		//Adds a zero index in front of the given index
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::Value* index, llvm::AtomicOrdering ordering)
		{
			llvm::Value* actualpointer = builder->CreateGEP(ptr, { MakeInt32(0), builder->CreateZExtOrTrunc(index, inttype(32)) });
			llvm::StoreInst* store = builder->CreateStore(val, actualpointer);
			store->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(((PointerType*)actualpointer->getType())->getElementType())));
			store->setAtomic(ordering);
			return store;
		}

		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::PointerType* asType, llvm::Value* index, llvm::AtomicOrdering ordering)
		{
			return MakeStore(builder, val, builder->CreatePointerCast(ptr, asType), index, ordering);
		}

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Module& mod, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, mod, val, ptr, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, val, ptr, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, val, ptr, indices, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::PointerType* asType, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, val, ptr, asType, indices, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::Value* index, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, val, ptr, index, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::PointerType* asType, llvm::Value* index, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, val, ptr, asType, index, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}

		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, llvm::AtomicOrdering ordering)
		{

			llvm::LoadInst* load = builder->CreateLoad(ptr);
			/*llvm::Value* pval = load;
			if (pval->getType()->isIntegerTy() && pval->getType() != INTTYPE)
			{
				pval = builder->CreateSExtOrTrunc(pval, INTTYPE);
			}
			if (pval->getType() == INTTYPE)
			{
				pval = builder->CreateIntToPtr(pval, POINTERTYPE);
			}
			else if (pval->getType() == FLOATTYPE)
			{
				pval = builder->CreateIntToPtr(builder->CreateBitCast(pval, INTTYPE), POINTERTYPE);
			}
			builder->CreateCall(mod.getFunction("RT_NOM_PRINT_LOAD"), { {builder->CreatePointerCast(pval,POINTERTYPE), builder->CreatePointerCast(ptr,POINTERTYPE)} });*/
			//load->setAtomic(ordering);
			load->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(((PointerType*)ptr->getType())->getElementType())));
			load->setAtomic(ordering);
			return load;
		}

		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto li = MakeLoad(builder, *builder->GetInsertBlock()->getParent()->getParent(), ptr, ordering);;
			li->setName(name);
			return li;
		}

		//Adds a zero index in front of the given indices list
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto indexcount = indices.size();
			llvm::Value** indexbuf = makealloca(llvm::Value*, indexcount + 1);
			indexbuf[0] = MakeInt32(0);
			for (size_t i = 0; i < indexcount; i++)
			{
				indexbuf[i + 1] = builder->CreateZExtOrTrunc(indices[i], inttype(32));
			}
			llvm::Value* actualpointer = builder->CreateGEP(ptr, llvm::ArrayRef<llvm::Value*>(indexbuf, indexcount + 1));
			llvm::LoadInst* load = builder->CreateLoad(actualpointer, name);
			load->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(((PointerType*)actualpointer->getType())->getElementType())));
			load->setAtomic(ordering);
			return load;
		}

		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::PointerType* asType, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			return MakeLoad(builder, builder->CreatePointerCast(ptr, asType), indices, name, ordering);
		}

		//Adds a zero index in front of the given index
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::Value* index, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			llvm::Value* actualpointer = builder->CreateGEP(ptr, { MakeInt32(0), builder->CreateZExtOrTrunc(index, inttype(32)) });
			llvm::LoadInst* load = builder->CreateLoad(actualpointer, name);
			load->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(((PointerType*)actualpointer->getType())->getElementType())));
			load->setAtomic(ordering);
			return load;
		}

		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::PointerType* asType, llvm::Value* index, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			return MakeLoad(builder, builder->CreatePointerCast(ptr, asType), index, name, ordering);
		}


		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, llvm::AtomicOrdering ordering)
		{
			auto inst = MakeLoad(builder, mod, ptr, ordering);
			inst->setMetadata("invariant.group", getGeneralInvariantNode());
			return inst;
		}
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Value* ptr, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto inst = MakeLoad(builder, ptr, name, ordering);
			inst->setMetadata("invariant.group", getGeneralInvariantNode());
			return inst;
		}
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto inst = MakeLoad(builder, ptr, indices, name, ordering);
			inst->setMetadata("invariant.group", getGeneralInvariantNode());
			return inst;
		}
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Value* ptr, llvm::PointerType* asType, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto inst = MakeLoad(builder, ptr, asType, indices, name, ordering);
			inst->setMetadata("invariant.group", getGeneralInvariantNode());
			return inst;
		}
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Value* ptr, llvm::Value* index, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto inst = MakeLoad(builder, ptr, index, name, ordering);
			inst->setMetadata("invariant.group", getGeneralInvariantNode());
			return inst;
		}


		ReadFieldFunction GetReadFieldFunction()
		{
			static ReadFieldFunction fun = (ReadFieldFunction)(intptr_t)(NomJIT::Instance().lookup("RT_NOM_READFIELD")->getAddress());
			return fun;
		}
		WriteFieldFunction GetWriteFieldFunction()
		{
			static WriteFieldFunction fun = (WriteFieldFunction)(intptr_t)(NomJIT::Instance().lookup("RT_NOM_WRITEFIELD")->getAddress());
			return fun;
		}
		FieldAddrFunction GetFieldAddrFunction()
		{
			static FieldAddrFunction fun = (FieldAddrFunction)(intptr_t)(NomJIT::Instance().lookup("RT_NOM_GETFIELDADDR")->getAddress());
			return fun;
		}
		ReadFieldFunction GetReadFieldFunction_ForInvoke()
		{
			static ReadFieldFunction fun = (ReadFieldFunction)(intptr_t)(NomJIT::Instance().lookup("RT_NOM_READFIELD_FORINVOKABLE")->getAddress());
			return fun;
		}
		WriteFieldFunction GetWriteFieldFunction_ForInvoke()
		{
			static WriteFieldFunction fun = (WriteFieldFunction)(intptr_t)(NomJIT::Instance().lookup("RT_NOM_WRITEFIELD_FORINVOKABLE")->getAddress());
			return fun;
		}
		ReadTypeArgFunction GetReadTypeArgFunction()
		{
			static ReadTypeArgFunction fun = (ReadTypeArgFunction)(intptr_t)(NomJIT::Instance().lookup("RT_NOM_READTYPEARG")->getAddress());
			return fun;
		}
		WriteTypeArgFunction GetWriteTypeArgFunction()
		{
			static WriteTypeArgFunction fun = (WriteTypeArgFunction)(intptr_t)(NomJIT::Instance().lookup("RT_NOM_WRITETYPEARG")->getAddress());
			return fun;
		}
		WriteVTableFunction GetWriteVTableFunction()
		{
			static WriteVTableFunction fun = (WriteVTableFunction)(intptr_t)(NomJIT::Instance().lookup("RT_NOM_WRITEVTABLE")->getAddress());
			return fun;
		}
		void* GetGeneralLLVMFunction(llvm::StringRef name)
		{
			return (void*)(intptr_t)(NomJIT::Instance().lookup(name)->getAddress());
		}
		void* GetBooleanTrue()
		{
			static void* bval = (void*)(*((intptr_t*)(NomJIT::Instance().lookup("RT_NOM_TRUE")->getAddress())));
			return bval;
		}
		void* GetBooleanFalse()
		{
			static void* bval = (void*)(*((intptr_t*)(NomJIT::Instance().lookup("RT_NOM_FALSE")->getAddress())));
			return bval;
		}
		void* GetVoidObj()
		{
			static void* voidobj = (void*)((intptr_t)(Nom::Runtime::NomJIT::Instance().getSymbolAddress("RT_NOM_VOIDOBJ")));
			return voidobj;
		}


		llvm::Constant* EnsureIntegerSize(llvm::Constant* cnst, int bitwidth)
		{
			int size = cnst->getType()->getPrimitiveSizeInBits();
			if (size < bitwidth)
			{
				cnst = ConstantExpr::getZExt(cnst, inttype(bitwidth));
			}
			else if (size > bitwidth)
			{
				cnst = ConstantExpr::getTrunc(cnst, inttype(bitwidth));
			}
			return cnst;
		}
		llvm::Value* EnsureIntegerSize(NomBuilder& builder, llvm::Value* val, int bitwidth)
		{
			int size = val->getType()->getPrimitiveSizeInBits();
			if (size < bitwidth)
			{
				val = builder->CreateZExt(val, inttype(bitwidth));
			}
			else if (size > bitwidth)
			{
				val = builder->CreateTrunc(val, inttype(bitwidth));
			}
			return val;
		}
		llvm::Value* UnmaskDictionaryValue(NomBuilder& builder, llvm::Value* val)
		{
			BasicBlock* incoming = builder->GetInsertBlock();
			BasicBlock* ref = BasicBlock::Create(LLVMCONTEXT, "unmaskRef", incoming->getParent());
			BasicBlock* continueBlock = BasicBlock::Create(LLVMCONTEXT, "continue", incoming->getParent());
			auto tag = builder->CreateTrunc(builder->CreatePtrToInt(val, numtype(intptr_t)), inttype(2), "tag");
			builder->CreateCondBr(builder->CreateICmpEQ(tag, MakeUInt(2, 0)), ref, continueBlock);

			builder->SetInsertPoint(ref);
			auto unmaskedRef = builder->CreateIntToPtr(builder->CreateAnd(builder->CreatePtrToInt(val, numtype(intptr_t)), GetMask(64, 0, 3)), REFTYPE);
			builder->CreateBr(continueBlock);

			builder->SetInsertPoint(continueBlock);
			auto resultPHI = builder->CreatePHI(REFTYPE, 2, "unmaskedDictValue");
			resultPHI->addIncoming(val, incoming);
			resultPHI->addIncoming(unmaskedRef, ref);
			return resultPHI;
		}
		llvm::Constant* MakeLLVMStruct(llvm::ArrayRef<llvm::Constant*> args)
		{
			llvm::Type** argtypes = makealloca(llvm::Type*, args.size());
			for (size_t i = args.size(); i > 0;)
			{
				i--;
				argtypes[i] = args[i]->getType();
			}
			return llvm::ConstantStruct::get(llvm::StructType::get(LLVMCONTEXT, llvm::ArrayRef<llvm::Type*>(argtypes, args.size())), args);
		}
		llvm::ConstantInt* MakeUInt(size_t size, uint64_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, size), val, false);
		}
		llvm::ConstantInt* MakeSInt(size_t size, int64_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, size), val, true);
		}
		llvm::ConstantInt* MakeInt(size_t size, int64_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, size), static_cast<uint64_t>(val), true);
		}
		llvm::ConstantInt* MakeInt32(int32_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, 32), bit_cast<uint32_t, int32_t>(val), true);
		}
		llvm::ConstantInt* MakeInt32(uint32_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, 32), val, true);
		}
		llvm::ConstantInt* MakeInt32(uint64_t val)
		{
			return MakeInt32((uint32_t)val);
		}
		llvm::ConstantInt* MakeInt32(int64_t val)
		{
			return MakeInt32((int32_t)val);
		}
		llvm::ConstantInt* MakeInt(size_t size, uint64_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, size), val, false);
		}
		llvm::ConstantInt* MakeIntLike(llvm::Value* value, uint64_t val)
		{
			if (!value->getType()->isIntegerTy())
			{
				throw new std::exception();//TODO: Exception
			}
			return ConstantInt::get((llvm::IntegerType*)(value->getType()), val);
		}
		llvm::Constant* GetLLVMRef(const void* const ptr)
		{
			if (ptr == nullptr)
			{
				return llvm::ConstantPointerNull::get(REFTYPE);
			}
			return llvm::ConstantExpr::getIntToPtr(llvm::ConstantInt::get(llvm::Type::getIntNTy(LLVMCONTEXT, bitsin(void*)), reinterpret_cast<uint64_t>(ptr), false), REFTYPE);
		}
		llvm::Constant* GetLLVMPointer(const void* const ptr)
		{
			if (ptr == nullptr)
			{
				return llvm::ConstantPointerNull::get(POINTERTYPE);
			}
			return llvm::ConstantExpr::getIntToPtr(llvm::ConstantInt::get(llvm::Type::getIntNTy(LLVMCONTEXT, bitsin(void*)), reinterpret_cast<uint64_t>(ptr), false), POINTERTYPE);
		}
		llvm::Constant* GetMask(int length, int leadingZeroes, int trailingZeroes)
		{
			auto ret = ConstantExpr::getXor(llvm::ConstantInt::getAllOnesValue(IntegerType::get(LLVMCONTEXT, length - leadingZeroes)), ConstantExpr::getZExt(llvm::ConstantInt::getAllOnesValue(IntegerType::get(LLVMCONTEXT, trailingZeroes)), IntegerType::get(LLVMCONTEXT, length - leadingZeroes)));
			if (leadingZeroes > 0)
			{
				ret = ConstantExpr::getZExt(ret, IntegerType::get(LLVMCONTEXT, length));
			}
			return ret;
		}
		llvm::Value* CreatePointerEq(NomBuilder& builder, llvm::Value* left, llvm::Value* right, const llvm::Twine& name)
		{
			return builder->CreateICmpEQ(builder->CreatePtrToInt(left, numtype(intptr_t)), builder->CreatePtrToInt(right, numtype(intptr_t)), name);
		}
	}
}