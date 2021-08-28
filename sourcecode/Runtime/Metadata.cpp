#include "Metadata.h"
#include "Context.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		MDNode* getStructDescriptorInvariantNode()
		{
			return getGeneralInvariantNode();
		}
		llvm::MDNode* getGeneralInvariantNode()
		{
			static MDNode* mdn = MDNode::get(LLVMCONTEXT, { });
			return mdn;
		}
	}
}