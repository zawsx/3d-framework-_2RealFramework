
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

#include "_2RealPlugin.h"
#include "_2RealPluginContext.h"
#include "_2RealException.h"
#include "_2RealServiceFactory.h"
#include "_2RealServiceMetadata.h"

#include <iostream>

namespace _2Real
{

	Plugin::Plugin(std::string const& _dir, std::string const& _file, std::string const& _class, IdentifierImpl *const _id) :
		Entity(_id),
		m_Metadata(_class, _dir),
		m_Activator(NULL),
		m_File(_dir + _file)
	{
	}

	Plugin::Plugin(Plugin const& _src) :
		Entity(_src),
		m_Metadata("", "")
	{
		throw Exception("internal error: attempted to copy an entity");
	}

	Plugin& Plugin::operator=(Plugin const& _src)
	{
		throw Exception("internal error: attempted to copy an entity");
	}

	Plugin::~Plugin()
	{
		try
		{
			if (m_Activator)
			{
				uninstall();
			}
		}
		catch (Exception &e)
		{
			std::cout << "plugin destruction error: " << e.what() << std::endl;
		}
	}

	PluginMetadata const& Plugin::pluginMetadata() const
	{
		return m_Metadata;
	}

	ServiceMetadata const& Plugin::serviceMetadata(std::string const& _name) const
	{
		return m_Metadata.getServiceMetadata(_name);
	}

	IDs const& Plugin::serviceIDs() const
	{
		return m_ServiceIDs;
	}

	void Plugin::install(ServiceFactory *const _factory)
	{
		try
		{
			if (m_Metadata.getInstallDirectory().empty())
			{
				throw Exception("could not install plugin - no install directory was given");
			}

			if (m_Metadata.getClassname().empty())
			{
				throw Exception("could not install plugin - no classname was given");
			}

			try
			{
				m_PluginLoader.loadLibrary(m_File);
			}
			catch (...)
			{
				//poco error
				throw Exception("could not install plugin - could not load dll");
			}

			if (m_PluginLoader.canCreate(m_Metadata.getClassname()))
			{
				try
				{
					m_Activator = m_PluginLoader.create(m_Metadata.getClassname());
				}
				catch (...)
				{
					//poco error
					throw Exception("could not install plugin - could not load dll");
				}

				m_Activator->getMetadata(m_Metadata);
#ifdef _VERBOSE
	std::cout << "plugin installation: metadata loaded" << std::endl;
#endif
				PluginContext context(this, _factory);
				m_Activator->setup(context);
#ifdef _VERBOSE
	std::cout << "plugin Installation: setup success" << std::endl;
#endif
			}
			else
			{
				throw Exception("could not install plugin - could not load dll");
			}
		}
		catch (Exception &e)
		{
			if (m_Activator)
			{
				delete m_Activator;
				m_Activator = NULL;
			}

			if (!m_ServiceIDs.empty())
			{
				//TODO: clean up services
			}

			throw e;
		}
	}

	void Plugin::uninstall() throw(...)
	{
		try
		{
			if (m_PluginLoader.isLibraryLoaded(m_File))
			{
				m_PluginLoader.unloadLibrary(m_File);
			}

			m_PluginLoader.destroy(m_Metadata.getClassname(), m_Activator);
			delete m_Activator;
			m_Activator = NULL;
		}
		catch (Exception &e)
		{
			throw e;
		}
	}
}