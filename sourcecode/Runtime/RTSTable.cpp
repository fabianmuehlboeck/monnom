#include "RTSTable.h"
#include "RTSignature.h"
#include "CompileHelpers.h"
#include "NomPartialApplication.h"
#include "Metadata.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTSTable::GetLLVMType()
		{
			static llvm::StructType* shst = StructType::create(LLVMCONTEXT, "RT_NOM_StructHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				shst->setBody(
					RTSignature::GetLLVMType()->getPointerTo(),							// Signature
					POINTERTYPE,														// Function Pointer
					NomPartialApplication::GetDynamicDispatcherType()->getPointerTo(),	// Dispatcher Pointer
					inttype(32),														// lambda type argument count
					inttype(32),														// lambda argument count
					POINTERTYPE															// NOM IR Link
				);
			}
			return shst;
		}

		llvm::Constant* RTSTable::GenerateConstant(llvm::Constant* signature, llvm::Constant* functionPtr, llvm::Constant* dispatcherPtr, llvm::Constant* lambdaTypeArgCount, llvm::Constant* lambdaArgCount, llvm::Constant* irlink)
		{
			return ConstantStruct::get(GetLLVMType(), { signature, functionPtr, dispatcherPtr, lambdaTypeArgCount, lambdaArgCount, irlink });
		}

		llvm::Value* RTSTable::GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptor)
		{
			auto loadInst = MakeInvariantLoad(builder, descriptor, GetLLVMType()->getPointerTo(), MakeInt32(RTSTableFields::Signature), "Signature", AtomicOrdering::NotAtomic);
			return loadInst;
		}

		llvm::Value* RTSTable::GenerateReadLambdaMethod(NomBuilder& builder, llvm::Value* descriptor)
		{
			auto loadInst = MakeInvariantLoad(builder, descriptor, GetLLVMType()->getPointerTo(), MakeInt32(RTSTableFields::LambdaMethod), "LambdaMethod", AtomicOrdering::NotAtomic);
			return loadInst;
		}

		llvm::Value* RTSTable::GenerateReadLambdaDispatcher(NomBuilder& builder, llvm::Value* descriptor)
		{
			auto loadInst = MakeInvariantLoad(builder, descriptor, GetLLVMType()->getPointerTo(), MakeInt32(RTSTableFields::LambdaDispatcher), "LambdaDispatcher", AtomicOrdering::NotAtomic);
			return loadInst;
		}

		llvm::Value* RTSTable::GenerateReadLambdaTypeArgCount(NomBuilder& builder, llvm::Value* descriptor)
		{
			return MakeInvariantLoad(builder, descriptor, GetLLVMType()->getPointerTo(), MakeInt32(RTSTableFields::TypeArgCount), "LambdaTypeArgCount", AtomicOrdering::NotAtomic);
		}

		llvm::Value* RTSTable::GenerateReadLambdaArgCount(NomBuilder& builder, llvm::Value* descriptor)
		{
			return MakeInvariantLoad(builder, descriptor, GetLLVMType()->getPointerTo(), MakeInt32(RTSTableFields::ArgCount), "LambdaArgCount", AtomicOrdering::NotAtomic);
		}

		llvm::Value* RTSTable::GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* descriptor)
		{
			auto loadInst = MakeInvariantLoad(builder, descriptor, GetLLVMType()->getPointerTo(), MakeInt32(RTSTableFields::NomIRLink), "NomIRLink", AtomicOrdering::NotAtomic);
			return loadInst;
		}

	}
}