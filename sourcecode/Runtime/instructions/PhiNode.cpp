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

		//void PhiNode::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		//{
		//	if (!env->basicBlockTerminated)
		//	{
		//		builder->CreateBr(block);
		//		AddJumpSource(builder->GetInsertBlock(), env);
		//	}
		//	builder->SetInsertPoint(block);
		//	RegIndex i;
		//	for (i = this->intSize - 1; i >= 0; i--)
		//	{
		//		phiNodes[i] = builder->CreatePHI(INTTYPE, 2, env->contextName + Twine("/") + Twine(lineno) + Twine("/i") + Twine(i));
		//		(*env)[std::get<0>(this->intRegs[i])] = NomValue(phiNodes[i], NomIntClass::GetInstance()->GetType());
		//	}
		//	for (i = this->floatSize - 1; i >= 0; i--)
		//	{
		//		phiNodes[i + intSize] = builder->CreatePHI(FLOATTYPE, 2, env->contextName + Twine("/") + Twine(lineno) + Twine("/f") + Twine(i));
		//		(*env)[std::get<0>(this->floatRegs[i])] = NomValue(phiNodes[i + intSize], NomFloatClass::GetInstance()->GetType());
		//	}
		//	for (i = this->refSize - 1; i >= 0; i--)
		//	{
		//		phiNodes[i + intSize + floatSize] = builder->CreatePHI(REFTYPE, 2, env->contextName + Twine("/") + Twine(lineno) + Twine("/r") + Twine(i));
		//		(*env)[std::get<0>(this->refRegs[i])] = NomValue(phiNodes[i + intSize + floatSize], (*env)[std::get<1>(this->refRegs[i])].GetNomType()->Join((*env)[std::get<2>(this->refRegs[i])].GetNomType()));
		//	}
		//	wasCompiled = true;
		//	if (sourcesPresent == 2)
		//	{
		//		processSources(env);
		//	}
		//}

		void PhiNode::Initialize(NomBuilder &builder, llvm::Function * fun, CompileEnv* env)
		{
			myBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			builder->SetInsertPoint(myBlock);

			NomSubstitutionContextMemberContext nscmc(env->Context);
			for (auto &reg : mergeRegisters)
			{
				NomTypeRef type = NomConstants::GetType(&nscmc, std::get<1>(reg));
				(*env)[std::get<0>(reg)] = NomValue(builder->CreatePHI(type->GetLLVMType(), IncomingCount), type);
			}
		}

		//void PhiNode::processSources(CompileEnv* env)
		//{
		//	RegIndex i;
		//	RegIndex j;
		//	for (i = this->intSize - 1; i >= 0; i--)
		//	{
		//		phiNodes[i]->addIncoming((*env)[std::get<1>(this->intRegs[i])], sources[0]);
		//		phiNodes[i]->addIncoming((*env)[std::get<2>(this->intRegs[i])], sources[1]);
		//	}
		//	for (i = this->floatSize - 1; i >= 0; i--)
		//	{
		//		j = i + intSize;
		//		phiNodes[j]->addIncoming((*env)[std::get<1>(this->floatRegs[i])], sources[0]);
		//		phiNodes[j]->addIncoming((*env)[std::get<2>(this->floatRegs[i])], sources[1]);
		//	}
		//	for (i = this->refSize - 1; i >= 0; i--)
		//	{
		//		j = i + intSize + floatSize;
		//		phiNodes[j]->addIncoming((*env)[std::get<1>(this->refRegs[i])], sources[0]);
		//		phiNodes[j]->addIncoming((*env)[std::get<2>(this->refRegs[i])], sources[1]);
		//	}
		//}
		void PhiNode::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
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