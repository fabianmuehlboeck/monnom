#pragma once
#include <unordered_map>
#include <llvm/IR/Constant.h>
#include <llvm/ADT/SmallVector.h>
#include "RTTypeHead.h"
#include "NomClass.h"
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{

		class TypeRegistry : public AvailableExternally<llvm::Function>
		{
		private:
			TypeRegistry();
			//llvm::SmallVector<llvm::GlobalVariable *, 16> resolveBuffer;
			//std::unordered_map<size_t, RTTypeHead *> typeDict;
		public:
			static TypeRegistry& Instance();
			virtual ~TypeRegistry();

			static llvm::FunctionType* GetLLVMFunctionType();

			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
//
//extern "C" Nom::Runtime::RTTypeHead *NOM_RT_GetClassType(Nom::Runtime::NomClass * cls, Nom::Runtime::NomTypeRef *args);
//extern "C" Nom::Runtime::RTTypeHead *NOM_RT_GetInterfaceType(size_t count, Nom::Runtime::NomTypeRef *args);