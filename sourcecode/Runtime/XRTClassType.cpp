#include "XRTClassType.h"
#include "NomAlloc.h"
#include <tuple>
#include "Context.h"
#include "RTClass.h"
#include "NomJIT.h"
#include "NomClassType.h"
#include "CompileHelpers.h"
#include "RTInterface.h"
#include "RTTypeHead.h"
#include "PWClassType.h"
#include "PWInterface.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* XRTClassType::GetLLVMType()
		{
			static llvm::StructType* llvmtype = llvm::StructType::create(LLVMCONTEXT, "LLVMRTClassType");
			static bool dontrepeat = true;
			if (dontrepeat)
			{
				dontrepeat = false;
				llvmtype->setBody(GetLLVMType(0)->elements()
				);
			}
			return llvmtype;
		}
		llvm::StructType* XRTClassType::GetLLVMType(size_t size)
		{
			return llvm::StructType::get(LLVMCONTEXT, {
				llvm::ArrayType::get(RTTypeHead::GetLLVMType()->getPointerTo(), size), //arguments
				RTTypeHead::GetLLVMType(), //header
				RTInterface::GetLLVMType()->getPointerTo(), //class type
				});
		}
		llvm::Constant* XRTClassType::GetConstant(llvm::Module& mod, const NomClassType* cls, llvm::Constant* castFun)
		{
			size_t argcount = cls->Arguments.size();
			llvm::Constant** argtypes = nullptr;
			if (argcount > 0)
			{
				argtypes = makealloca(llvm::Constant*, argcount);
				for (size_t i = argcount; i > 0;)
				{
					i--;
					argtypes[i] = cls->Arguments[argcount - (i + 1)]->GetLLVMElement(mod);
				}
			}
			return llvm::ConstantStruct::get(GetLLVMType(argcount), llvm::ConstantArray::get(arrtype(TYPETYPE, argcount), llvm::ArrayRef<llvm::Constant*>(argtypes, argcount)), RTTypeHead::GetConstant(TypeKind::TKClass, MakeInt(cls->GetHashCode()), cls, castFun), cls->Named->GetInterfaceDescriptor(mod));
		}

		llvm::Value* XRTClassType::GenerateReadClassDescriptorLink(NomBuilder& builder, llvm::Value* type)
		{
			return PWClassType(type).ReadClassDescriptorLink(builder);
		}
		llvm::Value* XRTClassType::GetTypeArgumentsPtr(NomBuilder& builder, llvm::Value* type)
		{
			return PWClassType(type).TypeArgumentsPointer(builder);
		}
		llvm::FunctionType* XRTClassType::GetInstantiateClassTypeFunctionType()
		{
			static llvm::FunctionType* ft = FunctionType::get(TYPETYPE, { GetLLVMType()->getPointerTo(), RTInterface::GetLLVMType()->getPointerTo(), TYPETYPE->getPointerTo(), inttype(32), numtype(size_t), POINTERTYPE }, false);
			return ft;
		}
		llvm::Function* XRTClassType::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
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

				RTTypeHead::CreateInitialization(builder, mod, ctype, TypeKind::TKClass, hash, nomtype, RTInterface::GenerateReadCastFunction(builder, vtableptr));
				MakeStore(builder, vtableptr, GetLLVMType(), ctype, MakeInt32(RTClassTypeFields::Class));

				auto targetTypeArgStart = builder->CreateGEP(GetLLVMType(), ctype, { MakeInt32(0), MakeInt32(RTClassTypeFields::TypeArgs), builder->CreateNeg(targcount) });
				auto sourceTypeArgStart = builder->CreateGEP(TYPETYPE, typeargs, builder->CreateNeg(targcount));
				builder->CreateMemCpy(targetTypeArgStart, MaybeAlign(8), sourceTypeArgStart, MaybeAlign(8), builder->CreateMul(targcount, MakeInt32(sizeof(intptr_t))));
				builder->CreateRet(builder->CreateGEP(GetLLVMType(), ctype, { MakeInt32(0), MakeInt32(RTClassTypeFields::Head) }));
			}
			return fun;
		}
		llvm::Function* XRTClassType::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_InstantiateClassType");
		}
	}
}
