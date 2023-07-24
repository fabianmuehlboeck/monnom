#include "CastInstruction.h"
#include "../NomType.h"
#include "../IntClass.h"
#include "../FloatClass.h"
#include "../BoolClass.h"
#include "../RTOutput.h"
#include "../CompileHelpers.h"
#include "../RTCast.h"
#include <iostream>
#include "../Metadata.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		CastInstruction::CastInstruction(const RegIndex _reg, const RegIndex _value, const ConstantID _type) :NomValueInstruction(_reg, OpCode::Cast), ValueRegister(_value), TypeConstant(_type)
		{

		}

		CastInstruction::~CastInstruction()
		{

		}

		NomValue CastInstruction::MakeCast(NomBuilder& builder, CompileEnv* env, NomValue val, NomTypeRef type)
		{
			auto valtype = val.GetNomType();
			if (valtype->IsSubtype(type))
			{
				return val;
			}
			Function* fun = builder->GetInsertBlock()->getParent();
			auto castResult = RTCast::GenerateCast(builder, env, val, type);
			return NomValue(castResult, type, true);
		}

		void CastInstruction::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			NomSubstitutionContextMemberContext nscmc(env->Context);
			RegisterValue(env, MakeCast(builder, env, (*env)[ValueRegister], NomConstants::GetType(&nscmc, TypeConstant)));
		}

		void CastInstruction::Print(bool resolve)
		{
			cout << "Cast #" << std::dec << ValueRegister;
			cout << " to ";
			NomConstants::PrintConstant(TypeConstant, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}

		void CastInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(TypeConstant);
		}

	}
}
