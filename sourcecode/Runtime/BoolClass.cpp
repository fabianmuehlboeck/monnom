#include "BoolClass.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "CompileHelpers.h"
#include "ObjectClass.h"
#include "RefValueHeader.h"
#include "RTTypeHead.h"

namespace Nom
{
	namespace Runtime
	{
		NomBoolClass::NomBoolClass() : NomInterface("Bool_0"), NomClassInternal(new NomString("Bool_0"))
		{
			SetDirectTypeParameters();
			SetSuperClass(NomInstantiationRef<NomClass>(NomObjectClass::GetInstance(), TypeList()));
			SetSuperInterfaces();
		}


		NomBoolClass* NomBoolClass::GetInstance() {
			static NomBoolClass nbc;
			static bool once = true;
			if (once)
			{
				once = false;
				NomObjectClass::GetInstance();
			}
			return &nbc;
		}

		NomBoolClass::~NomBoolClass()
		{
		}

		NomBoolObjects::NomBoolObjects() :AvailableExternally < llvm::Constant >()
		{

		}


		llvm::Constant* NomBoolObjects::GetTrue(llvm::Module& mod)
		{
			auto elem = GetInstance()->GetLLVMElement(mod);
			return llvm::ConstantExpr::getPointerCast(llvm::ConstantExpr::getGetElementPtr(((PointerType*)elem->getType())->getElementType(), elem, llvm::ArrayRef<llvm::Constant*>({ {MakeInt(0), MakeInt(1), MakeInt(1)} })), REFTYPE);
		}

		llvm::Constant* NomBoolObjects::GetFalse(llvm::Module& mod)
		{
			auto elem = GetInstance()->GetLLVMElement(mod);
			return llvm::ConstantExpr::getPointerCast(llvm::ConstantExpr::getGetElementPtr(((PointerType*)elem->getType())->getElementType(), elem, llvm::ArrayRef<llvm::Constant*>({ {MakeInt(0), MakeInt(0), MakeInt(1)} })), REFTYPE);
		}

		llvm::Value* NomBoolObjects::PackBool(NomBuilder& builder, llvm::Value* b)
		{
			if (b->getType() != BOOLTYPE)
			{
				throw new std::exception();
			}
			auto index = builder->CreateZExt(b, numtype(int));
			auto elem = GetInstance()->GetLLVMElement(*builder->GetInsertBlock()->getParent()->getParent());
			return builder->CreatePointerCast(builder->CreateGEP(((PointerType*)elem->getType())->getElementType(), elem, llvm::ArrayRef<llvm::Value*>({ {MakeInt(0), index,MakeInt(1)} })), REFTYPE);
		}

		llvm::Value* NomBoolObjects::UnpackBool(NomBuilder& builder, llvm::Value* b)
		{
			return builder->CreatePtrToInt(ObjectHeader::ReadField(builder, b, 0, false), llvm::IntegerType::get(LLVMCONTEXT, 1));
		}


		llvm::Constant* NomBoolObjects::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto var = new llvm::GlobalVariable(mod, arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), true, linkage, nullptr, "RT_NOM_BOOLEANS");
			auto clsref = NomBoolClass::GetInstance()->GetLLVMElement(mod);
			auto fieldstype = arrtype(REFTYPE, 1);
			llvm::Constant* falseConst = ObjectHeader::GetConstant(clsref, llvm::ConstantArray::get(fieldstype, { llvm::ConstantExpr::getIntToPtr(MakeInt(1, (uint64_t)0), REFTYPE) }), llvm::ConstantArray::get(arrtype(RTTypeHead::GetLLVMType()->getPointerTo(), 0), {}));
			llvm::Constant* trueConst = ObjectHeader::GetConstant(clsref, llvm::ConstantArray::get(fieldstype, { llvm::ConstantExpr::getIntToPtr(MakeInt(1, (uint64_t)1), REFTYPE) }), llvm::ConstantArray::get(arrtype(RTTypeHead::GetLLVMType()->getPointerTo(), 0), {}));
			llvm::Constant* arr = llvm::ConstantArray::get(arrtype(falseConst->getType(), 2), { {falseConst, trueConst} });
			var->setAlignment(llvm::MaybeAlign(8));
			var->setInitializer(arr);

			auto vartrue = new llvm::GlobalVariable(mod, RefValueHeader::GetLLVMType()->getPointerTo(), true, linkage, llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), var, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(1),MakeInt32(ObjectHeaderFields::RefValueHeader) })), "RT_NOM_TRUE");
			auto varfalse = new llvm::GlobalVariable(mod, RefValueHeader::GetLLVMType()->getPointerTo(), true, linkage, llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), var, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(0),MakeInt32(ObjectHeaderFields::RefValueHeader) })), "RT_NOM_FALSE");
			return var;
		}

		llvm::Constant* NomBoolObjects::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getGlobalVariable("RT_NOM_BOOLEANS");
		}

		//}
	}
}