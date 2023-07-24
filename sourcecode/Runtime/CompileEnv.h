#pragma once

PUSHDIAGSUPPRESSION
#include "llvm/ADT/Twine.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/IR/IRBuilder.h"
POPDIAGSUPPRESSION
#include "Defs.h"
#include "NomValue.h"
#include "NomTypeVarValue.h"
#include "NomTypeDecls.h"
#include <vector>
#include <cstddef>
#include <stack>
#include "NomBuilder.h"
#include "PWTypeArr.h"

namespace Nom
{
	namespace Runtime
	{
		class PWDispatchPair;
		class NomMemberContext;
		class PhiNode;
		class NomLambdaBody;
		class NomMethod;
		class NomStaticMethod;
		class NomConstructor;
		class NomRecord;
		class NomRecordMethod;
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

			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) = 0;

			virtual NomValue GetArgument(size_t i) = 0;

			virtual void PushArgument(NomValue arg) = 0;

			virtual void PushDispatchPair(PWDispatchPair dpair) = 0;
			virtual PWDispatchPair PopDispatchPair() = 0;

			virtual void ClearArguments() = 0;

			virtual size_t GetArgCount() = 0;

			virtual PhiNode* GetPhiNode(size_t index) = 0;

			virtual bool GetInConstructor() = 0;

			virtual size_t GetLocalTypeArgumentCount() = 0;
			virtual size_t GetEnvTypeArgumentCount() = 0;
			virtual PWTypeArr GetLocalTypeArgumentArray(NomBuilder& builder) = 0;
			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) = 0;
		};
		class ACompileEnv : public CompileEnv
		{
		protected:
			const RegIndex regcount;
			NomValue* registers;
			const std::vector<PhiNode*>* phiNodes;
			llvm::SmallVector<NomValue, 8u> Arguments;
			llvm::SmallVector<NomTypeVarValue, 8u> TypeArguments;
			std::stack<llvm::Value*> dispatcherPairs;
			llvm::Value* localTypeArgArray = nullptr;
			llvm::Value* envTypeArgArray = nullptr;
		public:
			virtual NomValue& operator[] (const RegIndex index) override
			{
				if (index >= regcount)
				{
					throw "Invalid Register index!";
				}
				return registers[index];
			}

			ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<llvm::Value*> typeArgValues);

			ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, int argument_offset, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			virtual ~ACompileEnv() override;

			virtual NomValue GetArgument(size_t i) override;

			virtual void PushArgument(NomValue arg) override;
			virtual void PushDispatchPair(PWDispatchPair dpair) override;
			virtual PWDispatchPair PopDispatchPair() override;


			virtual void ClearArguments() override;

			virtual size_t GetArgCount() override;

			virtual PhiNode* GetPhiNode(size_t index) override;

			virtual size_t GetLocalTypeArgumentCount() override;
			virtual PWTypeArr GetLocalTypeArgumentArray(NomBuilder& builder) override;
		};
		/// <summary>
		/// An abstract compilation environment for functions with a full signatures
		/// </summary>
		class AFullArityCompileEnv : public ACompileEnv
		{
		public:

			AFullArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<llvm::Value*> typeArgValues);

			AFullArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, int argument_offset, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			AFullArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			virtual ~AFullArityCompileEnv() override;
		};
		/// <summary>
		/// An abstract compilation environment, but for "varargs" functions
		/// </summary>
		class AVariableArityCompileEnv : public ACompileEnv
		{
		public:
			virtual NomValue& operator[] (const RegIndex index) override;
			AVariableArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<llvm::Value*> typeArgValues);

			AVariableArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, int argument_offset, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			AVariableArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			virtual ~AVariableArityCompileEnv() override;
		};

		class InstanceMethodCompileEnv : public AFullArityCompileEnv
		{
		public:
			const NomMethod* const Method;
			InstanceMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomClassTypeRef thisType, const NomMethod* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override;

			// Inherited via CompileEnv
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};

		class StaticMethodCompileEnv : public AFullArityCompileEnv
		{
		public:
			const NomStaticMethod* const Method;
			StaticMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomStaticMethod* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};

		class ConstructorCompileEnv : public AFullArityCompileEnv
		{
		private:
			bool inConstructor = true;
		public:
			const NomConstructor* const Method;
			ConstructorCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomClassTypeRef thisType, const NomConstructor* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override;
			virtual void SetPastInitialSetup();
		};

		class LambdaCompileEnv : public AVariableArityCompileEnv
		{
		public:
			const NomLambdaBody* const Lambda;
			LambdaCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomLambdaBody* lambda);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};


		class StructMethodCompileEnv : public AFullArityCompileEnv
		{
		public:
			const NomRecordMethod* const Method;
			StructMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomRecordMethod* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};

		class StructInstantiationCompileEnv : public AFullArityCompileEnv
		{
		private:
			bool inConstructor = true;
		public:
			const NomRecord* const Record;
			StructInstantiationCompileEnv(RegIndex regcount, llvm::Function* function, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomRecord* structure, RegIndex endargregcount);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override;
			virtual void SetPastInitialSetup();
		};


		class SimpleClassCompileEnv : public ACompileEnv
		{
		public:
			SimpleClassCompileEnv(llvm::Function* function, const NomMemberContext* context, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomTypeRef thisType);
			// Inherited via CompileEnv
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override;
			virtual size_t GetEnvTypeArgumentCount() override;

			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};

		class CastedValueCompileEnv : public CompileEnv
		{
		protected:
			const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs;
			const llvm::ArrayRef<NomTypeParameterRef> instanceTypeArgs;
			llvm::Function* const function;
			const size_t regularArgsBegin;
			const size_t funValueArgCount;
			llvm::Value* instanceTypeArrPtr;
		public:
			CastedValueCompileEnv(const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<NomTypeParameterRef> instanceTypeArgs, llvm::Function* fun, size_t regular_args_begin, size_t funValueArgCount, llvm::Value* instanceTypeArrPtr);

			// Inherited via CompileEnv
			virtual NomValue& operator[](const RegIndex index) override;

			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override;

			virtual NomValue GetArgument(size_t i) override;

			virtual void PushArgument(NomValue arg) override;

			virtual void ClearArguments() override;

			virtual size_t GetArgCount() override;

			virtual PhiNode* GetPhiNode(size_t index) override;

			virtual size_t GetLocalTypeArgumentCount() override;

			virtual size_t GetEnvTypeArgumentCount() override;

			virtual PWTypeArr GetLocalTypeArgumentArray(NomBuilder& builder) override;

			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override;

			virtual bool GetInConstructor() override { return false; }

			// Inherited via CompileEnv
			virtual void PushDispatchPair(PWDispatchPair dpair) override;

			virtual PWDispatchPair PopDispatchPair() override;

		};

		class CastedValueCompileEnvIndirect : public CastedValueCompileEnv
		{
		public:
			CastedValueCompileEnvIndirect(const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<NomTypeParameterRef> instanceTypeArgs, llvm::Function* fun, size_t regular_args_begin, size_t funValueArgCount, llvm::Value* instanceTypeArrPtr);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override;
			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override;
		};
	}
}
