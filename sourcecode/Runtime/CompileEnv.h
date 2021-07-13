#pragma once

#include "llvm/ADT/Twine.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/IR/IRBuilder.h"
#include "Defs.h"
#include "NomValue.h"
#include "NomTypeVarValue.h"
#include "NomTypeDecls.h"
#include <vector>
#include <cstddef>
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomMemberContext;
		class PhiNode;
		class NomLambdaBody;
		class NomMethod;
		class NomStaticMethod;
		class NomConstructor;
		class NomStruct;
		class NomStructMethod;
		class NomTypeParameter;
		using NomTypeParameterRef = const NomTypeParameter*;
		class CompileEnv
		{
		protected:
			CompileEnv(const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context);
			virtual ~CompileEnv() {}
		public:
			bool basicBlockTerminated = false;
			const llvm::Twine contextName;
			llvm::Module* Module;
			llvm::Function* Function;
			const NomMemberContext* Context;
			virtual NomValue& operator[] (const RegIndex index) = 0;

			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) = 0;

			virtual NomValue GetArgument(int i) = 0;

			virtual void PushArgument(NomValue arg) = 0;

			virtual void ClearArguments() = 0;

			virtual int GetArgCount() = 0;

			virtual PhiNode* GetPhiNode(int index) = 0;

			virtual size_t GetLocalTypeArgumentCount() = 0;
			virtual size_t GetEnvTypeArgumentCount() = 0;
			virtual llvm::Value* GetLocalTypeArgumentArray(NomBuilder& builder) = 0;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) = 0;
		};
		class ACompileEnv : public CompileEnv
		{
		protected:
			const RegIndex regcount;
			NomValue* registers;
			const std::vector<PhiNode*>* phiNodes;
			llvm::SmallVector<NomValue, 8u> Arguments;
			llvm::SmallVector<NomTypeVarValue, 8u> TypeArguments;
			llvm::Value* localTypeArgArray = nullptr;
			llvm::Value* envTypeArgArray = nullptr;
		public:
			virtual NomValue& operator[] (const RegIndex index) override
			{
				if (index < 0 || index >= regcount)
				{
					throw "Invalid Register index!";
				}
				return registers[index];
			}

			ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			virtual ~ACompileEnv();

			virtual NomValue GetArgument(int i) override;

			virtual void PushArgument(NomValue arg) override;

			virtual void ClearArguments() override;

			virtual int GetArgCount() override;

			virtual PhiNode* GetPhiNode(int index) override;

			virtual size_t GetLocalTypeArgumentCount() override;
			virtual llvm::Value* GetLocalTypeArgumentArray(NomBuilder& builder) override;
		};

		class InstanceMethodCompileEnv : public ACompileEnv
		{
		public:
			const NomMethod* const Method;
			InstanceMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomClassTypeRef thisType, const NomMethod* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;

			// Inherited via CompileEnv
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
		};

		class StaticMethodCompileEnv : public ACompileEnv
		{
		public:
			const NomStaticMethod* const Method;
			StaticMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomStaticMethod* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
		};

		class ConstructorCompileEnv : public ACompileEnv
		{
		public:
			const NomConstructor* const Method;
			ConstructorCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomClassTypeRef thisType, const NomConstructor* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
		};

		class LambdaCompileEnv : public ACompileEnv
		{
		public:
			const NomLambdaBody* const Lambda;
			LambdaCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomLambdaBody* lambda);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
		};


		class StructMethodCompileEnv : public ACompileEnv
		{
		public:
			const NomStructMethod* const Method;
			StructMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomStructMethod* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
		};

		class StructInstantiationCompileEnv : public ACompileEnv
		{
		public:
			const NomStruct* const Struct;
			StructInstantiationCompileEnv(RegIndex regcount, llvm::Function* function, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomStruct* structure, RegIndex endargregcount);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
		};


		class SimpleClassCompileEnv : public ACompileEnv
		{
		public:
			SimpleClassCompileEnv(llvm::Function* function, const NomMemberContext* context, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomTypeRef thisType);
			// Inherited via CompileEnv
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;

			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
		};

		class CastedValueCompileEnv : public CompileEnv
		{
		private:
			const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs;
			const llvm::ArrayRef<NomTypeParameterRef> instanceTypeArgs;
			llvm::Value* localTypeArr;
			llvm::Value* instanceTypeArr;
		public:
			CastedValueCompileEnv(const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<NomTypeParameterRef> instanceTypeArgs, llvm::Value* localTypeArr, llvm::Value* instanceTypeArr);

			// Inherited via CompileEnv
			virtual NomValue& operator[](const RegIndex index) override;

			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;

			virtual NomValue GetArgument(int i) override;

			virtual void PushArgument(NomValue arg) override;

			virtual void ClearArguments() override;

			virtual int GetArgCount() override;

			virtual PhiNode* GetPhiNode(int index) override;

			virtual size_t GetLocalTypeArgumentCount() override;

			virtual size_t GetEnvTypeArgumentCount() override;

			virtual llvm::Value* GetLocalTypeArgumentArray(NomBuilder& builder) override;

			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;



		};
	}
}