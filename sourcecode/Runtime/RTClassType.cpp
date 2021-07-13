#include "RTClassType.h"
#include "RecursionBuffer.h"
#include "NomAlloc.h"
#include <tuple>
#include "Context.h"
#include "RTClass.h"
#include "NomJIT.h"
#include "NomClassType.h"
#include "CompileHelpers.h"
#include "RTInterface.h"
#include "RTTypeHead.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType * RTClassType::GetLLVMType()
		{
			static llvm::StructType *llvmtype = llvm::StructType::create(LLVMCONTEXT, "LLVMRTClassType");
			static bool dontrepeat = true;
			if (dontrepeat)
			{
				dontrepeat = false;
				llvmtype->setBody(GetLLVMType(0)->elements()
				);
			}
			return llvmtype;
		}
		llvm::StructType * RTClassType::GetLLVMType(size_t size)
		{
			return llvm::StructType::get(LLVMCONTEXT, {
				llvm::ArrayType::get(RTTypeHead::GetLLVMType()->getPointerTo(), size), //arguments
				RTTypeHead::GetLLVMType(), //header
				RTInterface::GetLLVMType()->getPointerTo(), //class type
				});
		}
		llvm::Constant * RTClassType::GetConstant(llvm::Module &mod, const NomClassType *cls)
		{
			//std::string globalName = "RT_CLASSTYPE_" + std::to_string(cls->GetHashCode());
			//auto globalVar = mod.getNamedGlobal(globalName);
			size_t argcount = cls->Arguments.size();
			//if (globalVar == nullptr)
			//{
				//globalVar = new GlobalVariable(mod, GetLLVMType(argcount), true, GlobalValue::LinkageTypes::ExternalWeakLinkage, nullptr);
				llvm::Constant** argtypes = nullptr;
				if (argcount > 0)
				{
					argtypes = makealloca(llvm::Constant*, argcount);
					for (size_t i = argcount; i > 0;)
					{
						i--;
						argtypes[i] = cls->Arguments[argcount-(i+1)]->GetLLVMElement(mod);
					}
				}
				//globalVar->setInitializer(llvm::ConstantStruct::get(GetLLVMType(argcount), llvm::ConstantArray::get(arrtype(TYPETYPE, argcount), llvm::ArrayRef<llvm::Constant*>(argtypes, argcount)), RTTypeHead::GetConstant(TypeKind::TKClass, MakeInt(cls->GetHashCode()), cls), llvm::ConstantExpr::getPointerCast(cls->Named->GetLLVMElement(mod), RTInterface::GetLLVMType()->getPointerTo())));
			//}
			//return ConstantExpr::getPointerCast(ConstantExpr::getGetElementPtr(GetLLVMType(argcount), globalVar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(1) })), GetLLVMType()->getPointerTo());
				return llvm::ConstantStruct::get(GetLLVMType(argcount), llvm::ConstantArray::get(arrtype(TYPETYPE, argcount), llvm::ArrayRef<llvm::Constant*>(argtypes, argcount)), RTTypeHead::GetConstant(TypeKind::TKClass, MakeInt(cls->GetHashCode()), cls), llvm::ConstantExpr::getPointerCast(cls->Named->GetLLVMElement(mod), RTInterface::GetLLVMType()->getPointerTo()));
		}


		uint64_t RTClassType::HeadOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassTypeFields::Head); return offset;
		}
		uint64_t RTClassType::ClassOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassTypeFields::Class); return offset;
		}


		uint64_t RTClassType::ArgumentsOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassTypeFields::TypeArgs); return offset;
		}
		llvm::Value * RTClassType::FromHead(NomBuilder& builder, llvm::Value * head)
		{
			if ((unsigned char)RTClassTypeFields::Head == 0 || HeadOffset() == 0)
			{
				return builder->CreatePointerCast(head, GetLLVMType()->getPointerTo());
			}
			return builder->CreateIntToPtr(
				builder->CreateSub(builder->CreatePtrToInt(head, numtype(intptr_t)), MakeInt<intptr_t>(HeadOffset())),
				GetLLVMType()->getPointerTo()
			);
		}
		llvm::Value* RTClassType::GenerateReadClassDescriptorLink(NomBuilder& builder, llvm::Value* type)
		{
			return MakeLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTClassTypeFields::Class), "class");
		}
		llvm::Value* RTClassType::GetTypeArgumentsPtr(NomBuilder& builder, llvm::Value* type)
		{
			return builder->CreateGEP(builder->CreatePointerCast(type, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTClassTypeFields::TypeArgs),MakeInt32(0) }, "typeArgs");
		}
		llvm::FunctionType* RTClassType::GetInstantiateClassTypeFunctionType()
		{
			static llvm::FunctionType* ft = FunctionType::get(TYPETYPE, { GetLLVMType()->getPointerTo(), RTInterface::GetLLVMType()->getPointerTo(), TYPETYPE->getPointerTo(), inttype(32), numtype(size_t), POINTERTYPE }, false);
			return ft;
		}
		llvm::Function* RTClassType::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = mod.getFunction("RT_NOM_InstantiateClassType");
			if (fun == nullptr)
			{
				fun = Function::Create(GetInstantiateClassTypeFunctionType(), linkage, "RT_NOM_InstantiateClassType", mod);
				NomBuilder builder;

				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);

				builder->SetInsertPoint(startBlock);
				auto args = fun->arg_begin();
				Value* ctype = args;
				args++;
				Value* vtableptr = args;
				args++;
				Value* typeargs = args;
				args++;
				Value* targcount = args;
				args++;
				Value* hash = args;
				args++;
				Value* nomtype = args;

				RTTypeHead::CreateInitialization(builder, mod, ctype, TypeKind::TKClass, hash, nomtype);
				MakeStore(builder, vtableptr, ctype, MakeInt32(RTClassTypeFields::Class));

				auto targetTypeArgStart = builder->CreateGEP(ctype, { MakeInt32(0), MakeInt32(RTClassTypeFields::TypeArgs), builder->CreateNeg(targcount) });
				auto sourceTypeArgStart = builder->CreateGEP(typeargs, builder->CreateNeg(targcount));
				builder->CreateMemCpy(targetTypeArgStart, MaybeAlign(8), sourceTypeArgStart, MaybeAlign(8), builder->CreateMul(targcount, MakeInt32(sizeof(intptr_t))));
				builder->CreateRet(builder->CreateGEP(ctype, { MakeInt32(0), MakeInt32(RTClassTypeFields::Head) }));
			}
			return fun;
		}
		llvm::Function* RTClassType::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_InstantiateClassType");
		}
	}
}
