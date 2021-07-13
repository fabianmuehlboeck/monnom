#pragma once
#include "RTTypeVar.h"
#include "RTClassType.h"
#include "RTMaybeType.h"
//
//namespace Nom
//{
//	namespace Runtime
//	{
//		class RTClass;
//		class RTIsectType;
//		class RTUnionType;
//		class RTClassType;
//		class RTTypeHead;		
//
//		/*class RTTypeList
//		{
//		private:
//			std::vector<RTTypeHead> arguments;
//		public:
//			RTTypeList(std::vector<RTTypeHead> &arguments) : arguments(arguments)
//			{
//
//			}
//			std::vector<RTTypeHead> * operator->()
//			{
//				return &arguments;
//			}
//			std::vector<RTTypeHead>& operator*()
//			{
//				return arguments;
//			}
//			~RTTypeList()
//			{
//
//			}
//		};*/
//
//		
//
//		/*class IsectTypeList
//		{
//		private:
//			std::vector<RTClassType> cases;
//		public:
//			IsectTypeList(std::vector<RTClassType> &cases) : cases(cases)
//			{
//
//			}
//			std::vector<RTClassType> * operator->()
//			{
//				return &cases;
//			}
//			std::vector<RTClassType>& operator*()
//			{
//				return cases;
//			}
//			~IsectTypeList()
//			{
//
//			}
//		};*/
//
//		
//
//		/*class UnionTypeList
//		{
//		private:
//			const size_t count;
//			variant<const RTIsectType * const, const RTClassType * const> cases[];
//		public:
//			UnionTypeList(std::vector<variant<RTIsectType, RTClassType>> &cases) : cases(cases)
//			{
//
//			}
//
//			std::vector<variant<RTIsectType, RTClassType>> * operator->()
//			{
//				return &cases;
//			}
//			std::vector<variant<RTIsectType, RTClassType>>& operator*()
//			{
//				return cases;
//			}
//			~UnionTypeList()
//			{
//
//			}
//		};*/
//
//		
//	}
//}
