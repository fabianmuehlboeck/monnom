#include "NomLambdaCallTag.h"
#include <unordered_map>
#include "CompileHelpers.h"
#include "Defs.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		NomLambdaCallTag::NomLambdaCallTag(size_t _typeArgCount, size_t _argCount) : typeArgCount(_typeArgCount), argCount(_argCount)
		{
		}
		const NomLambdaCallTag* NomLambdaCallTag::GetCallTag(size_t typeArgCount, size_t argCount)
		{
			[[clang::no_destroy]] static unordered_map<size_t, NomLambdaCallTag*> callTags;
			size_t key = ((typeArgCount) << 32) + argCount;
			auto findResult = callTags.find(key);
			if (findResult == callTags.end())
			{
				auto newCallTag = new NomLambdaCallTag(typeArgCount, argCount);
				callTags[key] = newCallTag;
				return newCallTag;
			}
			return findResult->second;
		}
		NomLambdaCallTag::~NomLambdaCallTag()
		{
		}
		llvm::Constant* NomLambdaCallTag::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
		{
			return findLLVMElement(mod);
		}
		llvm::Constant* NomLambdaCallTag::findLLVMElement([[maybe_unused]] llvm::Module& mod) const
		{
			size_t key = ((typeArgCount) << 32) + argCount;
			key = key << 3;
			key++;
			return ConstantExpr::getIntToPtr(MakeInt<size_t>(key), POINTERTYPE);
		}
	}
}
