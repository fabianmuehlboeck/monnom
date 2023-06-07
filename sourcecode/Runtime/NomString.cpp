#include "NomString.h"
#include "ObjectHeader.h"
#include "StringClass.h"
#include "NomAlloc.h"
#include "NomJIT.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "GlobalNameAddressLookupList.h"
#include "BoolClass.h"
#include "instructions/CastInstruction.h"
#include "NomClassType.h"
#include <unordered_map>
#include "CompileHelpers.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Constant* NomString::GetLLVMConstant(llvm::Module& mod) const
		{
			return GetLLVMElement(mod);
		}
		void* NomString::GetStringObject() const
		{
			void* ret = (void*)(((char**)gcalloc(ObjectHeader::SizeOf()+sizeof(char *))) + 1);
			GetWriteVTableFunction()(ret, *((void**)(NomJIT::Instance().lookup("NOM_CDREF_String_0")->getValue())));
			GetWriteFieldFunction()(ret, 0, const_cast<NomString*>(this));
			return ret;
		}
		
		const std::string *getStringID(NomStringRef str)
		{
			static std::unordered_map<NomStringRef, std::string, NomStringHash, NomStringEquality> stringKeys;
			auto result = stringKeys.find(str);
			if (result != stringKeys.end())
			{
				return &stringKeys[str];
			}
			stringKeys[str] = "NOM_STR_" + std::to_string(stringKeys.size());
			return &stringKeys[str];
		}

		llvm::Constant* NomString::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			return ObjectHeader::GetGlobal(mod, linkage, llvm::Twine(getStringID(this)->data()), NomStringClass::GetInstance()->GetLLVMElement(mod), llvm::ConstantArray::get(arrtype(REFTYPE, 1), { llvm::ConstantExpr::getIntToPtr(MakeInt((const intptr_t)this), REFTYPE) }));
		}

		llvm::Constant* NomString::findLLVMElement(llvm::Module& mod) const
		{
			return ObjectHeader::FindGlobal(mod, *getStringID(this));
			//return mod.getGlobalVariable(getStringID(this)->data());
		}
	}
}