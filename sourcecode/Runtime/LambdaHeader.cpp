#include "LambdaHeader.h"
#include "RTVTable.h"
#include "RefValueHeader.h"
#include "RTLambda.h"
#include "NomVMInterface.h"
#include "CompileHelpers.h"
#include "TypeOperations.h"
#include "NomLambda.h"
#include "NomTypeRegistry.h"
#include "RTCompileConfig.h"
#include "StructuralValueHeader.h"
#include "PWLambda.h"
#include "PWTypeArr.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* LambdaHeader::GetLLVMType()
		{
			static StructType* lhtype = StructType::create(LLVMCONTEXT, "RT_NOM_LambdaHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				lhtype->setBody(
					StructuralValueHeader::GetLLVMType(),
					arrtype(REFTYPE, 0));																	// closure fields
			}
			return lhtype;
		}

		llvm::Value* LambdaHeader::GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, const NomLambda* lambda)
		{
			return PWLambda(thisObj).ReadTypeArgument(builder, argindex);
		}
		llvm::Value* LambdaHeader::GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, const NomLambda *lambda)
		{
			return PWLambdaPrecise(thisObj, lambda).PointerToTypeArgs(builder);
		}
		void LambdaHeader::GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, llvm::ArrayRef<llvm::Value*> arguments, llvm::Constant* descriptorRef, const NomLambda *lambda)
		{
			Function* fun = builder->GetInsertBlock()->getParent();
			Function* allocfun = GetClosureAlloc(fun->getParent());

			llvm::Value* newmem = builder->CreatePointerCast(builder->CreateCall(allocfun, { MakeInt<size_t>(arguments.size()+(NomLambdaOptimizationLevel>0?1:0)), MakeInt<size_t>(typeArguments.size()) }), GetLLVMType()->getPointerTo());

			PWLambda newlambda = newmem;

			int fieldIndex=0;
			for (auto arg : arguments)
			{
				newlambda.WriteField(builder, fieldIndex, EnsurePacked(builder, arg));
				fieldIndex++;
			}
			int targIndex = 0;
			if (NomLambdaOptimizationLevel > 0)
			{
				RefValueHeader::GenerateWriteRawInvoke(builder, newmem, ConstantPointerNull::get(POINTERTYPE));
			}
			auto vTablePtr = ConstantExpr::getPointerCast(descriptorRef, RTVTable::GetLLVMType()->getPointerTo());

			Constant* rawInvokePtr=nullptr;
			if (NomLambdaOptimizationLevel > 0)
			{
				rawInvokePtr = lambda->Body.GetLLVMElement(*fun->getParent());
			}

			StructuralValueHeader::GenerateInitializationCode(builder, newmem, typeArguments, vTablePtr, rawInvokePtr);

			auto refValHeader = builder->CreateGEP(GetLLVMType(), newmem, { MakeInt32(0), MakeInt32(LambdaHeaderFields::StructValueHeader), MakeInt32(StructuralValueHeaderFields::RefValueHeader) }, "RefValHeader");

			builder->CreateRet(refValHeader);
		}
	}
}