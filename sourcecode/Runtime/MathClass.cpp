#include "MathClass.h"
PUSHDIAGSUPPRESSION
#include "llvm/Support/DynamicLibrary.h"
POPDIAGSUPPRESSION
#include "DLLExport.h"
#include "FloatClass.h"


extern "C" DLLEXPORT double LIB_NOM_Math_Sin_1(double number)
{
	return sin(number);
}
extern "C" DLLEXPORT double LIB_NOM_Math_Cos_1(double number)
{
	return cos(number);
}
extern "C" DLLEXPORT double LIB_NOM_Math_Tan_1(double number)
{
	return tan(number);
}
extern "C" DLLEXPORT double LIB_NOM_Math_Sqrt_1(double number)
{
	return sqrt(number);
}
extern "C" DLLEXPORT double LIB_NOM_Math_FMax_2(double left, double right)
{
	if (left > right)
	{
		return left;
	}
	return right;
}
extern "C" DLLEXPORT double LIB_NOM_Math_FMin_2(double left, double right)
{
	if (left < right)
	{
		return left;
	}
	return right;
}

namespace Nom
{
	namespace Runtime
	{
		NomMathClass::NomMathClass() : NomInterface(), NomClassInternal(new NomString("Math_0"))
		{
			this->SetDirectTypeParameters();
			this->SetSuperInterfaces();
			this->SetSuperClass();

			NomTypeRef* floatArr = new NomTypeRef[2]{ GetFloatClassType(),GetFloatClassType() };

			NomStaticMethodInternal* sin = new NomStaticMethodInternal("Sin", "LIB_NOM_Math_Sin_1", this);
			sin->SetDirectTypeParameters();
			sin->SetArgumentTypes(TypeList(floatArr,1));
			sin->SetReturnType(GetFloatClassType());
			this->AddStaticMethod(sin);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Math_Sin_1", reinterpret_cast<void*>(&LIB_NOM_Math_Sin_1));

			NomStaticMethodInternal* cos = new NomStaticMethodInternal("Cos", "LIB_NOM_Math_Cos_1", this);
			cos->SetDirectTypeParameters();
			cos->SetArgumentTypes(TypeList(floatArr, 1));
			cos->SetReturnType(GetFloatClassType());
			this->AddStaticMethod(cos);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Math_Cos_1", reinterpret_cast<void*>(&LIB_NOM_Math_Cos_1));

			NomStaticMethodInternal* tan = new NomStaticMethodInternal("Tan", "LIB_NOM_Math_Tan_1", this);
			tan->SetDirectTypeParameters();
			tan->SetArgumentTypes(TypeList(floatArr, 1));
			tan->SetReturnType(GetFloatClassType());
			this->AddStaticMethod(tan);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Math_Tan_1", reinterpret_cast<void*>(&LIB_NOM_Math_Tan_1));

			NomStaticMethodInternal* fmax = new NomStaticMethodInternal("FMax", "LIB_NOM_Math_FMax_2", this);
			fmax->SetDirectTypeParameters();
			fmax->SetArgumentTypes(TypeList(floatArr, 2));
			fmax->SetReturnType(GetFloatClassType());
			this->AddStaticMethod(fmax);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Math_FMax_2", reinterpret_cast<void*>(&LIB_NOM_Math_FMax_2));

			NomStaticMethodInternal* fmin = new NomStaticMethodInternal("FMin", "LIB_NOM_Math_FMin_2", this);
			fmin->SetDirectTypeParameters();
			fmin->SetArgumentTypes(TypeList(floatArr, 2));
			fmin->SetReturnType(GetFloatClassType());
			this->AddStaticMethod(fmin);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Math_FMin_2", reinterpret_cast<void*>(&LIB_NOM_Math_FMin_2));

			NomStaticMethodInternal* fsqrt = new NomStaticMethodInternal("Sqrt", "LIB_NOM_Math_Sqrt_1", this);
			fsqrt->SetDirectTypeParameters();
			fsqrt->SetArgumentTypes(TypeList(floatArr, 1));
			fsqrt->SetReturnType(GetFloatClassType());
			this->AddStaticMethod(fsqrt);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Math_Sqrt_1", reinterpret_cast<void*>(&LIB_NOM_Math_Sqrt_1));
		}

		NomMathClass* NomMathClass::GetInstance() { [[clang::no_destroy]] static NomMathClass noc; return &noc; }
	}
}
