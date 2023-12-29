#include "PWTypeVar.h"
#include "CompileHelpers.h"
#include "XRTClassType.h"
#include "PWClassType.h"
#include "PWInterface.h"
#include "PWSubstStack.h"
#include "NomType.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWClassType::GetLLVMType()
		{
			return XRTClassType::GetLLVMType();
		}
		llvm::Type* PWClassType::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWInterface PWClassType::ReadClassDescriptorLink(NomBuilder& builder)
		{
			return MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTClassTypeFields::Class), "class");
		}
		PWTypeArr PWClassType::TypeArgumentsPointer(NomBuilder& builder)
		{
			return builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(RTClassTypeFields::TypeArgs), MakeInt32(0) }, "typeArgs");
		}
		PWBool PWClassType::IsEq(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst)
		{
			BasicBlock* headerBlock = BasicBlock::Create(builder->getContext(), "typeEqHead", builder.GetFunction());
			BasicBlock* origBlock = builder->GetInsertBlock();
			builder->CreateBr(headerBlock);
			PWPhi<PWType> typePHI = PWPhi<PWType>::CreatePtr(builder, 2, "otherPHI");
			typePHI->addIncoming(other, origBlock);
			auto otherkind = typePHI.ReadKind(builder);
			BasicBlock* classTypeBlock = BasicBlock::Create(builder->getContext(), "otherIsClassType", builder.GetFunction());
			BasicBlock* instanceTypeBlock = BasicBlock::Create(builder->getContext(), "otherIsInstanceType", builder.GetFunction());
			BasicBlock* varBlock = BasicBlock::Create(builder->getContext(), "otherIsTypeVar", builder.GetFunction());
			BasicBlock* falseBlock = BasicBlock::Create(builder->getContext(), "notEq", builder.GetFunction());
			auto kswtch=builder->CreateSwitch(otherkind, falseBlock);
			kswtch->addCase(MakeIntLike(otherkind, static_cast<uint64_t>(TypeKind::TKClass)), classTypeBlock);
			kswtch->addCase(MakeIntLike(otherkind, static_cast<uint64_t>(TypeKind::TKInstance)), instanceTypeBlock);
			kswtch->addCase(MakeIntLike(otherkind, static_cast<uint64_t>(TypeKind::TKVariable)), varBlock);
			kswtch->addCase(MakeIntLike(otherkind, static_cast<uint64_t>(TypeKind::TKClass)), classTypeBlock);
		}
	}
}
