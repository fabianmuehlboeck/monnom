#include "IEnumeratorInterface.h"
#include "NomTypeParameter.h"
#include "NomType.h"
#include "NomClassType.h"
#include "BoolClass.h"

namespace Nom
{
	namespace Runtime
	{
		IEnumeratorInterface::IEnumeratorInterface() : NomInterface("IEnumerator_1"), NomInterfaceInternal(new NomString("IEnumerator_1"))
		{
			NomTypeParameterRef* ntparr = (NomTypeParameterRef*)nmalloc(sizeof(NomTypeParameterRef));
			ntparr[0] = new NomTypeParameterInternal(this, 0, NomType::AnythingRef, NomType::NothingRef);
			SetDirectTypeParameters(llvm::ArrayRef<NomTypeParameterRef>(ntparr, 1));
			SetSuperInterfaces();
		}

		IEnumeratorInterface::~IEnumeratorInterface()
		{
		}

		IEnumeratorInterface* IEnumeratorInterface::GetInstance()
		{
			static IEnumeratorInterface iei;
			static bool once = true;
			if (once)
			{
				once = false;
				NomMethodDeclInternal* meth = new NomMethodDeclInternal(&iei, "MoveNext", "IEnumerator$$MoveNext");
				meth->SetDirectTypeParameters();
				meth->SetReturnType(NomBoolClass::GetInstance()->GetType());

				//NomTypeRef* argarr = (NomTypeRef*)nmalloc(sizeof(NomTypeRef));
				//argarr[0] = iei.GetTypeParameter(0)->GetVariable();

				meth->SetArgumentTypes();

				iei.AddMethod(meth);


				meth = new NomMethodDeclInternal(&iei, "Current", "IEnumerator$$Current");
				meth->SetDirectTypeParameters();
				meth->SetArgumentTypes();
				
				auto tvar = new NomTypeVar(iei.GetDirectTypeParameters()[0]);

				meth->SetReturnType(tvar);

				iei.AddMethod(meth);

				//iei.PreprocessInheritance();
			}
			return &iei;
		}
	}
}
