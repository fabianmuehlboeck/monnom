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
		NomLambdaCallTag::NomLambdaCallTag(int typeArgCount, int argCount) : typeArgCount(typeArgCount), argCount(argCount)
		{
		}
		const NomLambdaCallTag* NomLambdaCallTag::GetCallTag(int typeArgCount, int argCount)
		{
			static unordered_map<size_t, NomLambdaCallTag*> callTags;
			size_t key = (((size_t)typeArgCount) << 32) + (size_t)argCount;
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
		llvm::Constant* NomLambdaCallTag::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			return findLLVMElement(mod);
		}
		llvm::Constant* NomLambdaCallTag::findLLVMElement(llvm::Module& mod) const
		{
			size_t key = (((size_t)typeArgCount) << 32) + (size_t)argCount;
			key = key << 3;
			key++;
			return ConstantExpr::getIntToPtr(MakeInt<size_t>(key), POINTERTYPE);
		}
	}
}