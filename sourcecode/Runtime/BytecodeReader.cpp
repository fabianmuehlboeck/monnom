#include "BytecodeReader.h"
#include "instructions/ArgumentInstruction.h"
#include "instructions/NoopInstruction.h"
#include "instructions/PhiNode.h"
#include "instructions/ReturnInstruction.h"
#include "instructions/LoadIntConstantInstruction.h"
#include "LoadFloatConstantInstruction.h"
#include "instructions/LoadStringConstantInstruction.h"
#include "instructions/LoadBoolConstantInstruction.h"
#include "instructions/CallCheckedInstanceMethod.h"
#include "instructions/CallCheckedStaticMethod.h"
#include "instructions/CallFinalInstanceMethod.h"
#include "instructions/DebugInstruction.h"
#include "instructions/ReturnVoidInstruction.h"
#include "instructions/BinOpInstruction.h"
#include "instructions/CastInstruction.h"
#include "instructions/ReadFieldInstruction.h"
#include "instructions/WriteFieldInstruction.h"
#include "instructions/CallConstructor.h"
#include "instructions/BranchInstruction.h"
#include "instructions/CondBranchInstruction.h"
#include "instructions/CallDispatchBestMethod.h"
#include "instructions/CreateClosure.h"
#include "instructions/ConstructStruct.h"
#include "instructions/ErrorInstruction.h"
#include "instructions/RTCmdInstruction.h"
#include "instructions/CallCFunction.h"
#include "LoadNullConstantInstruction.h"
#include "EnsureCheckedMethodInstruction.h"
#include "EnsureDynamicMethodInstruction.h"
#include <exception>
#include "NomLambda.h"
#include "RTConfig.h"
#include "NomRecordMethod.h"
#include <iostream>
#include "UnaryOpInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		using namespace std;
		BytecodeReader::BytecodeReader(CharStream& stream) :stream(stream)
		{
			uint64_t versionflag = this->stream.read<uint32_t>();
			if (versionflag != 2)
			{
				if (versionflag < 2)
				{
					std::cout << "The given bytecode file uses an older format that is not supported by this runtime!\n";
				}
				else
				{
					std::cout << "The given bytecode file uses an newer format that is not supported by this runtime!\n";
				}
				throw new std::exception(/*"Wrong Bytecode version!"*/);
			}
		}

		BytecodeReader::~BytecodeReader()
		{
		}

		NomInstruction* BytecodeReader::ReadInstruction([[maybe_unused]] BytecodeTopReadHandler* handler) {
			OpCode opcode = stream.read<OpCode>();
			switch (opcode) {
			case OpCode::Noop:
				return new NoopInstruction();
			case OpCode::Debug: {
				LocalConstantID msgConstant = stream.read<LocalConstantID>();
				return new DebugInstruction(context.GetGlobalID(msgConstant));
			}
			case OpCode::RTCmd: {
				LocalConstantID cmdConstant = stream.read<LocalConstantID>();
				return new RTCmdInstruction(context.GetGlobalID(cmdConstant));
			}
			case OpCode::Argument:
				return new ArgumentInstruction(stream.read<RegIndex>());
			case OpCode::Return:
				return new ReturnInstruction(stream.read<RegIndex>());
			case OpCode::ReturnVoid: {
				return new ReturnVoidInstruction();
			}
			case OpCode::EnsureCheckedMethod: {
				LocalConstantID methodName = stream.read<LocalConstantID>();
				RegIndex receiver = stream.read<RegIndex>();
				return new EnsureCheckedMethodInstruction(context.GetGlobalID(methodName), receiver);
			}
			case OpCode::EnsureDynamicMethod: {
				LocalConstantID methodName = stream.read<LocalConstantID>();
				RegIndex receiver = stream.read<RegIndex>();
				return new EnsureDynamicMethodInstruction(context.GetGlobalID(methodName), receiver);
			}
			case OpCode::LoadIntConstant: {
				uint64_t value = stream.read<uint64_t>();
				RegIndex reg = stream.read<RegIndex>();
				return new LoadIntConstantInstruction(value, reg);
			}
			case OpCode::LoadFloatConstant: {
				double value = stream.read<double>();
				RegIndex reg = stream.read<RegIndex>();
				return new LoadFloatConstantInstruction(value, reg);
			}
			case OpCode::LoadBoolConstant: {
				bool value = (bool)stream.read_char();
				RegIndex reg = stream.read<RegIndex>();
				return new LoadBoolConstantInstruction(reg, value); }
			case OpCode::LoadNullConstant: {
				RegIndex reg = stream.read<RegIndex>();
				return new LoadNullConstantInstruction(reg);
			}
			case OpCode::LoadStringConstant: {
				LocalConstantID strConstant = stream.read<LocalConstantID>();
				RegIndex reg = stream.read<RegIndex>();
				return new LoadStringConstantInstruction(reg, context.GetGlobalID(strConstant)); }
			case OpCode::InvokeCheckedInstance: {
				LocalConstantID method = stream.read<LocalConstantID>();
				LocalConstantID typeArgs = stream.read <LocalConstantID>();
				RegIndex reg = stream.read<RegIndex>();
				RegIndex receiver = stream.read<RegIndex>();
				return new CallCheckedInstanceMethod(reg, context.GetGlobalID(method), context.GetGlobalID(typeArgs), receiver); }
			case OpCode::CallFinal: {
				//LocalConstantID method = stream.read<LocalConstantID>();
				//LocalConstantID typeArgs = stream.read <LocalConstantID>();
				//RegIndex reg = stream.read<RegIndex>();
				//RegIndex receiver = stream.read<RegIndex>();
				//return new CallFinalInstanceMethod(reg, receiver, context.GetGlobalID(method)); }
				throw new std::exception(); }
			case OpCode::CallCheckedStatic: {
				LocalConstantID method = stream.read <LocalConstantID>();
				LocalConstantID typeArgs = stream.read <LocalConstantID>();
				RegIndex reg = stream.read<RegIndex>();
				return new CallCheckedStaticMethod(context.GetGlobalID(method), context.GetGlobalID(typeArgs), reg);
			}
			case OpCode::CallCFun: {
				RegIndex reg = stream.read<RegIndex>();
				LocalConstantID method = stream.read <LocalConstantID>();
				LocalConstantID typeArgs = stream.read <LocalConstantID>();
				return new CallCFunction(context.GetGlobalID(method), context.GetGlobalID(typeArgs), reg);
			}
			case OpCode::CallDispatchBest: {
				RegIndex reg = stream.read<RegIndex>();
				RegIndex receiver = stream.read<RegIndex>();
				LocalConstantID methodName = stream.read<LocalConstantID>();
				LocalConstantID typeArgs = stream.read<LocalConstantID>();
				return new CallDispatchBestMethod(reg, receiver, context.GetGlobalID(methodName), context.GetGlobalID(typeArgs));
			}
			case OpCode::BinOp: {
				BinaryOperation op = (BinaryOperation)stream.read<unsigned char>();
				RegIndex left = stream.read<RegIndex>();
				RegIndex right = stream.read<RegIndex>();
				RegIndex reg = stream.read<RegIndex>();
				return new BinOpInstruction(op, left, right, reg);
			}
			case OpCode::UnaryOp: {
				UnaryOperation op = (UnaryOperation)stream.read<unsigned char>();
				RegIndex arg = stream.read<RegIndex>();
				RegIndex reg = stream.read<RegIndex>();
				return new UnaryOpInstruction(op, arg, reg);
			}
			case OpCode::CallConstructor: {
				RegIndex reg = stream.read<RegIndex>();
				LocalConstantID cls = stream.read <LocalConstantID>();
				LocalConstantID typeArgs = stream.read <LocalConstantID>();
				return new CallConstructor(reg, context.GetGlobalID(cls), context.GetGlobalID(typeArgs));
			}
			case OpCode::CreateClosure: {
				LocalConstantID lambda = stream.read <LocalConstantID>();
				LocalConstantID typeArgs = stream.read <LocalConstantID>();
				RegIndex reg = stream.read<RegIndex>();
				return new CreateClosure(reg, context.GetGlobalID(lambda), context.GetGlobalID(typeArgs));
			}
			case OpCode::ConstructStruct: {
				LocalConstantID structure = stream.read <LocalConstantID>();
				LocalConstantID typeArgs = stream.read <LocalConstantID>();
				RegIndex reg = stream.read<RegIndex>();
				return new ConstructStructInstruction(reg, context.GetGlobalID(structure), context.GetGlobalID(typeArgs));
			}
			case OpCode::Cast: {
				RegIndex reg = stream.read<RegIndex>();
				RegIndex value = stream.read<RegIndex>();
				LocalConstantID type = stream.read<LocalConstantID>();
				return new CastInstruction(reg, value, context.GetGlobalID(type));
			}
			case OpCode::WriteField: {
				RegIndex receiver = stream.read<RegIndex>();
				RegIndex value = stream.read<RegIndex>();
				LocalConstantID fieldName = stream.read<LocalConstantID>();
				LocalConstantID receiverType = stream.read<LocalConstantID>();
				return new WriteFieldInstruction(receiver, value, context.GetGlobalID(fieldName), context.GetGlobalID(receiverType));
			}
			case OpCode::ReadField: {
				RegIndex reg = stream.read<RegIndex>();
				RegIndex receiver = stream.read<RegIndex>();
				LocalConstantID fieldName = stream.read<LocalConstantID>();
				LocalConstantID receiverType = stream.read<LocalConstantID>();
				return new ReadFieldInstruction(reg, receiver, context.GetGlobalID(fieldName), context.GetGlobalID(receiverType));
			}
			case OpCode::PhiNode: {
				int incoming = stream.read<int>();
				PhiNode* pn = new PhiNode(incoming);
				RegIndex regcount = stream.read<RegIndex>();
				while (regcount > 0)
				{
					RegIndex reg = stream.read<RegIndex>();
					LocalConstantID typeID = stream.read<LocalConstantID>();
					pn->AddRegisterEntry(reg, context.GetGlobalID(typeID));
					regcount--;
				}
				return pn;
			}
			case OpCode::Branch: {
				int target = stream.read<int>();
				BranchInstruction* bi = new BranchInstruction(target);
				RegIndex incount = stream.read<RegIndex>();
				while (incount > 0)
				{
					RegIndex to = stream.read<RegIndex>();
					RegIndex from = stream.read<RegIndex>();
					bi->AddIncoming(to, from);
					incount--;
				}
				return bi;
			}
			case OpCode::CondBranch: {
				RegIndex condition = stream.read<RegIndex>();
				int thenTarget = stream.read<int>();
				int elseTarget = stream.read<int>();
				CondBranchInstruction* cbi = new CondBranchInstruction(condition, thenTarget, elseTarget);
				RegIndex thencount = stream.read<RegIndex>();
				RegIndex elsecount = stream.read<RegIndex>();
				while (thencount > 0)
				{
					RegIndex to = stream.read<RegIndex>();
					RegIndex from = stream.read<RegIndex>();
					cbi->AddThenIncoming(to, from);
					thencount--;
				}
				while (elsecount > 0)
				{
					RegIndex to = stream.read<RegIndex>();
					RegIndex from = stream.read<RegIndex>();
					cbi->AddElseIncoming(to, from);
					elsecount--;
				}
				return cbi;
			}
			case OpCode::Error: {
				RegIndex reg = stream.read<RegIndex>();
				return new ErrorInstruction(reg);
			}
			default: {
				throw "Unknown opcode";
			}
			}
		}

		bool BytecodeReader::readNext(NomModule* mod, BytecodeTopReadHandler* handler) {
			auto nextType = GetNextElementType();
			switch (nextType) {
			case BytecodeTopElementType::None:
				return false;
			case BytecodeTopElementType::Class:
				ReadClass(mod, handler);
				break;
			case BytecodeTopElementType::Interface:
				ReadInterface(mod, handler);
				break;
			case BytecodeTopElementType::StringConstant:
				ReadStringConstant();
				break;
			case BytecodeTopElementType::ClassConstant:
				ReadClassConstant();
				break;
			case BytecodeTopElementType::LambdaConstant:
				ReadLambdaConstant();
				break;
			case BytecodeTopElementType::CTRecord:
				ReadStructConstant();
				break;
			case BytecodeTopElementType::ClassTypeConstant:
				ReadClassTypeConstant();
				break;
			case BytecodeTopElementType::MethodConstant:
				ReadMethodConstant();
				break;
			case BytecodeTopElementType::StaticMethodConstant:
				ReadStaticMethodConstant();
				break;
			case BytecodeTopElementType::TypeListConstant:
				ReadTypeListConstant();
				break;
			case BytecodeTopElementType::SuperClass:
				ReadSuperClassConstant();
				break;
			case BytecodeTopElementType::SuperInterfaces:
				ReadSuperInterfacesConstant();
				break;
			case BytecodeTopElementType::CTTypeParameters:
				ReadTypeParametersConstant();
				break;
			case BytecodeTopElementType::TypeVarConstant:
				ReadTypeVarConstant();
				break;
			case BytecodeTopElementType::CTSuperInterfaceList:
				ReadSuperInterfacesConstant();
				break;
			case BytecodeTopElementType::InterfaceConstant:
				ReadInterfaceConstant();
				break;
			case BytecodeTopElementType::CTIntersection:
				throw new std::exception();
				break;
			case BytecodeTopElementType::CTBottom:
				ReadBottomTypeConstant();
				break;
			case BytecodeTopElementType::CTDynamic:
				ReadDynamicTypeConstant();
				break;
			case BytecodeTopElementType::CTMaybe:
				ReadMaybeTypeConstant();
				break;
			case BytecodeTopElementType::CFunctionConstant:
				ReadCFunctionConstant(mod, handler);
				break;
			default:
				auto str = std::string("Invalid byte: ");
				unsigned char c = (unsigned char)stream.read_char();
				str.push_back(c);
				cout << str << "\n";
				throw str;
			}
			return true;
		}

		void BytecodeReader::readToEnd(NomModule* mod, BytecodeTopReadHandler* handler) {
			bool cont = true;
			while (cont) {
				cont = readNext(mod, handler);
			}
		}

		NomClass* BytecodeReader::ReadClass(NomModule* mod, BytecodeTopReadHandler* handler) {
			if (GetNextElementType() != BytecodeTopElementType::Class) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			ConstantID name = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID typeArgs = context.GetGlobalID(stream.read<LocalConstantID>());
			unsigned char visibility = stream.read<unsigned char>();
			unsigned char flags = stream.read<unsigned char>();
			ConstantID superInterfaces = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID superClass = context.GetGlobalID(stream.read<LocalConstantID>());
			NomClassLoaded* cls = mod->AddClass(name, typeArgs, superClass, superInterfaces);
			uint64_t methodCount = stream.read<uint64_t>();
			while (methodCount > 0) {
				ReadMethod(cls, handler);
				methodCount--;
			}
			uint64_t fieldCount = stream.read<uint64_t>();
			while (fieldCount > 0) {
				ReadField(cls, handler);
				fieldCount--;
			}
			uint64_t staticMethodCount = stream.read<uint64_t>();
			while (staticMethodCount > 0) {
				ReadStaticMethod(cls, handler);
				staticMethodCount--;
			}
			uint64_t constructorCount = stream.read<uint64_t>();
			while (constructorCount > 0) {
				ReadConstructor(cls, handler);
				constructorCount--;
			}
			uint64_t lambdaCount = stream.read<uint64_t>();
			while (lambdaCount > 0) {
				ReadLambda(cls, handler);
				lambdaCount--;
			}
			uint64_t structCount = stream.read<uint64_t>();
			while (structCount > 0) {
				ReadStruct(cls, handler);
				structCount--;
			}
			if (handler != nullptr)
			{
				handler->ReadClass(cls);
			}
			return cls;
		}

		NomInterface* BytecodeReader::ReadInterface(NomModule* mod, BytecodeTopReadHandler* handler) {
			if (GetNextElementType() != BytecodeTopElementType::Interface) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			ConstantID name = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID typeParameters = context.GetGlobalID(stream.read<LocalConstantID>());
			unsigned char visibility = stream.read<unsigned char>();
			unsigned char flags = stream.read<unsigned char>();
			ConstantID superInterfaces = context.GetGlobalID(stream.read<LocalConstantID>());
			NomInterfaceLoaded* iface = mod->AddInterface(name, typeParameters, superInterfaces);
			uint64_t methodCount = stream.read<uint64_t>();
			while (methodCount > 0) {
				ReadMethod(iface, handler);
				methodCount--;
			}
			if (handler != nullptr)
			{
				handler->ReadInterface(iface);
			}
			return iface;
		}

		NomLambda* BytecodeReader::ReadLambda(NomClassLoaded* cls, [[maybe_unused]] BytecodeTopReadHandler* handler) {
			if (stream.read_char() != (unsigned char)BytecodeInternalElementType::Lambda) {
				throw "Expected lambda, but did not encounter lambda marker";
			}
			ConstantID lambdaID = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID closureTypeParameters = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID closureArguments = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID typeParameters = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID argTypes = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID returnType = context.GetGlobalID(stream.read<LocalConstantID>());
			RegIndex regcount = stream.read<RegIndex>();
			NomLambda* lambda = cls->AddLambda(lambdaID, regcount, closureTypeParameters, closureArguments, typeParameters, argTypes, returnType);
			size_t fieldCount = stream.read<size_t>();
			while (fieldCount > 0)
			{
				ConstantID fieldName = context.GetGlobalID(stream.read<LocalConstantID>());
				ConstantID fieldType = context.GetGlobalID(stream.read<LocalConstantID>());
				lambda->AddField(fieldName, fieldType);
				fieldCount--;
			}
			uint64_t instructionCount = stream.read<uint64_t>();
			while (instructionCount > 0) {
				auto instr = ReadInstruction(handler);
				if (NomVerbose)
				{
					instr->Print(true);
				}
				lambda->Body.AddInstruction(instr);
				instructionCount--;
			}
			return lambda;
		}

		NomRecord* BytecodeReader::ReadStruct(NomClassLoaded* cls, [[maybe_unused]] BytecodeTopReadHandler* handler) {
			if (stream.read_char() != (unsigned char)BytecodeInternalElementType::Record) {
				throw "Expected struct, but did not encounter struct marker";
			}
			ConstantID structID = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID closureTypeParameters = context.GetGlobalID(stream.read<LocalConstantID>());
			RegIndex regcount = stream.read<RegIndex>();
			RegIndex endargregcount = stream.read<RegIndex>();
			LocalConstantID typeListID = stream.read<LocalConstantID>();
			ConstantID initializerArgTypes = context.GetGlobalID(typeListID);
			NomRecord* structure = cls->AddStruct(structID, closureTypeParameters, regcount, endargregcount, initializerArgTypes);
			size_t fieldCount = stream.read<size_t>();
			while (fieldCount > 0)
			{
				ConstantID fieldName = context.GetGlobalID(stream.read<LocalConstantID>());
				ConstantID fieldType = context.GetGlobalID(stream.read<LocalConstantID>());
				bool isReadOnly = stream.read_char() != 0;
				RegIndex valueRegister = stream.read<RegIndex>();
				int32_t instructionCount = stream.read<int32_t>();
				while (instructionCount > 0)
				{
					auto instr = ReadInstruction(handler);
					if (NomVerbose)
					{
						instr->Print(true);
					}
					structure->AddInstruction(instr);
					instructionCount--;
				}
				structure->AddField(fieldName, fieldType, isReadOnly, valueRegister);
				fieldCount--;
			}
			uint64_t methodCount = stream.read<uint64_t>();
			while (methodCount > 0) {
				ReadStructMethod(structure, handler);
				methodCount--;
			}
			return structure;
		}

		NomRecordMethod* BytecodeReader::ReadStructMethod(NomRecord* structure, [[maybe_unused]] BytecodeTopReadHandler* handler) {
			if (stream.read_char() != (unsigned char)BytecodeInternalElementType::Method) {
				throw "Expected method, but did not encounter method marker";
			}
			ConstantID name = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID typeParameters = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID returnType = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID argTypes = context.GetGlobalID(stream.read<LocalConstantID>());
			std::string namestr = NomConstants::GetString(name)->GetText()->ToStdString();
			bool isFinal = (bool)stream.read<char>();
			RegIndex regcount = stream.read<RegIndex>();
			std::string qname = "STRUCT_" + to_string(structure->StructID) + "." + namestr;
			NomRecordMethod* meth = structure->AddMethod(namestr, qname, typeParameters, returnType, argTypes, regcount);
			uint64_t instructionCount = stream.read<uint64_t>();
			while (instructionCount > 0) {
				auto instr = ReadInstruction(handler);
				if (NomVerbose)
				{
					instr->Print(true);
				}
				meth->AddInstruction(instr);
				instructionCount--;
			}
			return meth;
		}

		NomMethod* BytecodeReader::ReadMethod(NomInterfaceLoaded* iface, [[maybe_unused]] BytecodeTopReadHandler* handler) {
			if (stream.read_char() != (unsigned char)BytecodeInternalElementType::Method) {
				throw "Expected method, but did not encounter method marker";
			}
			ConstantID name = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID typeParameters = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID returnType = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID argTypes = context.GetGlobalID(stream.read<LocalConstantID>());
			std::string namestr = NomConstants::GetString(name)->GetText()->ToStdString();
			bool isFinal = (bool)stream.read<char>();
			RegIndex regcount = stream.read<RegIndex>();
			NomMethodLoaded* meth = iface->AddMethod(namestr, iface->GetName()->ToStdString() + "." + namestr, typeParameters, returnType, argTypes, regcount, isFinal);
			
			uint64_t instructionCount = stream.read<uint64_t>();
			while (instructionCount > 0) {
				auto instr = ReadInstruction(handler);
				if (NomVerbose)
				{
					instr->Print(true);
				}
				meth->AddInstruction(instr);
				instructionCount--;
			}
			return meth;
		}

		NomStaticMethod* BytecodeReader::ReadStaticMethod(NomClassLoaded* cls, [[maybe_unused]] BytecodeTopReadHandler* handler) {
			if (stream.read_char() != (unsigned char)BytecodeInternalElementType::StaticMethod) {
				throw "Expected static method, but did not encounter static method marker";
			}
			ConstantID name = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID typeargs = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID returnType = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID argTypes = context.GetGlobalID(stream.read<LocalConstantID>());
			std::string namestr = NomConstants::GetString(name)->GetText()->ToStdString();
			RegIndex regcount = stream.read<RegIndex>();
			std::string qnamestr = cls->GetName()->ToStdString() + "." + namestr;

			//NomSubstitutionContextMemberContext nscmc("What Context Here?");
			//auto argTypes = NomConstants::GetTypeList(argTypes)->GetTypeList(&nscmc);


			NomStaticMethodLoaded* meth = cls->AddStaticMethod(namestr, qnamestr, typeargs, returnType, argTypes, regcount);
			ArrayRef<NomTypeParameterRef> ref = meth->GetAllTypeParameters();
			//TypeList t = meth->get;
			uint64_t instructionCount = stream.read<uint64_t>();
			while (instructionCount > 0) {
				auto instr = ReadInstruction(handler);
				if (NomVerbose)
				{
					instr->Print(true);
				}
				meth->AddInstruction(instr);
				instructionCount--;
			}
			return meth;
		}

		NomConstructor* BytecodeReader::ReadConstructor(NomClassLoaded* cls, [[maybe_unused]] BytecodeTopReadHandler* handler) {
			if (stream.read_char() != (unsigned char)BytecodeInternalElementType::Constructor) {
				throw "Expected constructor, but did not encounter constructor marker";
			}
			ConstantID argTypes = context.GetGlobalID(stream.read<LocalConstantID>());
			RegIndex regcount = stream.read<RegIndex>();
			NomConstructorLoaded* cons = cls->AddConstructor(argTypes, regcount);
			uint64_t preinstructionCount = stream.read<uint64_t>();
			uint64_t superargcount = stream.read<uint64_t>();
			uint64_t instructionCount = stream.read<uint64_t>();
			while (preinstructionCount > 0) {
				auto instr = ReadInstruction(handler);
				if (NomVerbose)
				{
					instr->Print(true);
				}
				cons->AddInstruction(instr);
				preinstructionCount--;
			}
			while (superargcount > 0) {
				cons->AddSuperConstructorArgReg(stream.read<RegIndex>());
				superargcount--;
			}
			while (instructionCount > 0) {
				auto instr = ReadInstruction(handler);
				if (NomVerbose)
				{
					instr->Print(true);
				}
				cons->AddInstruction(instr);
				instructionCount--;
			}
			return cons;
		}

		NomTypedField* BytecodeReader::ReadField(NomClassLoaded* cls, [[maybe_unused]] BytecodeTopReadHandler* handler) {
			if (stream.read_char() != (unsigned char)BytecodeInternalElementType::Field) {
				throw "Expected constructor, but did not encounter constructor marker";
			}
			ConstantID name = context.GetGlobalID(stream.read<LocalConstantID>());
			ConstantID type = context.GetGlobalID(stream.read<LocalConstantID>());
			Visibility visibility = stream.read<Visibility>();
			unsigned char flags = stream.read<unsigned char>();
			NomTypedField* field = cls->AddField(name, type, visibility, (flags & '\1') == '\1', (flags & '\2') == '\2');
			return field;
		}

		BytecodeTopElementType BytecodeReader::GetNextElementType(bool peek) {
			unsigned char peekval;
			if (!stream.try_peek(peekval))
			{
				return BytecodeTopElementType::None;
			}
			if (peek)
			{
				return (BytecodeTopElementType)peekval;
			}
			return (BytecodeTopElementType)(stream.read_char());
		}

		ConstantID BytecodeReader::ReadStringConstant() {
			if (GetNextElementType() != BytecodeTopElementType::StringConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			uint64_t length = stream.read<uint64_t>();
			return context.AddString(lcid, NomString(length, stream));
		}

		ConstantID BytecodeReader::ReadClassConstant() {
			if (GetNextElementType() != BytecodeTopElementType::ClassConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			LocalConstantID llid = stream.read<LocalConstantID>();
			LocalConstantID cnid = stream.read<LocalConstantID>();
			return context.AddClass(lcid, llid, cnid);
		}
		ConstantID BytecodeReader::ReadInterfaceConstant() {
			if (GetNextElementType() != BytecodeTopElementType::InterfaceConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			LocalConstantID llid = stream.read<LocalConstantID>();
			LocalConstantID cnid = stream.read<LocalConstantID>();
			return context.AddInterface(lcid, llid, cnid);
		}

		ConstantID BytecodeReader::ReadLambdaConstant() {
			if (GetNextElementType() != BytecodeTopElementType::LambdaConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID llid = stream.read<LocalConstantID>();
			return context.AddLambda(llid);
		}

		ConstantID BytecodeReader::ReadStructConstant() {
			if (GetNextElementType() != BytecodeTopElementType::CTRecord) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID llid = stream.read<LocalConstantID>();
			return context.AddStruct(llid);
		}

		ConstantID BytecodeReader::ReadClassTypeConstant() {
			if (GetNextElementType() != BytecodeTopElementType::ClassTypeConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			LocalConstantID cnid = stream.read<LocalConstantID>();
			LocalConstantID tlid = stream.read<LocalConstantID>();
			return context.AddClassType(lcid, cnid, tlid);
		}
		ConstantID BytecodeReader::ReadMaybeTypeConstant() {
			if (GetNextElementType() != BytecodeTopElementType::CTMaybe) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			LocalConstantID tlid = stream.read<LocalConstantID>();
			return context.AddMaybeType(lcid, tlid);
		}

		ConstantID BytecodeReader::ReadBottomTypeConstant() {
			if (GetNextElementType() != BytecodeTopElementType::CTBottom) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			return context.AddBottomType(lcid);
		}
		ConstantID BytecodeReader::ReadDynamicTypeConstant() {
			if (GetNextElementType() != BytecodeTopElementType::CTDynamic) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			return context.AddDynamicType(lcid);
		}

		ConstantID BytecodeReader::ReadTypeVarConstant() {
			if (GetNextElementType() != BytecodeTopElementType::TypeVarConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char();
			LocalConstantID lcid = stream.read<LocalConstantID>();
			return context.AddTypeVar(lcid, stream.read<int32_t>());
		}

		ConstantID BytecodeReader::ReadTypeParametersConstant() {
			if (GetNextElementType() != BytecodeTopElementType::CTTypeParameters) {
				throw "Wrong constant type!";
			}
			stream.read_char();
			LocalConstantID lcid = stream.read<LocalConstantID>();
			uint64_t paramcount = stream.read<uint64_t>();
			NomTypeParameterConstant** parameters = nullptr;
			if (paramcount > 0)
			{
				parameters = makealloca(NomTypeParameterConstant*, paramcount);
				for (uint64_t i = 0; i < paramcount; i++)
				{
					//ParameterVariance variance = stream.read<ParameterVariance>();
					ConstantID lowerBound = context.GetGlobalID(stream.read<LocalConstantID>());
					ConstantID upperBound = context.GetGlobalID(stream.read<LocalConstantID>());
					parameters[i] = new NomTypeParameterConstant(ParameterVariance::Invariant, lowerBound, upperBound);
				}
			}
			return context.AddTypeParameters(lcid, llvm::ArrayRef<NomTypeParameterConstant*>(parameters, paramcount));
		}

		ConstantID BytecodeReader::ReadSuperClassConstant() {
			if (GetNextElementType() != BytecodeTopElementType::SuperClass) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			LocalConstantID cnid = stream.read<LocalConstantID>();
			LocalConstantID tlid = stream.read<LocalConstantID>();
			return context.AddSuperClass(lcid, cnid, tlid);
		}

		ConstantID BytecodeReader::ReadSuperInterfacesConstant() {
			if (GetNextElementType() != BytecodeTopElementType::CTSuperInterfaceList) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			uint64_t count = stream.read<uint64_t>();
			llvm::SmallVector<std::tuple<LocalConstantID, LocalConstantID>, 4> args;
			while (count > 0)
			{
				count--;
				LocalConstantID cnid = stream.read<LocalConstantID>();
				LocalConstantID tlid = stream.read<LocalConstantID>();
				args.emplace_back(cnid, tlid);
			}
			return context.AddSuperInterfaces(lcid, args);

		}

		ConstantID BytecodeReader::ReadMethodConstant() {
			if (GetNextElementType() != BytecodeTopElementType::MethodConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			LocalConstantID cnid = stream.read<LocalConstantID>();
			LocalConstantID mnid = stream.read<LocalConstantID>();
			LocalConstantID taid = stream.read<LocalConstantID>();
			LocalConstantID atid = stream.read<LocalConstantID>();
			return context.AddMethod(lcid, cnid, mnid, taid, atid);
		}
		ConstantID BytecodeReader::ReadStaticMethodConstant() {
			if (GetNextElementType() != BytecodeTopElementType::StaticMethodConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			LocalConstantID cnid = stream.read<LocalConstantID>();
			LocalConstantID mnid = stream.read<LocalConstantID>();
			LocalConstantID taid = stream.read<LocalConstantID>();
			LocalConstantID atid = stream.read<LocalConstantID>();
			return context.AddStaticMethod(lcid, cnid, mnid, taid, atid);
		}
		ConstantID BytecodeReader::ReadTypeListConstant() {
			if (GetNextElementType() != BytecodeTopElementType::TypeListConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			uint32_t typecount = stream.read<uint32_t>();
			llvm::SmallVector<ConstantID, 8> types;
			for (uint32_t i = typecount; i > 0; i--) {
				types.push_back(stream.read<LocalConstantID>());
			}
			return context.AddTypeList(lcid, types);
		}
		ConstantID BytecodeReader::ReadCFunctionConstant(NomModule *mod, BytecodeTopReadHandler* handler) {
			if (GetNextElementType() != BytecodeTopElementType::CFunctionConstant) {
				throw "Wrong constant type!";
			}
			stream.read_char(); //advance past marker
			LocalConstantID lcid = stream.read<LocalConstantID>();
			LocalConstantID source = stream.read<LocalConstantID>();
			LocalConstantID name = stream.read<LocalConstantID>();
			LocalConstantID typeargs = stream.read<LocalConstantID>();
			LocalConstantID argTypes = stream.read<LocalConstantID>();
			LocalConstantID returnType = stream.read<LocalConstantID>();
			auto ret= context.AddCFunction(lcid, source, name, typeargs, argTypes, returnType);
			auto binname = NomConstants::GetString(context.GetGlobalID(source))->GetText()->ToStdString();
			auto binlist = handler->RequireBinary(binname);
			mod->AddBinaries(std::get<0>(binlist), std::get<1>(binlist));
			return ret;
		}
	}
}
