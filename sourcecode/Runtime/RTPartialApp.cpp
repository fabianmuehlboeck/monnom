#include "RTPartialApp.h"
#include "Defs.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/DerivedTypes.h"
POPDIAGSUPPRESSION
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
				arrtype(llvm::StructType::get(numtype(uint64_t), POINTERTYPE.AsLLVMType()), 0)  
				}, "RT_NOM_PartialAppDescriptor");
			return pat;
		}
		static llvm::Constant* GetKeyConstant(size_t typeargcount, size_t argcount)
		{
			return MakeInt<uint64_t>(((typeargcount) << 32) + argcount);
		}
		llvm::Constant* RTPartialApp::CreateConstant(llvm::ArrayRef<std::pair<std::pair<size_t, size_t>, llvm::Constant*>> entries)
		{
			llvm::Constant ** cbuf = makealloca(llvm::Constant*, entries.size());
			int pos = 0;
			for (auto &entry : entries)
			{
				cbuf[pos] = llvm::ConstantStruct::get(llvm::StructType::get(numtype(uint64_t), POINTERTYPE.AsLLVMType()), GetKeyConstant(std::get<0>(std::get<0>(entry)), std::get<1>(std::get<0>(entry))), std::get<1>(entry));
			}
			auto arr = llvm::ConstantArray::get(arrtype(llvm::StructType::get(numtype(uint64_t), POINTERTYPE.AsLLVMType()), entries.size()), llvm::ArrayRef<llvm::Constant*>(cbuf, entries.size()));
			return ConstantStruct::get(llvm::StructType::get(numtype(uint64_t), arr->getType()), MakeInt<uint64_t>(entries.size()), arr);
		}

		llvm::Value* RTPartialApp::GenerateFindDispatcher([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] llvm::Value* partialAppDesc, [[maybe_unused]] size_t typeargcount, [[maybe_unused]] size_t argcount)
		{
			throw new std::exception();
		}
	}
}
