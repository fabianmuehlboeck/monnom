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
#include <stack>
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

			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) = 0;

			virtual NomValue GetArgument(int i) = 0;

			virtual void PushArgument(NomValue arg) = 0;

			virtual void PushDispatchPair(llvm::Value* dpair) = 0;
			virtual llvm::Value* PopDispatchPair() = 0;

			virtual void ClearArguments() = 0;

			virtual int GetArgCount() = 0;

			virtual PhiNode* GetPhiNode(int index) = 0;

			virtual bool GetInConstructor() = 0;

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
			std::stack<llvm::Value*> dispatcherPairs;
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

			ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<llvm::Value*> typeArgValues);

			ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, int argument_offset, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			virtual ~ACompileEnv() override;

			virtual NomValue GetArgument(int i) override;

			virtual void PushArgument(NomValue arg) override;
			virtual void PushDispatchPair(llvm::Value* dpair) override;
			virtual llvm::Value* PopDispatchPair() override;


			virtual void ClearArguments() override;

			virtual int GetArgCount() override;

			virtual PhiNode* GetPhiNode(int index) override;

			virtual size_t GetLocalTypeArgumentCount() override;
			virtual llvm::Value* GetLocalTypeArgumentArray(NomBuilder& builder) override;
		};
		/// <summary>
		/// An abstract compilation environment for functions with a full signatures
		/// </summary>
		class AFullArityCompileEnv : public ACompileEnv
		{
		public:

			AFullArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<llvm::Value*> typeArgValues);

			AFullArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, int argument_offset, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType);

			AFullArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, int argument_offset, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType, NomBuilder& builder);

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
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;

			// Inherited via CompileEnv
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};

		class StaticMethodCompileEnv : public AFullArityCompileEnv
		{
		public:
			const NomStaticMethod* const Method;
			StaticMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomStaticMethod* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};
		//Create a new subclass of AFullArityCompileEnv, 

		class CLibStaticCompileEnv : public AFullArityCompileEnv
		{
		public:
			const NomStaticMethod* const Method;
			CLibStaticCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomBuilder& builder);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};
		class CLibInstanceCompileEnv : public AFullArityCompileEnv
		{
		public:
			const NomStaticMethod* const Method;
			CLibInstanceCompileEnv::CLibInstanceCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, 
				const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, 
				const TypeList argtypes, NomClassTypeRef thisType, NomBuilder& builder);
			llvm::SmallVector<NomTypeVarValue, 8u> ClassTypeArguments;
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};


		class ConstructorCompileEnv : public AFullArityCompileEnv
		{
		private:
			bool inConstructor = true;
		public:
			const NomConstructor* const Method;
			ConstructorCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomClassTypeRef thisType, const NomConstructor* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override;
			virtual void SetPastInitialSetup();
		};

		class LambdaCompileEnv : public AVariableArityCompileEnv
		{
		public:
			const NomLambdaBody* const Lambda;
			LambdaCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomLambdaBody* lambda);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};


		class StructMethodCompileEnv : public AFullArityCompileEnv
		{
		public:
			const NomRecordMethod* const Method;
			StructMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomRecordMethod* method);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};

		class StructInstantiationCompileEnv : public AFullArityCompileEnv
		{
		private:
			bool inConstructor = true;
		public:
			const NomRecord* const Record;
			StructInstantiationCompileEnv(RegIndex regcount, llvm::Function* function, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomRecord* structure, RegIndex endargregcount);
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override;
			virtual void SetPastInitialSetup();
		};


		class SimpleClassCompileEnv : public ACompileEnv
		{
		public:
			SimpleClassCompileEnv(llvm::Function* function, const NomMemberContext* context, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomTypeRef thisType);
			// Inherited via CompileEnv
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override;
			virtual size_t GetEnvTypeArgumentCount() override;

			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override;
			virtual bool GetInConstructor() override { return false; }
		};

		class CastedValueCompileEnv : public CompileEnv
		{
		private:
			const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs;
			const llvm::ArrayRef<NomTypeParameterRef> instanceTypeArgs;
			llvm::Function* const function;
			const int regularArgsBegin;
			const int funValueArgCount;
			llvm::Value* const instanceTypeArrPtr;
		public:
			CastedValueCompileEnv(const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<NomTypeParameterRef> instanceTypeArgs, llvm::Function* fun, int regular_args_begin, int funValueArgCount, llvm::Value* instanceTypeArrPtr);

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

			virtual bool GetInConstructor() override { return false; }

			// Inherited via CompileEnv
			virtual void PushDispatchPair(llvm::Value* dpair) override;

			virtual llvm::Value* PopDispatchPair() override;

		};
	}
}