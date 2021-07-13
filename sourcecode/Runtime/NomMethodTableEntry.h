#pragma once
#include "AvailableExternally.h"
#include "Defs.h"

namespace Nom
{
	namespace Runtime
	{
		class NomMethod;
		class NomCallableVersion;
		class NomMethodTableEntry /*: public AvailableExternally<llvm::Function>*/
		{
		public:
			NomMethodTableEntry(const NomMethod* method, llvm::FunctionType* functionType, int offset);

			const NomMethod* const Method;
			const NomCallableVersion* const CallableVersion;

			const int Offset;
			bool PerfectOverrideMatch(NomMethodTableEntry* other, TypeList outerSubstitutions);
			//// Inherited via AvailableExternally
			//virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			//virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}