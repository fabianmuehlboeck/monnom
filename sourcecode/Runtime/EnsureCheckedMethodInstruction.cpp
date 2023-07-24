#include "EnsureCheckedMethodInstruction.h"
#include <stdio.h>
#include "NomConstants.h"
#include "RefValueHeader.h"
#include "CompileEnv.h"
#include "NomClassType.h"
#include "RTOutput.h"
#include "RTRecord.h"
#include "NomNameRepository.h"
#include "CompileHelpers.h"
#include "CallingConvConf.h"
#include "RTVTable.h"
#include "RTCompileConfig.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		EnsureCheckedMethodInstruction::EnsureCheckedMethodInstruction(ConstantID methodNameID, RegIndex receiver) : NomInstruction(OpCode::EnsureCheckedMethod), MethodName(methodNameID), Receiver(receiver)
		{
		}
		EnsureCheckedMethodInstruction::~EnsureCheckedMethodInstruction()
		{
		}
		void EnsureCheckedMethodInstruction::Compile([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
		}
		void EnsureCheckedMethodInstruction::Print(bool resolve)
		{
			cout << "EnsureCheckedMethod ";
			NomConstants::PrintConstant(MethodName, resolve);
			cout << "@" << Receiver;
			cout << "\n";
		}
		void EnsureCheckedMethodInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(MethodName);
		}
	}
}
