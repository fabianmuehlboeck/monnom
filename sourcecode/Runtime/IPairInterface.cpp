#include "IPairInterface.h"
#include "NomTypeParameter.h"
#include "NomType.h"
#include "IntClass.h"
#include "NomClassType.h"

namespace Nom
{
	namespace Runtime
	{
		IPairInterface::IPairInterface() : NomInterface(), NomInterfaceInternal(new NomString("IPair_2"))
		{
			NomTypeParameterRef* ntparr = makenmalloc(NomTypeParameterRef, 2);
			ntparr[0] = new NomTypeParameterInternal(this, 0, NomType::AnythingRef, NomType::NothingRef);
			ntparr[1] = new NomTypeParameterInternal(this, 1, NomType::AnythingRef, NomType::NothingRef);
			SetDirectTypeParameters(llvm::ArrayRef<NomTypeParameterRef>(ntparr, 2));
			SetSuperInterfaces();
		}

		IPairInterface::~IPairInterface()
		{
		}

		IPairInterface* IPairInterface::GetInstance()
		{
			[[clang::no_destroy]] static IPairInterface ici;
			static bool once = true;
			if (once)
			{
				once = false;
				NomMethodDeclInternal* fst = new NomMethodDeclInternal(&ici, "Fst", "IPair$$Fst");
				fst->SetDirectTypeParameters();
				fst->SetReturnType(ici.GetTypeParameter(0)->GetVariable());

				NomTypeRef* argarr = static_cast<NomTypeRef*>(nullptr);

				fst->SetArgumentTypes(llvm::ArrayRef<NomTypeRef>(argarr, static_cast<size_t>(0)));

				ici.AddMethod(fst);


				NomMethodDeclInternal* snd = new NomMethodDeclInternal(&ici, "Snd", "IPair$$Snd");
				snd->SetDirectTypeParameters();
				snd->SetReturnType(ici.GetTypeParameter(1)->GetVariable());

				snd->SetArgumentTypes(llvm::ArrayRef<NomTypeRef>(argarr, static_cast<size_t>(0)));

				ici.AddMethod(snd);
			}
			return &ici;
		}
	}
}
