#include "BoolClass.h"
#include <iostream>
PUSHDIAGSUPPRESSION
#include "llvm/Support/raw_os_ostream.h"
POPDIAGSUPPRESSION
#include "CompileHelpers.h"
#include "ObjectClass.h"
#include "RefValueHeader.h"
#include "RTTypeHead.h"
#include "PWObject.h"

namespace Nom
{
	namespace Runtime
	{
		NomBoolClass::NomBoolClass() : NomInterface(), NomClassInternal(new NomString("Bool_0"))
		{
			SetDirectTypeParameters();
			SetSuperClass(NomInstantiationRef<NomClass>(NomObjectClass::GetInstance(), TypeList()));
			SetSuperInterfaces();
		}


		NomBoolClass* NomBoolClass::GetInstance() {
			[[clang::no_destroy]] static NomBoolClass nbc;
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
			return llvm::ConstantExpr::getPointerCast(llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), elem, llvm::ArrayRef<llvm::Constant*>({ {MakeInt(0), MakeInt(1), MakeInt(1)} })), REFTYPE);
		}

		llvm::Constant* NomBoolObjects::GetFalse(llvm::Module& mod)
		{
			auto elem = GetInstance()->GetLLVMElement(mod);
			return llvm::ConstantExpr::getPointerCast(llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), elem, llvm::ArrayRef<llvm::Constant*>({ {MakeInt(0), MakeInt(0), MakeInt(1)} })), REFTYPE);
		}

		llvm::Value* NomBoolObjects::PackBool(NomBuilder& builder, llvm::Value* b)
		{
			if (b->getType() != BOOLTYPE)
			{
				throw new std::exception();
			}
			auto index = builder->CreateZExt(b, numtype(int));
			auto mod = builder->GetInsertBlock()->getParent()->getParent();
			auto elem = GetInstance()->GetLLVMElement(*mod);
			return builder->CreatePointerCast(builder->CreateGEP(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), elem, llvm::ArrayRef<llvm::Value*>({{MakeInt(0), index,MakeInt(1)}})), REFTYPE);
		}

		llvm::Value* NomBoolObjects::UnpackBool(NomBuilder& builder, llvm::Value* b)
		{
			return builder->CreatePtrToInt(PWObject(b).ReadField(builder, PWCInt32(0,false), false), llvm::IntegerType::get(LLVMCONTEXT, 1));
		}

		PWBool NomBoolObjects::BoolObjToRawBool(NomBuilder& builder, llvm::Value* b)
		{
			llvm::Value* baddr = builder->CreatePtrToInt(b, numtype(intptr_t));
			llvm::Constant* basePtr = GetInstance()->findLLVMElement(*builder->GetInsertBlock()->getParent()->getParent());
			llvm::Constant* basePtrNext = ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), ConstantPointerNull::get(POINTERTYPE), llvm::ArrayRef<llvm::Constant*>({MakeInt32(0), MakeInt32(1),MakeInt32(ObjectHeaderFields::RefValueHeader)}));
			llvm::Constant* base = ConstantExpr::getPtrToInt(basePtr, numtype(intptr_t));
			llvm::Value* relToBase = builder->CreateSub(baddr, base);
			llvm::Value* divBySize = builder->CreateUDiv(relToBase, ConstantExpr::getPtrToInt(basePtrNext, numtype(intptr_t)));
			return builder->CreateTrunc(divBySize, inttype(1), "addrToBool");
		}


		llvm::Constant* NomBoolObjects::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto var = new llvm::GlobalVariable(mod, arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), true, linkage, nullptr, "RT_NOM_BOOLEANS");
			auto clsref = NomBoolClass::GetInstance()->GetLLVMElement(mod);
			auto fieldstype = arrtype(REFTYPE, 1);
			llvm::Constant* falseConst = ObjectHeader::GetConstant(clsref, llvm::ConstantArray::get(fieldstype, { llvm::ConstantExpr::getIntToPtr(MakeInt(1, static_cast<uint64_t>(0)), REFTYPE) }), llvm::ConstantArray::get(NLLVMPointerArr(RTTypeHead::GetLLVMType(), 0), {}));
			llvm::Constant* trueConst = ObjectHeader::GetConstant(clsref, llvm::ConstantArray::get(fieldstype, { llvm::ConstantExpr::getIntToPtr(MakeInt(1, static_cast<uint64_t>(1)), REFTYPE) }), llvm::ConstantArray::get(NLLVMPointerArr(RTTypeHead::GetLLVMType(), 0), {}));
			llvm::Constant* arr = llvm::ConstantArray::get(arrtype(falseConst->getType(), 2), { {falseConst, trueConst} });
			var->setAlignment(llvm::MaybeAlign(8));
			var->setInitializer(arr);

			new llvm::GlobalVariable(mod, NLLVMPointer(RefValueHeader::GetLLVMType()), true, linkage, llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), var, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(1),MakeInt32(ObjectHeaderFields::RefValueHeader) })), "RT_NOM_TRUE");
			new llvm::GlobalVariable(mod, NLLVMPointer(RefValueHeader::GetLLVMType()), true, linkage, llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), var, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(0),MakeInt32(ObjectHeaderFields::RefValueHeader) })), "RT_NOM_FALSE");
			return var;
		}

		llvm::Constant* NomBoolObjects::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getGlobalVariable("RT_NOM_BOOLEANS");
		}
	}
}
