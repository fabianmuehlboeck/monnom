#include "RTCmdInstruction.h"
#include <iostream>
#include "../NomConstants.h"
#include "../Runtime.h"
#include "../CompileHelpers.h"
PUSHDIAGSUPPRESSION
#include "llvm/Support/DynamicLibrary.h"
POPDIAGSUPPRESSION
#include "../DLLExport.h"
#include "../RTConfig.h"

using namespace llvm;
using namespace std;

extern "C" DLLEXPORT void RT_NOM_SetDebugLevel(decltype(NomDebugPrintLevel) dbglvl)
{
	NomDebugPrintLevel = dbglvl;
}

namespace Nom
{
	namespace Runtime
	{
		RTCmdInstruction::RTCmdInstruction(const ConstantID cmd) : NomInstruction(OpCode::Debug), cmdConstant(cmd)
		{
		}


		RTCmdInstruction::~RTCmdInstruction()
		{
		}

		void RTCmdInstruction::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			static bool once = true;
			if (once)
			{
				once = false;
				llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_SetDebugLevel", reinterpret_cast<void*>(& RT_NOM_SetDebugLevel));
			}
			auto cmd = NomConstants::GetString(cmdConstant)->GetText()->ToStdString();
			if (cmd.substr(0,14)=="SetDebugLevel ")
			{
				char* end = nullptr;
				auto lvl = strtol(cmd.substr(14).c_str(), &end, 10);
				auto setDebugLevelFun = env->Module->getFunction("RT_NOM_SetDebugLevel");
				if (setDebugLevelFun == nullptr)
				{
					FunctionType* setDebugLevelFT = FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { numtype(decltype(NomDebugPrintLevel)) }, false);
					setDebugLevelFun = Function::Create(setDebugLevelFT, GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_SetDebugLevel", env->Module);
				}
				builder->CreateCall(setDebugLevelFun, { MakeInt<decltype(NomDebugPrintLevel)>(lvl) });
				return;
			}
			cout << "Unknown runtime command";
			throw new std::exception();
			/*env->basicBlockTerminated = false;
			std::string * textptr = new std::string();
			std::array<llvm::Value *, 1> args = { { llvm::ConstantInt::get(Type::getIntNTy(LLVMCONTEXT, bitsin(uint64_t)), reinterpret_cast<uint64_t>(textptr), false) } };
			llvm::Function * fun = GetPrint(env->Module);*/
		}
		void RTCmdInstruction::Print(bool resolve)
		{
			cout << "RUNTIMECMD ";
			NomConstants::PrintConstant(cmdConstant, resolve);
			cout << "\n";
		}
		void RTCmdInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(cmdConstant);
		}
	}
}
