#include "RTGeneralInterface.h"
#include "RTInterface.h"
#include "RTStructInterface.h"
#include "CompileHelpers.h"
#include "RTStructInterface.h"
#include "RTInterface.h"
#include "RTInterfaceTableEntry.h"
#include "NomInterface.h"
#include "Defs.h"
#include "IMT.h"
#include "RTCompileConfig.h"
#include "RTVTable.h"
#include "RTSignature.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTGeneralInterface::GetLLVMType()
		{
			static auto git = StructType::create(LLVMCONTEXT, "RT_NOM_GeneralInterface");
			static bool once = true;
			if (once)
			{
				once = false;
				git->setBody(RTInterface::GetLLVMType(),		//regular interface stuff
					RTStructInterface::GetLLVMPointerType(),	//pointer to template
					numtype(size_t),							//template size
					numtype(size_t),							//template offset
					RTStructInterface::GetLLVMPointerType()		//pointer to optimized interface (for instances which store type variables themselves)
					);
			}
			return git;
		}

		llvm::Value* RTGeneralInterface::GenerateReadTemplate(NomBuilder& builder, llvm::Value* descPtr)
		{
			return MakeLoad(builder, descPtr, GetLLVMPointerType(), MakeInt32(RTGeneralInterfaceFields::Template), "template");
		}
		llvm::Value* RTGeneralInterface::GenerateReadOptimizedVTable(NomBuilder& builder, llvm::Value* descPtr)
		{
			return MakeLoad(builder, descPtr, GetLLVMPointerType(), MakeInt32(RTGeneralInterfaceFields::OptimizedInterface), "optimizedVtable");
		}
		llvm::Value* RTGeneralInterface::GenerateReadTemplateSize(NomBuilder& builder, llvm::Value* descPtr)
		{
			return MakeLoad(builder, descPtr, GetLLVMPointerType(), MakeInt32(RTGeneralInterfaceFields::TemplateSize), "templateSize");
		}
		llvm::Value* RTGeneralInterface::GenerateReadTemplateOffset(NomBuilder& builder, llvm::Value* descPtr)
		{
			return MakeLoad(builder, descPtr, GetLLVMPointerType(), MakeInt32(RTGeneralInterfaceFields::TemplateOffset), "templateOffset");
		}
		llvm::Constant* RTGeneralInterface::CreateGlobalConstant(llvm::Module& mod, GlobalValue::LinkageTypes linkage, const Twine& name, const NomInterface* irptr, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* instantiationDictionary)
		{
			llvm::StructType* gvartype = StructType::get(LLVMCONTEXT, { GetLLVMType() }, true);
			GlobalVariable* gvar = new GlobalVariable(mod, gvartype, true, linkage, nullptr, name);

			llvm::Constant* checkReturnValueFun = irptr->GetCheckReturnTypeFunction(mod, linkage);

			llvm::Constant* templateConstant = RTStructInterface::CreateGlobalConstant(mod, linkage, "NOM_RT_SI_" + name, irptr, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), instantiationDictionary, ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), false);
			llvm::Constant* optimizedConstant = RTStructInterface::CreateGlobalConstant(mod, linkage, "NOM_RT_SIOPT_" + name, irptr, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), instantiationDictionary, ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), true);

			llvm::Constant* templateOffset = MakeInt<size_t>(0); // ConstantExpr::getAdd(llvmsizeof(irptr->GetInterfaceTableType(false)), ConstantExpr::getMul(llvmsizeof(POINTERTYPE), MakeInt<size_t>(irptr->MethodTable.size())));

			llvm::Constant** imtNullPtrs = makealloca(llvm::Constant*, IMTsize);
			for (decltype(IMTsize) i = 0; i < IMTsize; i++)
			{
				imtNullPtrs[i] = ConstantPointerNull::get(GetIMTFunctionType()->getPointerTo());
			}

			gvar->setInitializer(llvm::ConstantStruct::get(gvartype, llvm::ConstantStruct::get(GetLLVMType(), RTInterface::CreateConstant(irptr, RTInterfaceFlags::IsInterface, typeArgCount, ConstantExpr::getSub(superTypesCount, MakeIntLike(superTypesCount,1)), ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(1) })), ConstantArray::get(arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize), ArrayRef<Constant*>(imtNullPtrs, IMTsize)), checkReturnValueFun, ConstantPointerNull::get(GetMethodEnsureFunctionType()->getPointerTo()), instantiationDictionary, ConstantPointerNull::get(RTSignature::GetLLVMType()->getPointerTo())), templateConstant, ConstantExpr::getAdd(templateOffset, llvmsizeof(RTStructInterface::GetLLVMType())), templateOffset, optimizedConstant)));
			return ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(0) }));
		}
		llvm::Constant* RTGeneralInterface::FindConstant(llvm::Module& mod, const StringRef name)
		{
			auto cnst = mod.getGlobalVariable(name);
			if (cnst == nullptr)
			{
				return nullptr;
			}
			return ConstantExpr::getGetElementPtr(cnst->getValueType(), cnst, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(0) }));
		}
	}
}