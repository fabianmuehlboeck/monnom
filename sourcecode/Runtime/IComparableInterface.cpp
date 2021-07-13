#include "IComparableInterface.h"
#include "NomTypeParameter.h"
#include "NomType.h"
#include "IntClass.h"
#include "NomClassType.h"

namespace Nom
{
	namespace Runtime
	{
		IComparableInterface::IComparableInterface() : NomInterface("IComparable_1"), NomInterfaceInternal(new NomString("IComparable_1"))
		{
			NomTypeParameterRef* ntparr = (NomTypeParameterRef*)nmalloc(sizeof(NomTypeParameterRef));
			ntparr[0] = new NomTypeParameterInternal(this, 0, NomType::AnythingRef, NomType::NothingRef);
			SetDirectTypeParameters(llvm::ArrayRef<NomTypeParameterRef>(ntparr, 1));
			SetSuperInterfaces();
			
			
		}

		IComparableInterface::~IComparableInterface()
		{
		}

		IComparableInterface* IComparableInterface::GetInstance()
		{
			static IComparableInterface ici;
			static bool once = true;
			if (once)
			{
				once = false;
				NomMethodDeclInternal* meth = new NomMethodDeclInternal(&ici, "Compare", "IComparable$$Compare");
				meth->SetDirectTypeParameters();
				meth->SetReturnType(NomIntClass::GetInstance()->GetType());

				NomTypeRef* argarr = (NomTypeRef*)nmalloc(sizeof(NomTypeRef));
				argarr[0] = ici.GetTypeParameter(0)->GetVariable();

				meth->SetArgumentTypes(llvm::ArrayRef<NomTypeRef>(argarr, 1));

				ici.AddMethod(meth);
				//ici.PreprocessInheritance();
			}
			return &ici;
		}
	}
}
