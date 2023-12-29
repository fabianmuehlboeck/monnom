#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
POPDIAGSUPPRESSION
#include "CompileEnv.h"
#include "NomBuilder.h"
#include "NomValue.h"
#include "RTValuePtr.h"

namespace Nom
{
	namespace Runtime
	{
		//enum class RTInstanceKind : unsigned char {Object = 0, Lambda = 1, OptLambda = 2, Record = 3, Array = 4, PartialApp = 5}; //Moved to RTDescriptorKind
		enum class RefValueHeaderFields : unsigned char {
			InterfaceTable = 0,
			RawInvoke = 1 //arrsize 0 - contains one pointer if value is directly invokable (for structural values, this is null until the corresponding monotonic cast happens and selects the relevant wrapper, if any)
							// fields must therefore be offset by one if there is a potential this-function; for classes, 
							// this means that only classes directly extending Object may specify that they are invokable, their subclasses can at most override this
		};
		class RefValueHeader
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::StructType* GetUninitializedLLVMType();
			static llvm::Value* GenerateReadVTablePointer(NomBuilder& builder, llvm::Value* refValue);

			//static unsigned int GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, RTValuePtr value, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives = false, llvm::BasicBlock** primitiveIntBlock = nullptr, llvm::Value** primitiveIntVar = nullptr, llvm::BasicBlock** primitiveFloatBlock = nullptr, llvm::Value** primitiveFloatVar = nullptr, llvm::BasicBlock** primitiveBoolBlock = nullptr, llvm::Value** primitiveBoolVar = nullptr, uint64_t refWeight = 100, uint64_t intWeight = 50, uint64_t floatWeight = 40, uint64_t boolWeight = 30);
			//static unsigned int GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, RTValuePtr value, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, uint64_t refWeight, uint64_t intWeight = 50, uint64_t floatWeight = 40, uint64_t boolWeight = 30);
			//static unsigned int GenerateNominalStructuralSwitch(NomBuilder& builder, RTValuePtr refValue, llvm::Value** vTableVar, llvm::BasicBlock** nominalObjectBlockVar, llvm::BasicBlock** structuralValueBlockVar);
			//static unsigned int GenerateStructuralValueKindSwitch(NomBuilder& builder, RTValuePtr refValue, llvm::Value** vtableVar, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** recordBlock, llvm::BasicBlock** partialAppBlock);
			//static unsigned int GenerateRefValueKindSwitch(NomBuilder& builder, RTValuePtr refValue, llvm::Value** vtableVar, llvm::BasicBlock** nominalObjectBlock, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** recordBlock, llvm::BasicBlock** partialAppBlock);

			//static void GenerateValueKindSwitch(NomBuilder& builder, RTValuePtr value, llvm::Value** vtableVar, llvm::BasicBlock** nominalObjectBlock, llvm::BasicBlock** structuralValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives, llvm::BasicBlock** primitiveIntBlock, llvm::Value** primitiveIntValVar, llvm::BasicBlock** primitiveFloatBlock, llvm::Value** primitiveFloatValVar, llvm::BasicBlock** primitiveBoolBlock, llvm::Value** primitiveBoolValVar);
			//static void GenerateValueKindSwitch(NomBuilder& builder, RTValuePtr value, llvm::Value** vtableVar, llvm::BasicBlock** nominalObjectBlock, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** recordBlock, llvm::BasicBlock** partialAppBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives = false, llvm::BasicBlock** primitiveIntBlock = nullptr, llvm::Value** primitiveIntValVar = nullptr, llvm::BasicBlock** primitiveFloatBlock = nullptr, llvm::Value** primitiveFloatValVar = nullptr, llvm::BasicBlock** primitiveBoolBlock = nullptr, llvm::Value** primitiveBoolValVar = nullptr);

			static llvm::Value* GenerateReadTypeTag(NomBuilder& builder, llvm::Value* refValue);
			static llvm::Value* GenerateGetReserveTypeArgsFromVTablePointer(NomBuilder& builder, llvm::Value* vtablePtr);
			static void GenerateWriteVTablePointer(NomBuilder& builder, llvm::Value* refValue, llvm::Value* vtableptr);
			static llvm::Value* GenerateWriteVTablePointerCMPXCHG(NomBuilder& builder, llvm::Value* refValue, llvm::Value* vtableptr, llvm::Value* vtableValue);
			static void GenerateInitializerCode(NomBuilder& builder, llvm::Value* valueHeader, llvm::Constant* vTablePointer, llvm::Constant* rawInvokePointer = nullptr);
			static llvm::Value* GetInterfaceMethodTableFunction(NomBuilder& builder, CompileEnv* env, RegIndex reg, llvm::Constant* index, size_t lineno);
			static llvm::Value* GenerateReadRawInvoke(NomBuilder& builder, llvm::Value* refValue);
			static void GenerateWriteRawInvoke(NomBuilder& builder, llvm::Value* refValue, llvm::Value* rawInvokePointer);
			static llvm::AtomicCmpXchgInst* GenerateWriteRawInvokeCMPXCHG(NomBuilder& builder, llvm::Value* refValue, llvm::Value* previousValueForCMPXCHG, llvm::Value* rawInvokePointer);
		};
		llvm::MDNode* GetNominalVsStructuralBranchWeights();
	}
}
