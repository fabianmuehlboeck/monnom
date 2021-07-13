#include "FunInterface.h"
#include <unordered_map>

namespace Nom
{
	namespace Runtime
	{
		FunInterface::FunInterface(std::string &name) : NomInterface(name), NomInterfaceInternal(new NomString(name))
			//NomInterface(NomConstants::AddString(NomString("Fun")), typeParameters, 0, nullptr)
		{
			//llvm::ArrayRef<NomInstantiationRef<NomInterface>>()
			//NomMethod* invoke = new NomMethod(this, "Invoke", "Fun.Invoke", returnType, argTypes, 0, 0, false, true);
			//Methods.push_back(invoke);
			this->SetSuperInterfaces();
		}
		FunInterface::~FunInterface()
		{
		}
		FunInterface* FunInterface::GetFun(size_t argcount)
		{
			static std::unordered_map<int, FunInterface*> instances;
			auto instance = instances.find(argcount);
			if (instance != instances.end())
			{
				return (*instance).second;
			}
			std::string name = "Fun_" + std::to_string(argcount + 1);
			auto fun = new FunInterface(name);
			instances[argcount] = fun;
			NomTypeParameterRef* tpbuf = (NomTypeParameterRef*)(nmalloc(sizeof(NomTypeParameterRef) * (argcount + 1)));
			NomTypeRef* argtbuf = (NomTypeRef*)(nmalloc(sizeof(NomTypeRef) * (argcount )));

			for (decltype(argcount) i = 0; i < argcount; i++)
			{
				tpbuf[i] = new NomTypeParameterInternal(fun, i, NomType::AnythingRef, NomType::NothingRef);
				argtbuf[i] = tpbuf[i]->GetVariable();
			}
			tpbuf[argcount] = new NomTypeParameterInternal(fun, argcount, NomType::AnythingRef, NomType::NothingRef);

			fun->SetDirectTypeParameters(llvm::ArrayRef<NomTypeParameterRef>(tpbuf, argcount + 1));

			auto method = new NomMethodDeclInternal(fun, "", name +"$$");
			method->SetArgumentTypes(TypeList(argtbuf, argcount));
			method->SetReturnType(tpbuf[argcount]->GetVariable());
			fun->AddMethod(method);

			//fun->PreprocessInheritance();
			return fun;
		}
	}
}