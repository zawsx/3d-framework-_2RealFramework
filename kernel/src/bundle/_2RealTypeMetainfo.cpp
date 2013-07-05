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

#include "bundle/_2RealTypeMetainfo.h"
#include "engine/_2RealTypeMetadata.h"

namespace _2Real
{
	namespace bundle
	{
		TypeMetainfo::TypeMetainfo( TypeMetadata &meta, std::map< std::string, TypeMetadata * > const& types ) : mImpl( meta ), mBundleTypes( types )
		{
		}

		TypeMetainfo::~TypeMetainfo()
		{
		}

		void TypeMetainfo::addFieldInternal( std::string const& name, FieldDescriptor *desc )
		{
			mImpl.addField( name, *desc );
		}

		void TypeMetainfo::addField( std::string const& name, std::string const& type )
		{
			std::map< std::string, TypeMetadata * >::const_iterator it = mBundleTypes.find( type );
			if ( it != mBundleTypes.end() )
				mImpl.addField( name, type, it->second );
		}
	}
}