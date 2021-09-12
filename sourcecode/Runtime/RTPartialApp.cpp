#include "RTPartialApp.h"
#include "Defs.h"
#include "llvm/IR/DerivedTypes.h"
#include "RTDictionary.h"
#include "CompileHelpers.h"
#include "ObjectHeader.h"
#include "NomNameRepository.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomPartialApplication.h"
#include "RTVTable.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTPartialApp::GetLLVMType()
		{
			static llvm::StructType* pat = llvm::StructType::create(LLVMCONTEXT, { 
				RTVTable::GetLLVMType(),
				numtype(uint64_t), 
				arrtype(llvm::StructType::get(numtype(uint64_t), POINTERTYPE), 0)  
				}, "RT_NOM_PartialAppDescriptor");
			return pat;
		}
		llvm::Constant* GetKeyConstant(uint32_t typeargcount, uint32_t argcount)
		{
			return MakeInt<uint64_t>((((uint64_t)typeargcount) << 32) + (uint64_t)argcount);
		}
		llvm::Constant* RTPartialApp::CreateConstant(llvm::ArrayRef<std::pair<std::pair<uint32_t, uint32_t>, llvm::Constant*>> entries)
		{
			llvm::Constant ** cbuf = makealloca(llvm::Constant*, entries.size());
			int pos = 0;
			for (auto &entry : entries)
			{
				cbuf[pos] = llvm::ConstantStruct::get(llvm::StructType::get(numtype(uint64_t), POINTERTYPE), GetKeyConstant(std::get<0>(std::get<0>(entry)), std::get<1>(std::get<0>(entry))), std::get<1>(entry));
			}
			auto arr = llvm::ConstantArray::get(arrtype(llvm::StructType::get(numtype(uint64_t), POINTERTYPE), entries.size()), llvm::ArrayRef<llvm::Constant*>(cbuf, entries.size()));
			return ConstantStruct::get(llvm::StructType::get(numtype(uint64_t), arr->getType()), MakeInt<uint64_t>(entries.size()), arr);
		}

		llvm::Value* RTPartialApp::GenerateFindDispatcher(NomBuilder& builder, llvm::Value* partialAppDesc, uint32_t typeargcount, uint32_t argcount)
		{
			throw new std::exception();
		}
	}
}