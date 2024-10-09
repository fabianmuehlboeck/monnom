#include "NomConstants.h"
#include "NomClass.h"
#include "NomClassType.h"
#include "NomVMInterface.h"
#include "NomMethod.h"
#include "NomStaticMethod.h"
#include "NomConstructor.h"
#include "StringClass.h"
#include "NomMemberContext.h"
#include "NomInstantiationRef.h"
#include "NomTypeVar.h"
#include "NomBottomType.h"
#include "NomDynamicType.h"
#include "NomLambda.h"
#include "Loader.h"
#include <iostream>
#include "NomMaybeType.h"
#include "NomTypeParameter.h"

using namespace std;
namespace Nom
{
	namespace Runtime
	{
		ConstantID NomConstantContext::operator[](const LocalConstantID local) const
		{
			return idmap.at(local);
		}

		ConstantID NomConstantContext::TryGetGlobalID(const LocalConstantID local) const
		{
			if (idmap.count(local) == 0)
			{
				return 0;
			}
			return idmap.at(local);
		}
		ConstantID NomConstantContext::GetGlobalID(const LocalConstantID local)
		{
			if (idmap.count(local) == 0)
			{
				idmap[local] = NomConstants::GetConstantID();
			}
			return idmap.at(local);
		}

		ConstantID NomConstantContext::AddString(LocalConstantID lid, const NomString &text)
		{
			idmap[lid]= NomConstants::AddString(text, TryGetGlobalID(lid));
			return idmap[lid];
		}
		ConstantID NomConstantContext::AddClass(LocalConstantID lid, const LocalConstantID library, const LocalConstantID name)
		{
			idmap[lid]= NomConstants::AddClass(GetGlobalID(library), GetGlobalID(name), TryGetGlobalID(lid));
			return idmap[lid];
		}

		ConstantID NomConstantContext::AddInterface(LocalConstantID lid, const LocalConstantID library, const LocalConstantID name)
		{
			idmap[lid] = NomConstants::AddInterface(GetGlobalID(library), GetGlobalID(name), TryGetGlobalID(lid));
			return idmap[lid];
		}

		ConstantID NomConstantContext::AddLambda(LocalConstantID lid)
		{
			idmap[lid] = NomConstants::AddLambda(TryGetGlobalID(lid));
			return idmap[lid];
		}

		ConstantID NomConstantContext::AddStruct(LocalConstantID lid)
		{
			idmap[lid] = NomConstants::AddStruct(TryGetGlobalID(lid));
			return idmap[lid];
		}


		ConstantID NomConstantContext::AddSuperClass(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID args)
		{
			idmap[lid]= NomConstants::AddSuperClass(GetGlobalID(cls), GetGlobalID(args), TryGetGlobalID(lid));
			return idmap[lid];
		}

		ConstantID NomConstantContext::AddSuperInterfaces(LocalConstantID lid, const llvm::SmallVector<std::tuple<LocalConstantID, LocalConstantID>, 4> &entries)
		{
			llvm::SmallVector<std::tuple<ConstantID, ConstantID>, 4> gentries;
			for (size_t i = 0; i < entries.size(); i++)
			{
				gentries.emplace_back(GetGlobalID(std::get<0>(entries[i])), GetGlobalID(std::get<1>(entries[i])));
			}
			idmap[lid]= NomConstants::AddSuperInterfaces(std::move(gentries), TryGetGlobalID(lid));
			return idmap[lid];
		}

		ConstantID NomConstantContext::AddMethod(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID name, const LocalConstantID typeArgs, const LocalConstantID argTypes)
		{
			idmap[lid]= NomConstants::AddMethod(GetGlobalID(cls), GetGlobalID(name), GetGlobalID(typeArgs), GetGlobalID(argTypes), TryGetGlobalID(lid));
			return idmap[lid];
		}
		ConstantID NomConstantContext::AddStaticMethod(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID name, const LocalConstantID typeArgs, const LocalConstantID argTypes)
		{
			idmap[lid]= NomConstants::AddStaticMethod(GetGlobalID(cls), GetGlobalID(name), GetGlobalID(typeArgs), GetGlobalID(argTypes), TryGetGlobalID(lid));
			return idmap[lid];
		}
		ConstantID NomConstantContext::AddCFunction(LocalConstantID lid, const LocalConstantID source, const LocalConstantID name, const LocalConstantID typeArgs, const LocalConstantID argTypes, const LocalConstantID returnType)
		{
			idmap[lid] = NomConstants::AddCFunction(GetGlobalID(source), GetGlobalID(name), GetGlobalID(typeArgs), GetGlobalID(argTypes), GetGlobalID(returnType), TryGetGlobalID(lid));
			return idmap[lid];
		}
		ConstantID NomConstantContext::AddTypeList(LocalConstantID lid, const llvm::SmallVector<LocalConstantID, 8> types)
		{
			llvm::SmallVector<ConstantID, 8> tps(types.size(), 0);
			for (int i = types.size() - 1; i >= 0; i--)
			{
				tps[i] = GetGlobalID(types[i]);
			}
			idmap[lid]= NomConstants::AddTypeList(tps, TryGetGlobalID(lid));
			return idmap[lid];
		}
		ConstantID NomConstantContext::AddClassType(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID typeArgs)
		{
			idmap[lid]= NomConstants::AddClassType(GetGlobalID(cls), GetGlobalID(typeArgs), TryGetGlobalID(lid));
			return idmap[lid];
		}
		ConstantID NomConstantContext::AddBottomType(LocalConstantID lid)
		{
			idmap[lid] = NomConstants::AddBottomType(TryGetGlobalID(lid));
			return idmap[lid];
		}
		ConstantID NomConstantContext::AddDynamicType(LocalConstantID lid)
		{
			idmap[lid] = NomConstants::AddDynamicType(TryGetGlobalID(lid));
			return idmap[lid];
		}
		ConstantID NomConstantContext::AddTypeVar(LocalConstantID lid, int index)
		{
			idmap[lid] = NomConstants::AddTypeVariable(index, TryGetGlobalID(lid));
			return idmap[lid];
		}
		ConstantID NomConstantContext::AddTypeParameters(LocalConstantID lid, llvm::ArrayRef<NomTypeParameterConstant *> parameters)
		{
			//int argsize = parameters.size();
			//NomTypeParameterConstant** argarr = nullptr;
			//if (argsize != 0)
			//{
			//	argarr = makealloca(NomTypeParameterConstant*, argsize);
			//	for (int i = 0; i < argsize; i++)
			//	{
			//		argarr[i] = new NomTypeParameterConstant(parameters[i].Variance, GetGlobalID(parameters[i].LowerBound), GetGlobalID(parameters[i].UpperBound));
			//	}
			//}
			idmap[lid] = NomConstants::AddTypeParameters(/*llvm::ArrayRef<NomTypeParameterConstant *>(argarr, argsize)*/ parameters, TryGetGlobalID(lid));
			return idmap[lid];
		}

		ConstantID NomConstantContext::AddConstructor(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID typeArgs, const LocalConstantID argTypes)
		{
			idmap[lid] = NomConstants::AddStaticMethod(GetGlobalID(cls), GetGlobalID(typeArgs), GetGlobalID(argTypes), TryGetGlobalID(lid));
			return idmap[lid];
		}

		ConstantID NomConstantContext::AddMaybeType(LocalConstantID lid, const LocalConstantID ptype)
		{
			idmap[lid] = NomConstants::AddMaybeType(GetGlobalID(ptype));
			return idmap[lid];
		}

		NomClassTypeRef NomClassTypeConstant::GetClassType(NomSubstitutionContextRef context) const
		{
			//TODO: possible memleak
			NomConstantType ct = NomConstants::Get(this->cls)->Type;
			if (ct == NomConstantType::CTClass)
			{
				value = NomConstants::GetClass(this->cls)->GetClass()->GetType(NomConstants::GetTypeList(this->args)->GetTypeList(context));
			}
			else
			{
				value = NomConstants::GetInterface(this->cls)->GetInterface()->GetType(NomConstants::GetTypeList(this->args)->GetTypeList(context));
			}
			return value;
		}

		void NomClassTypeConstant::Print(bool resolve)
		{
			cout << "ClassType ";
			NomConstants::PrintConstant(cls, resolve);
			cout << "<";
			NomConstants::PrintConstant(args, resolve);
			cout << ">";
		}
		NomInstantiationRef<NomClass>  NomSuperClassConstant::GetClassType(NomSubstitutionContextRef context) const
		{
			if (value.Elem == nullptr)
			{
				value = NomInstantiationRef<NomClass>(NomConstants::GetClass(this->SuperClass)->GetClass(), NomConstants::GetTypeList(this->Arguments)->GetTypeList(context));
			}
			return value;
		}

		void NomSuperClassConstant::Print(bool resolve)
		{
			cout << "SuperClass ";
			NomConstants::PrintConstant(SuperClass, resolve);
			cout << "<";
			NomConstants::PrintConstant(Arguments, resolve);
			cout << ">";
		}

		NomInstantiationRef<const NomMethod> NomMethodConstant::GetMethod(NomSubstitutionContextRef  context) const
		{
			//TODO : possible memleak
			{
				auto clstype = NomConstants::GetClassType(classConstant)->GetClassType(context);
				auto targs = NomConstants::GetTypeList(typeArgs)->GetTypeList(context);
				auto clsargs = clstype->Arguments;
				NomTypeRef* carr = nullptr;
				const int clsargscount = clsargs.size();
				const int targcount = targs.size() + clsargscount;
				if (targcount > 0)
				{
					carr = (NomTypeRef*)nmalloc(sizeof(NomTypeRef) * targcount);
					for (int i = 0; i < targcount; i++)
					{
						if (i < clsargscount)
						{
							carr[i] = clsargs[i];
						}
						else
						{
							carr[i] = targs[i - clsargscount];
						}
					}
				}
				auto argarggref = llvm::ArrayRef<NomTypeRef>(carr, targcount);
				auto nsc = NomSubstitutionContextList(argarggref);
				if (clstype->Named->IsInterface())
				{
					method = (dynamic_cast<const NomInterface*>(clstype->Named))->GetMethod(&nsc, NomConstants::GetString(methodName)->GetText(), targs, NomConstants::GetTypeList(argTypes)->GetTypeList(&nsc));
				}
				else
				{
					method = (dynamic_cast<const NomClass*>(clstype->Named))->GetMethod(&nsc, NomConstants::GetString(methodName)->GetText(), targs, NomConstants::GetTypeList(argTypes)->GetTypeList(&nsc));
				}
			}
			return method;
		}

		void NomMethodConstant::Print(bool resolve)
		{
			cout << "Method ";
			NomConstants::PrintConstant(classConstant, resolve);
			cout << ".";
			NomConstants::PrintConstant(methodName, resolve);
			cout << "<";
			NomConstants::PrintConstant(typeArgs, resolve);
			cout << ">(";
			NomConstants::PrintConstant(argTypes, resolve);
			cout << ")";
		}

		NomInstantiationRef<const NomStaticMethod> NomStaticMethodConstant::GetStaticMethod(NomSubstitutionContextRef  context) const
		{
			//TODO : MEMLEAK
			//MEMLEAK : AAAH
			//if (method.Elem == nullptr)
			{
				method = NomConstants::GetClass(ClassConstant)->GetClass()->GetStaticMethod(NomConstants::GetString(MethodName)->GetText(), NomConstants::GetTypeList(TypeArgs)->GetTypeList(context), NomConstants::GetTypeList(ArgTypes)->GetTypeList(context));
			}
			return method;
		}

		void NomStaticMethodConstant::Print(bool resolve)
		{
			cout << "StaticMethod ";
			NomConstants::PrintConstant(ClassConstant, resolve);
			cout << ".";
			NomConstants::PrintConstant(MethodName, resolve);
			cout << "<";
			NomConstants::PrintConstant(TypeArgs, resolve);
			cout << ">(";
			NomConstants::PrintConstant(ArgTypes, resolve);
			cout << ")";
		}

		const NomClass* NomClassConstant::GetClass()
		{
			if (cls == nullptr)
			{
				throw new std::exception();
				////cls = NomClass::getClass(((NomStringConstant*)NomConstants::Get(Name))->GetText());
				//auto libname = NomConstants::GetString(this->Library)->GetText()->ToStdString();
				//auto clsname = NomConstants::GetString(this->Name)->GetText();
				//cls = Loader::GetInstance()->GetLibrary(&libname)->GetClass(clsname);
			}
			return cls;
		}

		void NomClassConstant::EnsureClassLoaded(NomModule* mod)
		{
			if (cls == nullptr)
			{
				//cls = NomClass::getClass(((NomStringConstant*)NomConstants::Get(Name))->GetText());
				auto libname = NomConstants::GetString(this->Library)->GetText()->ToStdString();
				auto clsname = NomConstants::GetString(this->Name)->GetText();
				cls = Loader::GetInstance()->GetLibrary(&libname)->GetClass(clsname, mod);
			}
		}

		void NomClassConstant::Print(bool resolve)
		{
			cout << "Class ";
			NomConstants::PrintConstant(Library, resolve);
			cout << "::";
			NomConstants::PrintConstant(Name, resolve);
		}

		llvm::ArrayRef<NomTypeRef> NomTypeListConstant::GetTypeList(NomSubstitutionContextRef context) const // delete copy-constructors, default move-constructors
		{
			//TODO : MEMLEAK
			//MEMLEAK : AAAH
			//if (typeRefs == nullptr)
			{
				typeRefs = (NomTypeRef*)nmalloc(sizeof(NomTypeRef) * types.size());
				for (size_t i = types.size(); i > 0;)
				{
					i--;
					typeRefs[i] = NomConstants::GetType(context, types[i]);
				}
			}
			return llvm::ArrayRef<NomTypeRef>(typeRefs, types.size());
		}

		void NomTypeListConstant::Print(bool resolve)
		{
			cout << "TypeList (";
			bool first = true;
			for (auto type : types)
			{
				if (!first)
				{
					cout << ", ";
				}
				first = false;
				NomConstants::PrintConstant(type, resolve);
			}
			cout << ")";
		}
		//ClassTypeList NomClassTypeListConstant::GetClassTypeList()
		//{
		//	ClassTypeList ret;
		//	for (auto& typeconst : types)
		//	{
		//		ret->push_back(NomConstants::GetClassType(typeconst)->GetClassType());
		//	}
		//	return ret;
		//}
		const NomInterface* NomInterfaceConstant::GetInterface() const
		{
			if (iface == nullptr)
			{
				throw new std::exception();
			//	//iface = NomInterface::GetInterface(((NomStringConstant*)NomConstants::Get(Name))->GetText());
			//	auto libname = NomConstants::GetString(Library)->GetText()->ToStdString();
			//	auto ifacename = NomConstants::GetString(Name)->GetText();
			//	iface = Loader::GetInstance()->GetLibrary(&libname)->GetInterface(ifacename);
			}
			return iface;
		}


		void NomInterfaceConstant::EnsureInterfaceLoaded(NomModule* mod)
		{
			if (iface == nullptr)
			{
				auto libname = NomConstants::GetString(Library)->GetText()->ToStdString();
				auto ifacename = NomConstants::GetString(Name)->GetText();
				iface = Loader::GetInstance()->GetLibrary(&libname)->GetInterface(ifacename,mod);
			}
		}

		void NomInterfaceConstant::Print(bool resolve)
		{
			cout << "Interface ";
			NomConstants::PrintConstant(Library, resolve);
			cout << "::";
			NomConstants::PrintConstant(Name, resolve);
		}

		//static NomClassTypeListConstant * GetClassTypeList(const ConstantID constant)
		//{
		//	if (constant == 0)
		//	{
		//		static NomClassTypeListConstant defaultConst{ llvm::SmallVector<ConstantID, 8>() };
		//		return &defaultConst;
		//	}
		//	NomConstant *cnstnt = constants()[constant];
		//	if (cnstnt->Type != NomConstantType::CTClassTypeList)
		//	{
		//		throw new std::exception();
		//	}
		//	return (NomClassTypeListConstant *)cnstnt;
		//}

		void NomConstants::PrintConstant(const ConstantID constant, bool resolve)
		{
			cout << "$" << std::dec << constant;
			if (resolve && constant > 0)
			{
				cout << "(";
				NomConstants::Get(constant)->Print(resolve);
				cout << ")";
			}
		}

		NomStringConstant* NomConstants::GetString(const ConstantID constant)
		{
			if (constant == 0)
			{
				static NomStringConstant emptyStringConstant = NomStringConstant(NomString(""));
				return &emptyStringConstant;
			}
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTString)
			{
				throw new std::exception();
			}
			return (NomStringConstant*)cnstnt;
		}

		NomClassConstant* NomConstants::GetClass(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTClass)
			{
				throw new std::exception();
			}
			return (NomClassConstant*)cnstnt;
		}

		NomLambdaConstant* NomConstants::GetLambda(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTLambda)
			{
				throw new std::exception();
			}
			return (NomLambdaConstant*)cnstnt;
		}

		NomRecordConstant* NomConstants::GetRecord(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTRecord)
			{
				throw new std::exception();
			}
			return (NomRecordConstant*)cnstnt;
		}

		NomMethodConstant* NomConstants::GetMethod(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTMethod)
			{
				throw new std::exception();
			}
			return (NomMethodConstant*)cnstnt;
		}

		NomStaticMethodConstant* NomConstants::GetStaticMethod(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTStaticMethod)
			{
				throw new std::exception();
			}
			return (NomStaticMethodConstant*)cnstnt;
		}

		NomConstructorConstant* NomConstants::GetConstructor(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTConstructor)
			{
				throw new std::exception();
			}
			return (NomConstructorConstant*)cnstnt;
		}

		NomTypeListConstant* NomConstants::GetTypeList(const ConstantID constant)
		{
			if (constant == 0)
			{
				static NomTypeListConstant defaultConst{ llvm::SmallVector<ConstantID, 8>() };
				return &defaultConst;
			}
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTTypeList)
			{
				throw new std::exception();
			}
			return (NomTypeListConstant*)cnstnt;
		}

		NomCFunctionConstant* NomConstants::GetCFunction(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CFunctionConstant)
			{
				throw new std::exception();
			}
			return (NomCFunctionConstant*)cnstnt;
		}

		NomClassTypeConstant* NomConstants::GetClassType(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTClassType)
			{
				throw new std::exception();
			}
			return (NomClassTypeConstant*)cnstnt;
		}
		NomSuperClassConstant* NomConstants::GetSuperClass(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTSuperClass)
			{
				throw new std::exception();
			}
			return (NomSuperClassConstant*)cnstnt;
		}
		NomSuperInterfacesConstant* NomConstants::GetSuperInterfaces(const ConstantID constant)
		{
			if (constant == 0)
			{
				static llvm::SmallVector<std::tuple<ConstantID, ConstantID>, 4 > entries;
				static NomSuperInterfacesConstant defaultConst(entries);
				return &defaultConst;
			}
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTSuperInterfaces)
			{
				throw new std::exception();
			}
			return (NomSuperInterfacesConstant*)cnstnt;
		}
		NomInterfaceConstant* NomConstants::GetInterface(const ConstantID constant)
		{
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTInterface)
			{
				throw new std::exception();
			}
			return (NomInterfaceConstant*)cnstnt;
		}
		NomTypeParametersConstant* NomConstants::GetTypeParameters(const ConstantID constant)
		{
			if (constant == 0)
			{
				static NomTypeParametersConstant defaultConst(llvm::ArrayRef< NomTypeParameterConstant*>((NomTypeParameterConstant**)nullptr, (size_t)0));
				return &defaultConst;
			}
			NomConstant* cnstnt = constants()[constant];
			if (cnstnt->Type != NomConstantType::CTTypeParameters)
			{
				throw new std::exception();
			}
			return (NomTypeParametersConstant*)cnstnt;
		}
		NomTypeRef NomConstants::GetType(NomSubstitutionContextRef context, const ConstantID constant, bool defaultBottom)
		{
			if (constant == 0)
			{
				if (defaultBottom)
				{
					return NomType::NothingRef;
				}
				return NomType::AnythingRef;
			}
			NomConstant* nc = constants()[constant];
			if (nc->Type == NomConstantType::CTClassType)
			{
				return ((NomClassTypeConstant*)nc)->GetClassType(context);
			}
			if (nc->Type == NomConstantType::CTIntersection)
			{
				
			}
			if (nc->Type == NomConstantType::CTTypeVar)
			{
				return ((NomTypeVarConstant*)nc)->GetTypeRef(context);
			}
			if (nc->Type == NomConstantType::CTBottom)
			{
				return NomBottomType::Instance();
			}
			if (nc->Type == NomConstantType::CTDynamic)
			{
				return &NomDynamicType::Instance();
			}
			if (nc->Type == NomConstantType::CTMaybe)
			{
				return ((NomMaybeTypeConstant*)nc)->GetTypeRef(context);
			}
			//if(nc->Type == NomConstantType::) //Bottom
			//TODO: other types
			throw constant;
		}

		ConstantID NomConstants::GetConstantID()
		{
			constants().push_back(nullptr);
			ConstantID ret = constants().size() - 1;
			return ret;
		}
		ConstantID NomConstants::AddString(const NomString& text, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomStringConstant(text));
			return cid;
		}
		ConstantID NomConstants::AddClass(const ConstantID library, const ConstantID name, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomClassConstant(library, name));
			return cid;
		}

		ConstantID NomConstants::AddInterface(const ConstantID library, const ConstantID name, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomInterfaceConstant(library, name));
			return cid;
		}

		ConstantID NomConstants::AddLambda(ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomLambdaConstant());
			return cid;
		}

		ConstantID NomConstants::AddStruct(ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomRecordConstant());
			return cid;
		}

		//static ConstantID AddTypeParameter(ParameterVariance variance, const ConstantID lowerBound, const ConstantID upperBound, ConstantID cid = 0)
		//{
		//	if (cid == 0)
		//	{
		//		cid = GetConstantID();
		//	}
		//	constants()[cid] = (new NomTypeParameterConstant(variance, lowerBound, upperBound));
		//	return cid;
		//}

		ConstantID NomConstants::AddMethod(const ConstantID cls, const ConstantID name, const ConstantID typeArgs, const ConstantID argTypes, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomMethodConstant(cls, name, typeArgs, argTypes));
			return cid;
		}

		ConstantID NomConstants::AddStaticMethod(const ConstantID cls, const ConstantID name, const ConstantID typeArgs, const ConstantID argTypes, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomStaticMethodConstant(cls, name, typeArgs, argTypes));
			return cid;
		}

		ConstantID NomConstants::AddConstructor(const ConstantID cls, const ConstantID typeArgs, const ConstantID argTypes, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomConstructorConstant(cls, typeArgs, argTypes));
			return cid;
		}

		ConstantID NomConstants::AddCFunction(const ConstantID source, const ConstantID name, const ConstantID typeArgs, const ConstantID argTypes, const ConstantID returnType, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomCFunctionConstant(source, name, typeArgs, argTypes, returnType));
			return cid;
		}

		ConstantID NomConstants::AddTypeList(const llvm::ArrayRef<ConstantID> types, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomTypeListConstant(types));
			return cid;
		}

		ConstantID NomConstants::AddClassType(const ConstantID cls, const ConstantID typeArgs, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomClassTypeConstant(cls, typeArgs));
			return cid;
		}

		ConstantID NomConstants::AddBottomType(ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomBottomConstant());
			return cid;
		}

		ConstantID NomConstants::AddDynamicType(ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomDynamicTypeConstant());
			return cid;
		}

		ConstantID NomConstants::AddMaybeType(const ConstantID ptype, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = new NomMaybeTypeConstant(ptype);
			return cid;
		}

		ConstantID NomConstants::AddTypeVariable(int index, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomTypeVarConstant(index));
			return cid;
		}

		ConstantID NomConstants::AddTypeParameters(llvm::ArrayRef<NomTypeParameterConstant*> typeParameters, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomTypeParametersConstant(typeParameters));
			return cid;
		}
		ConstantID NomConstants::AddSuperClass(const ConstantID cls, const ConstantID args, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomSuperClassConstant(cls, args));
			return cid;
		}
		ConstantID NomConstants::AddSuperInterfaces(const llvm::SmallVector<std::tuple<ConstantID, ConstantID>, 4>& entries, ConstantID cid)
		{
			if (cid == 0)
			{
				cid = GetConstantID();
			}
			constants()[cid] = (new NomSuperInterfacesConstant(std::move(entries)));
			return cid;
		}
		NomInstantiationRef<const NomConstructor> NomConstructorConstant::GetConstructor(NomSubstitutionContextRef  context) const
		{
			if (constructor.Elem == nullptr)
			{
				constructor = NomConstants::GetClass(ClassConstant)->GetClass()->GetConstructor(NomConstants::GetTypeList(TypeArgs)->GetTypeList(context), NomConstants::GetTypeList(ArgTypes)->GetTypeList(context));
			}
			return constructor;
		}

		void NomConstructorConstant::Print(bool resolve)
		{
			cout << "Constructor ";
			NomConstants::PrintConstant(ClassConstant, resolve);
			cout << "<";
			NomConstants::PrintConstant(TypeArgs, resolve);
			cout << ">(";
			NomConstants::PrintConstant(ArgTypes, resolve);
			cout << ")";
		}
		llvm::Constant* NomStringConstant::getObject(llvm::Module& mod)
		{
			if (obj == nullptr)
			{
				obj = Text.GetLLVMConstant(mod);
				//obj = CPP_NOM_CreateInstance(_RTStringClassRTC, 0, nullptr, nullptr);
				//obj->Fields() = (intptr_t)&Text;
			}
			return obj;
		}
		void NomStringConstant::Print(bool resolve)
		{
			cout << "\"" << Text.ToStdString() << "\"";
		}
		size_t NomTypeListConstant::GetSize() const
		{
			return types.size();
		}
		const llvm::ArrayRef<NomInstantiationRef<NomInterface>> NomSuperInterfacesConstant::GetSuperInterfaces(NomSubstitutionContextRef  context) const
		{
			if (ifaces.size() == 0 && entries.size() != 0)
			{
				for (auto entry : entries)
				{
					ifaces.push_back(NomInstantiationRef<NomInterface>(NomConstants::GetInterface(std::get<0>(entry))->GetInterface(), NomConstants::GetTypeList(std::get<1>(entry))->GetTypeList(context)));
				}
			}
			return ifaces;
		}

		void NomSuperInterfacesConstant::Print(bool resolve)
		{
			cout << "SuperInterfaces (";
			bool first = true;
			for (auto entry : entries)
			{
				if (!first)
				{
					cout << ",";
				}
				first = false;
				NomConstants::PrintConstant(get<0>(entry), resolve);
				cout << "<";
				NomConstants::PrintConstant(get<1>(entry), resolve);
				cout << ">";
			}
			cout << ")";
		}

		NomTypeRef NomTypeVarConstant::GetTypeRef(NomSubstitutionContextRef  context) const
		{
			return context->GetTypeVariable(index);
		}
		void NomTypeVarConstant::Print(bool resolve)
		{
			cout << "TypeVar " << std::dec << index;
		}
		NomLambda* NomLambdaConstant::GetLambda() const
		{
			return lambda;
		}
		void NomLambdaConstant::SetLambda(NomLambda* lambda) const
		{
			if (this->lambda != nullptr)
			{
				throw new std::exception(); //should only happen once; at lambda creation
			}
			this->lambda = lambda;
		}
		void NomLambdaConstant::Print(bool resolve)
		{
			cout << "Lambda";
		}
		void NomTypeParametersConstant::Print(bool resolve)
		{
			cout << "TypeParameters (";
			bool first = true;
			int counter = 0;
			for (auto tpc : typeParams)
			{
				if (!first)
				{
					cout << ", ";
				}
				first = false;
				NomConstants::PrintConstant(tpc->LowerBound, resolve);
				cout << " <: ";
				switch (tpc->Variance)
				{
				case ParameterVariance::Covariant:
					cout << "+";
					break;
				case ParameterVariance::Contravariant:
					cout << "-";
					break;
				case ParameterVariance::Invariant:
					cout << "!";
					break;
				}
				cout << std::dec << counter;
				cout << " <: ";
				NomConstants::PrintConstant(tpc->UpperBound, resolve);
				counter++;
			}
			cout << ")";
		}
		void NomBottomConstant::Print(bool resolve)
		{
			cout << "Bottom";
		}
		void NomDynamicTypeConstant::Print(bool resolve)
		{
			cout << "Dyn";
		}
		NomRecord* NomRecordConstant::GetRecord() const
		{
			return this->structure;
		}
		void NomRecordConstant::SetStruct(NomRecord* structure) const
		{
			this->structure = structure;
		}
		void NomRecordConstant::Print(bool resolve)
		{
			cout << "Record";
		}
		NomMaybeTypeRef NomMaybeTypeConstant::GetTypeRef(NomSubstitutionContextRef context) const
		{
			if (value == nullptr)
			{
				value = NomMaybeType::GetMaybeType(NomConstants::GetType(context, ptype, false));
			}
			return value;
		}
		void NomMaybeTypeConstant::Print(bool resolve)
		{
			NomConstants::PrintConstant(ptype, resolve);
			cout << "?";
		}
		void NomMaybeTypeConstant::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(ptype);
		}
		llvm::Function* NomCFunctionConstant::GetCFunction(llvm::Module &mod) const
		{
			auto name = NomConstants::GetString(FunctionNameConstant)->GetText()->ToStdString();
			auto fun = mod.getFunction(name);
			if (fun == 0) {
				int paramc = GetNumTypeParameters();
				NomTypeRef types[paramc];
				for (int i = 0; i < paramc; i++)
				{
					NomTypeParameterInternal ntp(nullptr, 0, NomType::AnythingRef, NomType::NothingRef);
					types[i] = ntp.GetVariable();
				}
				NomSubstitutionContextList nscl(TypeList(types, paramc));
				auto argtypes = GetArgumentTypes(&nscl);
				int argnum = argtypes.size() + paramc;
				llvm::Type* funargtypes[argnum];
				for (int i = 0; i < argnum; i++) {
					if (i < paramc) {
						funargtypes[i] = TYPETYPE;
					}
					else {
						funargtypes[i] = argtypes[i - paramc]->GetLLVMType();
					}
				}

				auto returnType = GetReturnType(&nscl);
				llvm::FunctionType* funtype = llvm::FunctionType::get(returnType->GetLLVMType(), llvm::ArrayRef<llvm::Type*>(funargtypes, argnum), false);
				fun = llvm::Function::Create(funtype, llvm::GlobalValue::LinkageTypes::ExternalLinkage, name, mod);
			}
			return fun;
		}
		size_t NomCFunctionConstant::GetNumTypeParameters() const
		{
			return NomConstants::GetTypeParameters(TypeArgs)->GetSize();
		}
		llvm::ArrayRef<NomTypeParameterConstant*> NomCFunctionConstant::GetTypeParameters() const
		{
			return NomConstants::GetTypeParameters(TypeArgs)->GetParameters();
		}
		TypeList NomCFunctionConstant::GetArgumentTypes(NomSubstitutionContextRef context) const
		{
			return NomConstants::GetTypeList(ArgTypes)->GetTypeList(context);
		}
		NomTypeRef NomCFunctionConstant::GetReturnType(NomSubstitutionContextRef context) const
		{
			return NomConstants::GetType(context, ReturnType);
		}
		void NomCFunctionConstant::Print(bool resolve)
		{
			cout << "CFunction ";
			NomConstants::PrintConstant(this->SourceNameConstant, resolve);
			cout << "::";
			NomConstants::PrintConstant(this->FunctionNameConstant, resolve);
			cout << "<";
			NomConstants::PrintConstant(TypeArgs, resolve);
			cout << ">(";
			NomConstants::PrintConstant(ArgTypes, resolve);
			cout << ") : ";
			NomConstants::PrintConstant(ReturnType, resolve);
		}
	}
}
