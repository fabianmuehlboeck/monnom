#pragma once
#include "AvailableExternally.h"
#include "llvm/IR/Function.h"
#include "NomClass.h" 
#include "NomValueInstruction.h"
#include "instructions/CallCheckedStaticMethod.h"
#include "VoidClass.h"
#include "BoolClass.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomClassType.h"
#include "ObjectClass.h"
#include "IComparableInterface.h"
#include "NomTypeParameter.h"
#include <algorithm>

namespace Nom {
	namespace Runtime {
		class LibraryTest : public AvailableExternally< llvm::Function >
		{ 
		public:
			static void print_test();
			void* foo;
			int getNumPointers(std::vector<NomTypeRef> types) {
				int i = 0;
				for (NomTypeRef ref : types) {
					if (ref->GetLLVMType() == REFTYPE) i++;
				}
				return i;
			}
			NomTypeRef GetReturnType(const NomSubstitutionContext* context) const;
			TypeList GetArgumentTypes(const NomSubstitutionContext* context) const;
			int GetArgumentCount() const;
			//llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const;
			mutable std::string symname = "";
			LibraryTest();
			~LibraryTest() {
			}

			NomString* methodName = new NomString("test3");
			NomString* className = new NomString("Main_0");
			NomValueInstruction* call_inst;
			
			NomTypeRef returnType = (NomType*)NomBoolClass::GetInstance()->GetType();
			std::vector<NomTypeRef> argTypesArray = {(NomType*)NomIntClass::GetInstance()->GetType(), (NomType*)NomIntClass::GetInstance()->GetType() };

		
			NomTypeParameterRef ref = new NomTypeParameterInternal(NULL, 0, NomType::AnythingRef, NomType::NothingRef);
			std::vector<NomTypeParameterRef> typeArgsArray = {ref};
			//std::vector<NomTypeRef> typeArgsArray = {};

			const llvm::ArrayRef<NomTypeRef> argTypes = llvm::ArrayRef<NomTypeRef>(argTypesArray);
			const llvm::ArrayRef<NomTypeParameterRef> typeArgs = llvm::ArrayRef<NomTypeParameterRef>(typeArgsArray);


			//const RegIndex regcount = std::max((int)(argTypesArray.size() + getNumPointers(argTypesArray) + typeArgsArray.size() + getNumPointers(typeArgsArray)), 1);
			const RegIndex regcount = std::max((int)(argTypesArray.size() + getNumPointers(argTypesArray)), 1);
			
			//std::vector<llvm::Type*> types = {INTTYPE, INTTYPE};

			//llvm::ArrayRef<llvm::Type*> LLVMargTypes = llvm::ArrayRef<llvm::Type*>(types);
			llvm::Function* findLLVMElement(llvm::Module& mod) const override;

			llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const;
			std::vector<PhiNode*> phiNodes;
			void InitializePhis(NomBuilder& builder, llvm::Function* fun, CompileEnv* env) const
			{
				for (auto& phi : phiNodes)
				{
					phi->Initialize(builder, fun, env);
				}
			}


			
			const std::string* GetSymbolName() const
			{
				return &symname;
			}

		};
	}
}

