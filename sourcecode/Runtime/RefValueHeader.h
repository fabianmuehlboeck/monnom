#pragma once
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "CompileEnv.h"
#include "NomBuilder.h"
#include "NomValue.h"

namespace Nom
{
	namespace Runtime
	{
		//enum class RTInstanceKind : unsigned char {Object = 0, Lambda = 1, OptLambda = 2, Struct = 3, Array = 4, PartialApp = 5}; //Moved to RTDescriptorKind
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

			static int GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, NomValue value, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives, llvm::BasicBlock** primitiveIntBlock, llvm::Value** primitiveIntVar,  llvm::BasicBlock** primitiveFloatBlock, llvm::Value** primitiveFloatVar, llvm::BasicBlock** primitiveBoolBlock, llvm::Value** primitiveBoolVar);
			static int GenerateVTableTagSwitch(NomBuilder& builder, NomValue refValue, llvm::Value** vtableVar, llvm::BasicBlock** realPointerBlock, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** structBlock, llvm::BasicBlock** partialAppBlock);

			static int GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, NomValue value, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** postMaskedInt, llvm::BasicBlock** negMaskedInt, llvm::BasicBlock** posZerofloatBlock, llvm::BasicBlock** negZerofloatBlock, llvm::BasicBlock** maksedfloatBlock, bool unpackPrimitives, llvm::BasicBlock** primitiveIntBlock, llvm::Value** primitiveIntVar, llvm::BasicBlock** primitiveFloatBlock, llvm::Value** primitiveFloatVar, llvm::BasicBlock** primitiveBoolBlock, llvm::Value** primitiveBoolVar);

			static llvm::Value* GenerateReadTypeTag(NomBuilder& builder, llvm::Value* refValue);
			static llvm::Value* GenerateGetReserveTypeArgsFromVTablePointer(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Value* GenerateWriteVTablePointer(NomBuilder& builder, llvm::Value* refValue, llvm::Value* vtableptr);
			static llvm::Value* GenerateWriteVTablePointerCMPXCHG(NomBuilder& builder, llvm::Value* refValue, llvm::Value* vtableptr, llvm::Value* vtableValue);
			static void GenerateInitializerCode(NomBuilder& builder, llvm::Value* valueHeader, llvm::Value* vTablePointer);
			static void GenerateInitializerCode(NomBuilder& builder, llvm::Value* valueHeader, llvm::Value* specialPointer, llvm::Value* tag);
			static llvm::Value* GetInterfaceMethodTableFunction(NomBuilder& builder, CompileEnv* env, RegIndex reg, llvm::Constant* index, int lineno);
			static llvm::Value* GenerateReadRawInvoke(NomBuilder& builder, llvm::Value* refValue);
			static void GenerateWriteRawInvoke(NomBuilder& builder, llvm::Value* refValue, llvm::Value* rawInvokePointer);
			static llvm::AtomicCmpXchgInst* GenerateWriteRawInvokeCMPXCHG(NomBuilder& builder, llvm::Value* refValue, llvm::Value* previousValueForCMPXCHG, llvm::Value* rawInvokePointer);
		};
	}
}