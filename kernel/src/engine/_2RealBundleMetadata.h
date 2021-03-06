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

#include "helpers/_2RealVersion.h"

#include <map>
#include <string>

namespace _2Real
{
	class BlockMetadata;

	namespace app
	{
		struct BlockInfo;
		struct BundleInfo;
	}

	class BundleMetadata
	{

	public:

		typedef std::map< std::string, BlockMetadata const* >					BlockMetadatas;
		typedef std::map< std::string, BlockMetadata const* >::iterator			BlockMetadataIterator;
		typedef std::map< std::string, BlockMetadata const* >::const_iterator	BlockMetadataConstIterator;

		BundleMetadata();

		app::BundleInfo				getBundleInfo() const;

		void						addBlockData( BlockMetadata const& data );
		BlockMetadata const&		getBlockData( std::string const& blockName ) const;
		BlockMetadatas const&		getExportedBlocks() const;

		void						setDescription( std::string const& description );
		void						setAuthor( std::string const& author );
		void						setContact( std::string const& contact );
		void						setName( std::string const& name );
		void						setInstallDirectory( std::string const& directory );
		void						setCategory( std::string const& category );
		void						setVersion( Version const& version );

		std::string const&			getInstallDirectory() const;
		std::string const&			getDescription() const;
		std::string const&			getAuthor() const;
		std::string const&			getContact() const;
		std::string const&			getName() const;
		std::string const&			getCategory() const;
		Version const&				getVersion() const;

	private:

		std::string					m_Name;
		std::string					m_InstallDirectory;
		std::string					m_Description;
		std::string					m_Author;
		std::string					m_Contact;
		std::string					m_Category;
		Version						m_Version;
		BlockMetadatas				m_ExportedBlocks;

	};
}