#include "PhiNode.h"
#include "../Defs.h"
#include "../NomString.h"
#include "../IntClass.h"
#include "../FloatClass.h"
#include "../NomClassType.h"
#include <iostream>

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		void PhiNode::Initialize(NomBuilder &builder, llvm::Function * fun, CompileEnv* env)
		{
			myBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			builder->SetInsertPoint(myBlock);

			NomSubstitutionContextMemberContext nscmc(env->Context);
			for (auto &reg : mergeRegisters)
			{
				NomTypeRef type = NomConstants::GetType(&nscmc, std::get<1>(reg));
				(*env)[std::get<0>(reg)] = RTValue::GetValue(builder, builder->CreatePHI(type->GetLLVMType(), IncomingCount), type);
			}
		}

		void PhiNode::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			builder->SetInsertPoint(myBlock);
			env->basicBlockTerminated = false;
		}
		void PhiNode::Print(bool resolve)
		{
			cout << "PHI(" << std::dec << IncomingCount << " {";
			bool first = true;
			for (auto &mr : mergeRegisters)
			{
				if (!first)
				{
					cout << ", ";
				}
				first = false;
				cout << "[" << std::dec << mr.first << "; ";
				NomConstants::PrintConstant(mr.second, resolve);
				cout << "]";
			}
			cout << "})\n";
		}
		void PhiNode::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			for (auto &pair : mergeRegisters)
			{
				result.push_back(pair.second);
			}
		}
	}
}
