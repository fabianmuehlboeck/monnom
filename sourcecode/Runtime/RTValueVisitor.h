#pragma once
#include "RTValuePtr.h"
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{
		class PWFloat;
		class PWRefValue;
		class PWPacked;
		class PWVMPtr;
		class PWObject;
		class PWLambda;
		class PWPartialApp;
		class PWRecord;
		class PWStructVal;
		class RTValueVisitor
		{
		public:
			std::function<void(RTValuePtr)> VisitDefault = []([[maybe_unused]]RTValuePtr o) -> void { throw new std::exception(); };
			std::function<void(RTPWValuePtr<PWObject>)> VisitObject = [this](RTPWValuePtr<PWObject> o) -> void { VisitDefault(o); };
			std::function<void(RTPWValuePtr<PWPacked>)> VisitPackedValue = [this](RTPWValuePtr<PWPacked> o) -> void { VisitDefault(o); };
			std::function<void(RTPWValuePtr<PWVMPtr>)> VisitVMPtr = [this](RTPWValuePtr<PWVMPtr> o) -> void { VisitDefault(o); };
			std::function<void(RTPWValuePtr<PWLambda>)> VisitLambda = [this](RTPWValuePtr<PWLambda> o) -> void { VisitDefault(o); };
			std::function<void(RTPWValuePtr<PWRecord>)> VisitRecord = [this](RTPWValuePtr<PWRecord> o) -> void { VisitDefault(o); };
			std::function<void(RTPWValuePtr<PWRefValue>)> VisitRefValue = [this](RTPWValuePtr<PWRefValue> o) -> void { VisitDefault(o); };
			std::function<void(RTPWValuePtr<PWStructVal>)> VisitStructValue = [this](RTPWValuePtr<PWStructVal> o) -> void { VisitDefault(o); };
			std::function<void(RTPWValuePtr<PWInt64>)> VisitRawInt = [this](RTPWValuePtr<PWInt64> o) -> void { VisitDefault(o); };
			std::function<void(RTPWValuePtr<PWFloat>)> VisitRawFloat = [this](RTPWValuePtr<PWFloat> o) -> void { VisitDefault(o); };
			std::function<void(RTPWValuePtr<PWBool>)> VisitRawBool = [this](RTPWValuePtr<PWBool> o) -> void { VisitDefault(o); };

			RTValueVisitor() {}
			RTValueVisitor(std::function<void(RTValuePtr)> _defaultVisitor) : VisitDefault(_defaultVisitor) {}
		};
	}
}
