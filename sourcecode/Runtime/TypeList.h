#pragma once

#include <vector>
#include "NomType.h"
#include "llvm/ADT/TinyPtrVector.h"

namespace Nom
{
	namespace Runtime
	{

		//class TypeList
		//{
		//private:
		//	llvm::TinyPtrVector<const NomType *> types;
		//public:
		//	llvm::TinyPtrVector<const NomType *> * operator->()
		//	{
		//		return &types;
		//	}
		//	const llvm::TinyPtrVector<const NomType *> * operator->() const
		//	{
		//		return &types;
		//	}

		//	llvm::TinyPtrVector<const NomType *> operator*() const
		//	{
		//		return types;
		//	}

		//	bool PointwiseSubtype(const TypeList &other) const
		//	{
		//		if (types.size() != other.types.size())
		//		{
		//			return false;
		//		}
		//		for (int i = types.size() - 1; i >= 0; i--)
		//		{
		//			if (!(types[i]->IsSubtype(other.types[i])))
		//			{
		//				return false;
		//			}
		//		}
		//		return true;
		//	}
		//	TypeList Meet(const TypeList &other) const
		//	{
		//		if (types.size() != other.types.size())
		//		{
		//			throw new std::exception();
		//		}
		//		TypeList ret;
		//		for (unsigned int i = 0; i < types.size(); i++)
		//		{
		//			ret->push_back(types[i]->Meet(other.types[i]));
		//			i++;
		//			ret->push_back(types[i]->Join(other.types[i]));
		//		}
		//		return ret;
		//	}

		//	TypeList()
		//	{
		//	}
		//	TypeList(int defaultFields)
		//	{
		//		
		//	}

		//	~TypeList()
		//	{

		//	}
		//};

	}
}

