#include "NomRecordCallTag.h"
#include <unordered_map>
#include "NomString.h"
#include "IMT.h"
#include "CallingConvConf.h"
#include "RTCompileConfig.h"
#include "NomLambdaCallTag.h"
#include "RTVTable.h"
#include "RefValueHeader.h"
#include "NomNameRepository.h"
#include "CompileHelpers.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		struct RCTTupleHash
		{
			std::size_t operator()(std::tuple<const std::string*, int, int>& s) const noexcept
			{
				return std::hash<std::string>()(*std::get<0>(s)) + (std::get<1>(s) * 93739 + std::get<2>(s)) * 45569;
			}
			std::size_t operator()(const std::tuple<const std::string*, int, int>& s) const noexcept
			{
				return std::hash<std::string>()(*std::get<0>(s)) + (std::get<1>(s) * 93739 + std::get<2>(s)) * 45569;
			}
		};
		struct RCTTupleEq
		{
			bool operator()(std::tuple<const std::string*, int, int>& lhs, std::tuple<const std::string*, int, int>& rhs) const noexcept
			{
				return std::equal_to<std::string>()(*std::get<0>(lhs), *std::get<0>(rhs)) && std::get<1>(lhs) == std::get<1>(rhs) && std::get<2>(lhs) == std::get<2>(rhs);
			}
			bool operator()(const std::tuple<const std::string*, int, int>& lhs, std::tuple<const std::string*, int, int>& rhs) const noexcept
			{
				return std::equal_to<std::string>()(*std::get<0>(lhs), *std::get<0>(rhs)) && std::get<1>(lhs) == std::get<1>(rhs) && std::get<2>(lhs) == std::get<2>(rhs);
			}
		};
		NomRecordCallTag::NomRecordCallTag(const std::string &name, int typeargcount, int argcount) : name(name), typeargcount(typeargcount), argcount(argcount)
		{
		}
		const NomRecordCallTag* NomRecordCallTag::GetCallTag(const std::string& name, int typeargcount, int argcount)
		{
			static unordered_map<std::string, NomRecordCallTag*> callTags;
			std::string key = name + "/" + std::to_string(typeargcount) + "/" + std::to_string(argcount);
			auto findResult = callTags.find(key);
			if (findResult == callTags.end())
			{
				auto newrct = new NomRecordCallTag(name, typeargcount, argcount);
				callTags[key] = newrct;
			}
			return callTags[key];
		}
		NomRecordCallTag::~NomRecordCallTag()
		{
		}
		llvm::Constant* NomRecordCallTag::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = mod.getFunction("MONNOM_RT_RCT_" + name);
			if (fun == nullptr)
			{
				fun = Function::Create(GetIMTFunctionType(), linkage, "MONNOM_RT_RCT_" + name, mod);
				fun->setCallingConv(NOMCC);

				NomBuilder builder;

				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
				builder->SetInsertPoint(startBlock);

				auto argiter = fun->arg_begin();
				auto callTag = argiter;
				argiter++;
				auto receiver = argiter;
				argiter++;

				auto argarr = makealloca(Value*, 2 + RTConfig_NumberOfVarargsArguments);
				argarr[0] = NomLambdaCallTag::GetCallTag(typeargcount, argcount)->GetLLVMElement(mod);
				argarr[1] = receiver;
				for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
				{
					argarr[i + 2] = argiter;
					argiter++;
				}
				auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				auto dpair = RTVTable::GenerateFindDynamicDispatcherPair(builder, builder->CreatePointerCast(receiver, REFTYPE), vtable, NomNameRepository::Instance().GetNameID(name));

				auto target = builder->CreateExtractValue(dpair, { 0 });
				argarr[1] = builder->CreatePointerCast(builder->CreateExtractValue(dpair, { 1 }), POINTERTYPE);

				auto callResult = builder->CreateCall(GetIMTFunctionType(), target, ArrayRef<Value*>(argarr, 2 + RTConfig_NumberOfVarargsArguments));
				callResult->setCallingConv(NOMCC);
				if (NOMCC == CallingConv::Fast)
				{
					callResult->setTailCallKind(CallInst::TailCallKind::TCK_MustTail);
				}
				builder->CreateRet(callResult);

				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify Record Call Tag function ";
					std::cout << name << "/" << typeargcount << "/" << argcount;
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			return fun;
		}
		llvm::Constant* NomRecordCallTag::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("MONNOM_RT_RCT_" + name);
		}
	}
}