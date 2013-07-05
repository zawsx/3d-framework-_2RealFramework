/*
	CADET - Center for Advances in Digital Entertainment Technologies
	Copyright 2011 Fachhochschule Salzburg GmbH
		http://www.cadet.at

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#pragma once

#include "datatypes/_2RealFieldDescriptor.h"
#include "engine/_2RealTypeMetadata.h"

namespace _2Real
{
	template< typename TType >
	struct BaseType;

	template< >
	struct BaseType< char >
	{
		static bool isBaseType()
		{
			return true;
		}

		//static FieldDescriptor *getFieldDescriptor()
		//{
		//	return new FieldDescriptor_t< int >( Init< int >::defaultValue() );
		//}
	};

	template< >
	struct BaseType< unsigned char >
	{
		static bool isBaseType()
		{
			return true;
		}

		//static FieldDescriptor *getFieldDescriptor()
		//{
		//	return new FieldDescriptor_t< unsigned int >( Init< unsigned int >::defaultValue() );
		//}
	};

	template< >
	struct BaseType< int >
	{
		static bool isBaseType()
		{
			return true;
		}

		//static FieldDescriptor *getFieldDescriptor()
		//{
		//	return new FieldDescriptor_t< int >( Init< int >::defaultValue() );
		//}
	};

	template< >
	struct BaseType< unsigned int >
	{
		static bool isBaseType()
		{
			return true;
		}

		//static FieldDescriptor *getFieldDescriptor()
		//{
		//	return new FieldDescriptor_t< unsigned int >( Init< unsigned int >::defaultValue() );
		//}
	};

	template< typename TType >
	struct BaseType< std::vector< TType > >
	{
		static bool isBaseType()
		{
			return BaseType< TType >::isBaseType();
		}

		//static FieldDescriptor *getFieldDescriptor()
		//{
		//	return new FieldDescriptor_t< std::vector< TType > >( Init< std::vector< TType > >::defaultValue() );
		//}
	};

	template< >
	struct BaseType< std::string >
	{
		static bool isBaseType()
		{
			return true;
		}

		//static FieldDescriptor *getFieldDescriptor()
		//{
		//	return new FieldDescriptor_t< std::vector< TType > >( Init< std::vector< TType > >::defaultValue() );
		//}
	};

	template< typename TType >
	struct BaseType
	{
		static bool isBaseType()
		{
			return false;
		}

		//static FieldDescriptor *getFieldDescriptor()
		//{
		//	return nullptr;
		//}
	};
}