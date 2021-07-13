#include "IEnumerableInterface.h"
#include "NomTypeParameter.h"
#include "NomType.h"
#include "NomClassType.h"
#include "IEnumeratorInterface.h"

namespace Nom
{
	namespace Runtime
	{
		IEnumerableInterface::IEnumerableInterface() : NomInterface("IEnumerable_1"), NomInterfaceInternal(new NomString("IEnumerable_1"))
		{
			NomTypeParameterRef* ntparr = (NomTypeParameterRef*)nmalloc(sizeof(NomTypeParameterRef));
			ntparr[0] = new NomTypeParameterInternal(this, 0, NomType::AnythingRef, NomType::NothingRef);
			SetDirectTypeParameters(llvm::ArrayRef<NomTypeParameterRef>(ntparr, 1));
			SetSuperInterfaces();
		}

		IEnumerableInterface::~IEnumerableInterface()
		{
		}

		IEnumerableInterface* IEnumerableInterface::GetInstance()
		{
			static IEnumerableInterface iei;
			static bool once = true;
			if (once)
			{
				once = false;
				NomMethodDeclInternal* meth = new NomMethodDeclInternal(&iei, "GetEnumerator", "IEnumerable$$GetEnumerator");
				meth->SetDirectTypeParameters();

				auto tvar = new NomTypeVar(iei.GetDirectTypeParameters()[0]);
				NomTypeRef* tvararr = new NomTypeRef[1]{ tvar };

				auto enumeratorType = IEnumeratorInterface::GetInstance()->GetType(TypeList(tvararr,1));

				meth->SetReturnType(enumeratorType);
				meth->SetArgumentTypes();
				iei.AddMethod(meth);

				//iei.PreprocessInheritance();
			}
			return &iei;
		}
		void IEnumerableInterface::GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const
		{
			results.push_back(IEnumeratorInterface::GetInstance());
		}
	}
}
