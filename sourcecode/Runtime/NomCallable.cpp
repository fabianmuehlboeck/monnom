#include "NomCallable.h"
#include "NomCallableVersion.h"
#include "NomTypeParameter.h"

namespace Nom
{
	namespace Runtime
	{
		using namespace llvm;

		NomCallableLoaded::NomCallableLoaded(const std::string &name, const NomMemberContext *parent, const std::string &qname, const RegIndex regcount, const ConstantID typeArgs, const ConstantID argTypes, bool declOnly, bool cppWrapper) : NomMemberContextLoaded(parent, typeArgs), declOnly(declOnly), cppWrapper(cppWrapper), name(name), qname(qname), regcount(regcount), argTypesID(argTypes)
		{
		}
		//NomCallableLoaded::NomCallableLoaded(const std::string& name, const NomMemberContext* parent, const std::string& qname, const RegIndex regcount, const llvm::ArrayRef<NomTypeVarRef> typeParameters, bool declOnly, bool cppWrapper) : NomMemberContextLoaded(parent, typeParameters), declOnly(declOnly), cppWrapper(cppWrapper), name(name), qname(qname), regcount(regcount)
		//{
		//}


		NomCallableLoaded::~NomCallableLoaded()
		{
			if (argTypes.data() != nullptr)
			{
				nmfree(const_cast<NomTypeRef*>(argTypes.data()));
			}
		}

		TypeList NomCallableLoaded::GetArgumentTypes(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() > 0)
			{
				return NomConstants::GetTypeList(argTypesID)->GetTypeList(context);
			}
			else
			{
				if (argTypes.data() == nullptr)
				{
					NomSubstitutionContextMemberContext nscmc = NomSubstitutionContextMemberContext(this);
					argTypes = NomConstants::GetTypeList(argTypesID)->GetTypeList(&nscmc);
				}
				return argTypes;
			}
		}

		int NomCallableLoaded::GetArgumentCount() const
		{
			if (argTypes.data() != nullptr)
			{
				return argTypes.size();
			}
			return NomConstants::GetTypeList(argTypesID)->GetSize();
		}

		bool NomCallable::Satisfies(const NomSubstitutionContext* context, const TypeList typeArgs, const TypeList argTypes) const
		{
			if (typeArgs.size() != this->GetArgumentTypeParameters().size())
			{
				return false;
			}
			auto nscl = NomSubstitutionContextList(typeArgs);
			auto substC = NomSubstitutionContextCombination(context, &nscl);
			for (size_t i = 0; i < typeArgs.size(); i++)
			{
				if (typeArgs[i] == this->GetTypeParameter(i)->GetVariable()) //shortcut
				{
					continue;
				}
				if (!typeArgs[i]->IsSubtype(this->GetArgumentTypeParameters()[i]->GetUpperBound()->SubstituteSubtyping(&substC)))
				{
					return false;
				}
				if (!typeArgs[i]->IsSupertype(this->GetArgumentTypeParameters()[i]->GetLowerBound()->SubstituteSubtyping(&substC)))
				{
					return false;
				}
			}
			return NomType::PointwiseSubtype(argTypes, GetArgumentTypes(&substC));
		}

		//llvm::Function * NomCallable::GetLLVMFunction(llvm::Module * mod) const
		//{
		//	Function *ret = mod->getFunction(GetSymbolName());
		//	if (ret == nullptr)
		//	{
		//		//std::cout << "\nCreating function Proto for static method " + name + "\n";
		//		ret = Function::Create(GetLLVMFunctionType(), Function::ExternalLinkage, GetSymbolName(), mod);;
		//	}
		//	return ret;
		//}

		const std::string *NomCallable::GetSymbolName() const
		{
			if (symname.empty())
			{
				if (GetName().size()>0 && GetName().at(0) == '$')
				{
					symname = GetName().substr(1);
				}
				else
				{
					symname = "";
					if (GetParent() != nullptr)
					{
						symname = *(GetParent()->GetSymbolName()) + "$$$";
					}
					symname += GetName() + "$$" + std::to_string(GetDirectTypeParametersCount()) + "$$";
					int pastfirst = false;
					for (auto &type : GetArgumentTypes(nullptr))
					{
						if (pastfirst)
						{
							symname += "$.$";
						}
						pastfirst = true;
						symname += type->GetSymbolRep();
					}
				}
			}
			return &symname;
		}

		NomCallableVersion* NomCallable::GetVersion(llvm::FunctionType* ft) const
		{
			auto result = versions.find(ft);
			if (result != versions.end())
			{
				return result->second;
			}
			versions[ft] = new NomCallableVersion(this, ft);
			return versions[ft];
		}

		llvm::FunctionType *NomCallable::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			if (llvmType == nullptr)
			{
				auto args_ = GetArgumentTypes(context);
				auto targs_ = GetArgumentTypeParameters();

				std::vector<Type *> args(targs_.size() + args_.size());
				unsigned int j;
				for (j = 0; j < targs_.size(); j++)
				{
					args[j] = TYPETYPE;
				}
				unsigned int i;
				for (i = 0; i < args_.size(); i++)
				{
					args[i + j] = (args_)[i]->GetLLVMType();
				}
				llvmType = FunctionType::get(GetReturnType(context)->GetLLVMType(), args, false);
			}
			return llvmType;
		}
		llvm::Function * NomCallable::findLLVMElement(llvm::Module & mod) const
		{
			return mod.getFunction(*GetSymbolName());
		}

		void NomCallableInternal::SetArgumentTypes(TypeList argTypes)
		{
			if (this->argTypes.data() != nullptr || argTypes.data() == nullptr)
			{
				throw new std::exception();
			}
			this->argTypes = argTypes;
		}
		void NomCallableInternal::SetArgumentTypes()
		{
			SetArgumentTypes(TypeList((NomTypeRef*)(this), (size_t)0));
		}
		TypeList NomCallableInternal::GetArgumentTypes(const NomSubstitutionContext* context) const
		{
			if (context == nullptr || context->GetTypeArgumentCount() == 0 || argTypes.size() == 0)
			{
				return argTypes;
			}
			bool hasVariables = false;
			for (auto at : argTypes)
			{
				if (at->ContainsVariables())
				{
					hasVariables = true;
					break;
				}
			}
			if (!hasVariables)
			{
				return argTypes;
			}
			NomTypeRef* tarr = (NomTypeRef*)(gcalloc(sizeof(NomTypeRef) * argTypes.size()));
			for (size_t i = 0; i < argTypes.size(); i++)
			{
				tarr[i] = argTypes[i]->SubstituteSubtyping(context);
			}
			return TypeList(tarr, argTypes.size());
		}
		int NomCallableInternal::GetArgumentCount() const
		{
			return argTypes.size();
		}
}
}