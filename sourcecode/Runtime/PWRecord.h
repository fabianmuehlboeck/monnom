#pragma once
#include "PWStructVal.h"
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{
		class PWStructDict;
		class PWType;
		class PWRecord : public PWStructVal
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWRecord(llvm::Value* _wrapped) : PWStructVal(_wrapped)
			{

			}
			llvm::Value* ReadField(NomBuilder& builder, PWInt32 fieldIndex);
			PWInt8 ReadWrittenTag(NomBuilder& builder, PWInt32 fieldIndex);
			void WriteField(NomBuilder& builder, PWInt32 fieldIndex, llvm::Value* value);
			void WriteWrittenTag(NomBuilder& builder, PWInt32 fieldIndex);
			PWStructDict ReadStructDict(NomBuilder& builder);
		};
	}
}
