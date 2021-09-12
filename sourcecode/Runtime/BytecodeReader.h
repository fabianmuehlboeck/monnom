#pragma once
#include <inttypes.h>
#include <istream>
#include <array>
#include "NomConstants.h"
#include "CharStream.h"
#include "NomModule.h"
#include "NomInstruction.h"
#include "BytecodeTopReadHandler.h"

namespace Nom
{
	namespace Runtime
	{
		enum class BytecodeTopElementType : unsigned char
		{
			None = 0,
			Class = 1,
			Interface = 2,
			StringConstant = 103, 
			IntegerConstant = 104,
			FloatConstant = 105,
			CTRecord = 110,
			ClassConstant = 111,
			InterfaceConstant = 112,
			CTTypeParameter = 113,
			LambdaConstant = 114,
			CTIntersection = 115,
			CTUnion = 116,
			CTBottom = 117,
			CTDynamic = 118,
			CTMaybe = 119,
			ClassTypeConstant = 121,
			TypeVarConstant = 122,
			CTConstructor = 133,
			MethodConstant = 131,
			StaticMethodConstant = 132,
			SuperClass = 141,
			SuperInterfaces = 142,
			TypeListConstant = 191,
			CTSuperInterfaceList = 192,
			CTClassTypeList = 193,
			CTTypeParameters = 194
		};

		enum class BytecodeInternalElementType : unsigned char
		{
			Field = 201,
			StaticMethod = 202,
			Method = 203,
			Constructor = 204,
			Lambda = 205,
			Record = 206
		};

		class BytecodeReader
		{
		private:
			CharStream stream;
			NomConstantContext context;
		public:
			BytecodeReader(CharStream &stream);
			~BytecodeReader();

			bool readNext(NomModule *mod, BytecodeTopReadHandler *handler = nullptr);

			void readToEnd(NomModule * mod, BytecodeTopReadHandler *handler = nullptr);

			NomClass *ReadClass(NomModule * mod, BytecodeTopReadHandler *handler = nullptr);

			NomInterface *ReadInterface(NomModule * mod, BytecodeTopReadHandler *handler = nullptr);

			NomLambda* ReadLambda(NomClassLoaded* cls);

			NomRecord* ReadStruct(NomClassLoaded* cls);

			NomRecordMethod* ReadStructMethod(NomRecord* structure);

			NomMethod *ReadMethod(NomInterfaceLoaded *iface);

			NomStaticMethod *ReadStaticMethod(NomClassLoaded*cls);

			NomConstructor * ReadConstructor(NomClassLoaded* cls);

			NomTypedField * ReadField(NomClassLoaded* cls);

			NomInstruction *ReadInstruction();

			BytecodeTopElementType GetNextElementType(bool peek = true);

			ConstantID ReadStringConstant();

			ConstantID ReadClassConstant();

			ConstantID ReadInterfaceConstant();

			ConstantID ReadLambdaConstant();

			ConstantID ReadStructConstant();

			ConstantID ReadClassTypeConstant();

			ConstantID ReadMaybeTypeConstant();

			ConstantID ReadBottomTypeConstant();

			ConstantID ReadDynamicTypeConstant();

			ConstantID ReadSuperClassConstant();
			ConstantID ReadSuperInterfacesConstant();

			ConstantID ReadMethodConstant();
			ConstantID ReadStaticMethodConstant();
			ConstantID ReadTypeListConstant();

			ConstantID ReadTypeVarConstant();
			ConstantID ReadTypeParametersConstant();
		};
	}
}
