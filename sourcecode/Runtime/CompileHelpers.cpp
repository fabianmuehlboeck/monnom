PUSHDIAGSUPPRESSION
POPDIAGSUPPRESSION
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
		llvm::Type* GetGEPTargetType(llvm::Type* origType, llvm::ArrayRef<llvm::Value*> arr)
		{
			if (arr.size() <= 1)
			{
				return origType;
			}
			llvm::Type* nextOrig = nullptr;
			if (origType->isStructTy())
			{
				nextOrig = origType->getStructElementType(static_cast<unsigned int>(*(static_cast<ConstantInt*>(arr[1]))->getValue().getRawData()));
			}
			else if (origType->isArrayTy())
			{
				nextOrig = origType->getArrayElementType();
			}
			else
			{
				throw "Invalid GEP target";
			}
			return GetGEPTargetType(nextOrig, arr.take_back(arr.size() - 1));
		}
		llvm::Type* GetGEPTargetType(llvm::Type* origType, llvm::ArrayRef<llvm::Constant*> arr)
		{
			if (arr.size() <= 1)
			{
				return origType;
			}
			llvm::Type* nextOrig = nullptr;
			if (origType->isStructTy())
			{
				nextOrig = origType->getStructElementType(static_cast<unsigned int>(*(static_cast<ConstantInt*>(arr[1]))->getValue().getRawData()));
			}
			else if (origType->isArrayTy())
			{
				nextOrig = origType->getArrayElementType();
			}
			else
			{
				throw "Invalid GEP target";
			}
			return GetGEPTargetType(nextOrig, arr.take_back(arr.size() - 1));
		}
		CallInst* GenerateFunctionCall(NomBuilder& builder, [[maybe_unused]] Module& mod, Value* fun, FunctionType* ftype, ArrayRef<Value*>& args, bool fastcall)
		{
			size_t argsize = args.size();
			Value** pargs = makealloca(Value*, argsize);
			for (size_t i = 0; i != argsize; i++)
			{
				Type* argtype = args[i]->getType();
				Type* paramtype = ftype->getParamType(static_cast<unsigned int>(i));
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

		CallInst* GenerateFunctionCall(NomBuilder& builder, [[maybe_unused]]  Module& mod, Function* fun, ArrayRef<Value*>& args, bool fastcall)
		{
			size_t argsize = args.size();
			Value** pargs = makealloca(Value*, argsize);
			llvm::FunctionType* ftype = fun->getFunctionType();
			for (size_t i = 0; i != argsize; i++)
			{
				Type* argtype = args[i]->getType();
				Type* paramtype = ftype->getParamType(static_cast<unsigned int>(i));
				if (argtype == paramtype)
				{
					pargs[i] = args[i];
					continue;
				}
				if (paramtype == INTTYPE)
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
					//else if (argtype->isPointerTy())
					//{
					//	pargs[i] = builder->CreatePointerCast(args[i], REFTYPE);
					//	continue;
					//}
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
			return builder->CreateICmpEQ(builder->CreatePtrToInt(val, numtype(intptr_t)), ConstantExpr::getPtrToInt(ConstantPointerNull::get(static_cast<PointerType*>(val->getType())), numtype(intptr_t)), "ISNULL");
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
				builder->CreateRet(ConstantPointerNull::get(static_cast<PointerType*>(rettype)));
				return;
			}
			if (rettype->isStructTy())
			{
				builder->CreateRet(UndefValue::get(rettype));
				return;
			}
			throw new std::exception();

		}

		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering)
		{
			llvm::StoreInst* store = builder->CreateStore(val, ptr);
			store->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(val->getType())));
			store->setAtomic(ordering);
			return store;
		}

		//Adds a zero index in front of the given indices list
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Type * targetType, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering)
		{
			auto indexcount = indices.size();
			llvm::Value** indexbuf = makealloca(llvm::Value*, indexcount + 1);
			indexbuf[0] = MakeInt32(0);
			for (size_t i = 0; i < indexcount; i++)
			{
				indexbuf[i + 1] = builder->CreateZExtOrTrunc(indices[i], inttype(32));
			}
			llvm::Value* actualpointer = builder->CreateGEP(targetType, ptr, llvm::ArrayRef<llvm::Value*>(indexbuf, indexcount + 1));
			llvm::StoreInst* store = builder->CreateStore(val, actualpointer);
			store->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(val->getType())));
			store->setAtomic(ordering);
			return store;
		}

		//Adds a zero index in front of the given index
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, PWInt32 index, llvm::AtomicOrdering ordering)
		{
			llvm::Value* actualpointer = builder->CreateGEP(targetType, ptr, { MakeInt32(0), builder->CreateZExtOrTrunc(index, inttype(32)) });
			llvm::StoreInst* store = builder->CreateStore(val, actualpointer);
			store->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(val->getType())));
			store->setAtomic(ordering);
			return store;
		}
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, PWCInt32 index, llvm::AtomicOrdering ordering)
		{
			llvm::Value* actualpointer = builder->CreateGEP(targetType, ptr, { MakeInt32(0), builder->CreateZExtOrTrunc(index, inttype(32)) });
			llvm::StoreInst* store = builder->CreateStore(val, actualpointer);
			store->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(val->getType())));
			store->setAtomic(ordering);
			return store;
		}

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, val, ptr, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, val, targetType, ptr, indices, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, PWInt32 index, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, val, targetType, ptr, index, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}
		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, PWCInt32 index, llvm::AtomicOrdering ordering)
		{
			auto storeInst = MakeStore(builder, val, targetType, ptr, index, ordering);
			storeInst->setMetadata("invariant.group", getGeneralInvariantNode());
			return storeInst;
		}

		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			llvm::LoadInst* load = builder->CreateLoad(elementType, ptr);
			load->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(elementType)));
			load->setAtomic(ordering);
			load->setName(name);
			return load;
		}

		//Adds a zero index in front of the given indices list
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto indexcount = indices.size();
			llvm::Value** indexbuf = makealloca(llvm::Value*, indexcount + 1);
			indexbuf[0] = MakeInt32(0);
			for (size_t i = 0; i < indexcount; i++)
			{
				indexbuf[i + 1] = builder->CreateZExtOrTrunc(indices[i], inttype(32));
			}
			llvm::Value* actualpointer = builder->CreateGEP(elementType, ptr, llvm::ArrayRef<llvm::Value*>(indexbuf, indexcount + 1));
			llvm::Type* loadedType = GetGEPTargetType(elementType, llvm::ArrayRef<llvm::Value*>(indexbuf, indexcount + 1));
			llvm::LoadInst* load = builder->CreateLoad(loadedType, actualpointer, name);
			load->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(loadedType)));
			load->setAtomic(ordering);
			return load;
		}

		//Adds a zero index in front of the given index
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, PWInt32 index, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			llvm::Value* actualpointer = builder->CreateGEP(elementType, ptr, { MakeInt32(0), builder->CreateZExtOrTrunc(index, inttype(32)) });
			llvm::Type* loadType = GetGEPTargetType(elementType, ArrayRef<llvm::Value*>({ MakeInt32(0), builder->CreateZExtOrTrunc(index, inttype(32)) }));
			llvm::LoadInst* load = builder->CreateLoad(loadType, actualpointer, name);
			load->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(loadType)));
			load->setAtomic(ordering);
			return load;
		}
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, PWCInt32 index, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			llvm::Value* actualpointer = builder->CreateGEP(elementType, ptr, { MakeInt32(0), builder->CreateZExtOrTrunc(index, inttype(32)) });
			llvm::Type* loadType = GetGEPTargetType(elementType, ArrayRef<llvm::Value*>({ MakeInt32(0), builder->CreateZExtOrTrunc(index, inttype(32)) }));
			llvm::LoadInst* load = builder->CreateLoad(loadType, actualpointer, name);
			load->setAlignment(llvm::Align(GetNomJITDataLayout().getTypeSizeInBits(loadType)));
			load->setAtomic(ordering);
			return load;
		}

		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto inst = MakeLoad(builder, elementType, ptr, name, ordering);
			inst->setMetadata("invariant.group", getGeneralInvariantNode());
			return inst;
		}
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto inst = MakeLoad(builder, elementType, ptr, indices, name, ordering);
			inst->setMetadata("invariant.group", getGeneralInvariantNode());
			return inst;
		}
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, PWInt32 index, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto inst = MakeLoad(builder, elementType, ptr, index, name, ordering);
			inst->setMetadata("invariant.group", getGeneralInvariantNode());
			return inst;
		}
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, PWCInt32 index, llvm::Twine name, llvm::AtomicOrdering ordering)
		{
			auto inst = MakeLoad(builder, elementType, ptr, index, name, ordering);
			inst->setMetadata("invariant.group", getGeneralInvariantNode());
			return inst;
		}


		ReadFieldFunction GetReadFieldFunction()
		{
			static ReadFieldFunction fun = reinterpret_cast<ReadFieldFunction>(reinterpret_cast<uintptr_t>(NomJIT::Instance().lookup("RT_NOM_READFIELD")->getValue()));
			return fun;
		}
		WriteFieldFunction GetWriteFieldFunction()
		{
			static WriteFieldFunction fun = reinterpret_cast<WriteFieldFunction>(reinterpret_cast<uintptr_t>(NomJIT::Instance().lookup("RT_NOM_WRITEFIELD")->getValue()));
			return fun;
		}
		FieldAddrFunction GetFieldAddrFunction()
		{
			static FieldAddrFunction fun = reinterpret_cast<FieldAddrFunction>(reinterpret_cast<uintptr_t>(NomJIT::Instance().lookup("RT_NOM_GETFIELDADDR")->getValue()));
			return fun;
		}
		ReadFieldFunction GetReadFieldFunction_ForInvoke()
		{
			static ReadFieldFunction fun = reinterpret_cast<ReadFieldFunction>(reinterpret_cast<uintptr_t>(NomJIT::Instance().lookup("RT_NOM_READFIELD_FORINVOKABLE")->getValue()));
			return fun;
		}
		WriteFieldFunction GetWriteFieldFunction_ForInvoke()
		{
			static WriteFieldFunction fun = reinterpret_cast<WriteFieldFunction>(reinterpret_cast<uintptr_t>(NomJIT::Instance().lookup("RT_NOM_WRITEFIELD_FORINVOKABLE")->getValue()));
			return fun;
		}
		ReadTypeArgFunction GetReadTypeArgFunction()
		{
			static ReadTypeArgFunction fun = reinterpret_cast<ReadTypeArgFunction>(reinterpret_cast<uintptr_t>(NomJIT::Instance().lookup("RT_NOM_READTYPEARG")->getValue()));
			return fun;
		}
		WriteTypeArgFunction GetWriteTypeArgFunction()
		{
			static WriteTypeArgFunction fun = reinterpret_cast<WriteTypeArgFunction>(reinterpret_cast<uintptr_t>(NomJIT::Instance().lookup("RT_NOM_WRITETYPEARG")->getValue()));
			return fun;
		}
		WriteVTableFunction GetWriteVTableFunction()
		{
			static WriteVTableFunction fun = reinterpret_cast<WriteVTableFunction>(reinterpret_cast<uintptr_t>(NomJIT::Instance().lookup("RT_NOM_WRITEVTABLE")->getValue()));
			return fun;
		}
		void* GetGeneralLLVMFunction(llvm::StringRef name)
		{
			return reinterpret_cast<void*>(*(reinterpret_cast<uintptr_t*>(NomJIT::Instance().lookup(name)->getValue())));
		}
		void* GetBooleanTrue()
		{
			static void* bval = reinterpret_cast<void*>(*(reinterpret_cast<uintptr_t*>(NomJIT::Instance().lookup("RT_NOM_TRUE")->getValue())));
			return bval;
		}
		void* GetBooleanFalse()
		{
			static void* bval = reinterpret_cast<void*>(*(reinterpret_cast<uintptr_t*>(NomJIT::Instance().lookup("RT_NOM_FALSE")->getValue())));
			return bval;
		}
		void* GetVoidObj()
		{
			static void* voidobj = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(Nom::Runtime::NomJIT::Instance().getSymbolAddress("RT_NOM_VOIDOBJ")));
			return voidobj;
		}


		llvm::Constant* EnsureIntegerSize(llvm::Constant* cnst, int bitwidth)
		{
			int size = static_cast<int>(cnst->getType()->getPrimitiveSizeInBits());
			if (size < bitwidth)
			{
				cnst = ConstantExpr::getZExt(cnst, inttype(static_cast<unsigned int>(bitwidth)));
			}
			else if (size > bitwidth)
			{
				cnst = ConstantExpr::getTrunc(cnst, inttype(static_cast<unsigned int>(bitwidth)));
			}
			return cnst;
		}
		llvm::Value* EnsureIntegerSize(NomBuilder& builder, llvm::Value* val, int bitwidth)
		{
			int size = static_cast<int>(val->getType()->getPrimitiveSizeInBits());
			if (size < bitwidth)
			{
				val = builder->CreateZExt(val, inttype(static_cast<unsigned int>(bitwidth)));
			}
			else if (size > bitwidth)
			{
				val = builder->CreateTrunc(val, inttype(static_cast<unsigned int>(bitwidth)));
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
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, static_cast<unsigned int>(size)), val, false);
		}
		llvm::ConstantInt* MakeSInt(size_t size, int64_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, static_cast<unsigned int>(size)), static_cast<uint64_t>(val), true);
		}
		llvm::ConstantInt* MakeInt(size_t size, int64_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, static_cast<unsigned int>(size)), static_cast<uint64_t>(val), true);
		}
		PWCInt32 MakeInt32(int32_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, 32), bit_cast<uint32_t, int32_t>(val), true);
		}
		PWCInt32 MakeInt32(uint32_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, 32), val, true);
		}
		PWCInt32 MakeInt32(uint64_t val)
		{
			return MakeInt32(static_cast<uint32_t>(val));
		}
		PWCInt32 MakeInt32(int64_t val)
		{
			return MakeInt32(static_cast<uint32_t>(val));
		}
		llvm::ConstantInt* MakeInt(size_t size, uint64_t val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, static_cast<unsigned int>(size)), val, false);
		}
		llvm::ConstantInt* MakeIntLike(llvm::Value* value, uint64_t val)
		{
			if (!value->getType()->isIntegerTy())
			{
				throw new std::exception();//TODO: Exception
			}
			return ConstantInt::get(static_cast<llvm::IntegerType*>(value->getType()), val);
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
			auto ret = ConstantExpr::getXor(llvm::ConstantInt::getAllOnesValue(IntegerType::get(LLVMCONTEXT, static_cast<unsigned int>(length - leadingZeroes))), ConstantExpr::getZExt(llvm::ConstantInt::getAllOnesValue(IntegerType::get(LLVMCONTEXT, static_cast<unsigned int>(trailingZeroes))), IntegerType::get(LLVMCONTEXT, static_cast<unsigned int>(length - leadingZeroes))));
			if (leadingZeroes > 0)
			{
				ret = ConstantExpr::getZExt(ret, IntegerType::get(LLVMCONTEXT, static_cast<unsigned int>(length)));
			}
			return ret;
		}
		llvm::Value* CreatePointerEq(NomBuilder& builder, llvm::Value* left, llvm::Value* right, const llvm::Twine& name)
		{
			return builder->CreateICmpEQ(builder->CreatePtrToInt(left, numtype(intptr_t)), builder->CreatePtrToInt(right, numtype(intptr_t)), name);
		}
		void CreateExpect(NomBuilder& builder, llvm::Value* value, llvm::Value* expected)
		{
			builder->CreateIntrinsic(Intrinsic::expect, { value->getType() }, { value,expected });
		}
	}
}
