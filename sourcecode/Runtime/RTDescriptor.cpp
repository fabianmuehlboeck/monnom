#include "RTDescriptor.h"
#include "CompileHelpers.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		//llvm::StructType* GetDescriptorDictionaryEntryType()
		//{
		//	static llvm::StructType* cdet = StructType::create(LLVMCONTEXT,
		//		{
		//			numtype(DescriptorDictionaryEntryKind), //kind
		//			numtype(unsigned char), //flags (readonly...)
		//			numtype(Visibility), //visibility
		//			numtype(unsigned char), //reserved
		//			numtype(int32_t), //field/dispatcher index
		//			POINTERTYPE //type; or dispatcher method
		//		}, "NOM_RT_DescriptorDictionaryEntryType");
		//	return cdet;
		//}

		llvm::Constant* RTDescriptorDictionaryEntry::CreateConstant(RTDescriptorDictionaryEntryKind det, bool readonly, Visibility visibility, llvm::Constant* typeOrPartialAppOrDispatcher, int32_t index, bool rawInt, bool rawFloat)
		{
			return ConstantStruct::get(GetLLVMType(), MakeInt<RTDescriptorDictionaryEntryKind>(det), MakeInt<unsigned char>((readonly ? 1 : 0) + (rawInt? 2 : 0) + (rawFloat ? 4 : 0)), MakeInt<Visibility>(visibility), MakeInt<unsigned char>(0), MakeInt32(index), ConstantExpr::getPointerCast(typeOrPartialAppOrDispatcher, POINTERTYPE));
		}

		llvm::Value* RTDescriptorDictionaryEntry::GenerateReadKind(NomBuilder& builder, llvm::Value* entry)
		{
			return MakeLoad(builder, entry, GetLLVMPointerType(), MakeInt32(RTDescriptorDictionaryEntryFields::Kind), "dictEntryKind");
		}

		llvm::Value* RTDescriptorDictionaryEntry::GenerateReadFlags(NomBuilder& builder, llvm::Value* entry)
		{
			return MakeLoad(builder, entry, GetLLVMPointerType(), MakeInt32(RTDescriptorDictionaryEntryFields::Flags), "dictEntryFlags");
		}

		llvm::Value* RTDescriptorDictionaryEntry::GenerateReadVisibility(NomBuilder& builder, llvm::Value* entry)
		{
			return MakeLoad(builder, entry, GetLLVMPointerType(), MakeInt32(RTDescriptorDictionaryEntryFields::Visibility), "dictEntryVisibility");
		}

		llvm::Value* RTDescriptorDictionaryEntry::GenerateReadIndex(NomBuilder& builder, llvm::Value* entry)
		{
			return MakeLoad(builder, entry, GetLLVMPointerType(), MakeInt32(RTDescriptorDictionaryEntryFields::Index), "dictEntryIndex");
		}

		llvm::Value* RTDescriptorDictionaryEntry::GenerateReadTypeOrPartialAppOrDispatcher(NomBuilder& builder, llvm::Value* entry)
		{
			return MakeLoad(builder, entry, GetLLVMPointerType(), MakeInt32(RTDescriptorDictionaryEntryFields::TypeOrPartialAppOrDispatcher), "typeOrPartialAppOrDispatcher");
		}

		llvm::StructType* RTDescriptorDictionaryEntry::GetLLVMType()
		{
			static llvm::StructType* cdet = StructType::create(LLVMCONTEXT,
				{
					numtype(RTDescriptorDictionaryEntryKind), //kind
					numtype(unsigned char), //flags (readonly...)
					numtype(Visibility), //visibility
					numtype(unsigned char), //reserved
					numtype(int32_t), //field/dispatcher index
					POINTERTYPE //type; or dispatcher method
				}, "NOM_RT_DescriptorDictionaryEntryType");
			return cdet;
		}

	}
}