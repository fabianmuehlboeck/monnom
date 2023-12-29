#pragma once
#include "NomClass.h"
#include "RTClass.h"
#include "XRTClassType.h"
#include "NomConstants.h"
#include "RTTypeHead.h"
#include "AvailableExternally.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constant.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		class ObjectHeader;
		class NomType;
	}
}


namespace Nom
{
	namespace Runtime
	{
		class NomNullClass : public NomClassInternal
		{
		private:
			NomNullClass();
		public:
			static void * NullObject();
			static NomNullClass *GetInstance();
			virtual ~NomNullClass() override
			{

			}

		};
		class NomNullObject : public AvailableExternally < llvm::Constant >
		{
		private:
			NomNullObject();
		public:
			static NomNullObject *GetInstance() { [[clang::no_destroy]] static NomNullObject obj; return &obj; }
			~NomNullObject() override {}

			virtual llvm::Constant * createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant * findLLVMElement(llvm::Module & mod) const override;
		};
	}
}
