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

#include "_2RealOutputContainer.h"

#include "_2RealGroupContainer.h"
#include "_2RealServiceContainer.h"
#include "_2RealFramework.h"

#include "_2RealAbstractContainer.h"
#include "_2RealConfigMetadata.h"

namespace _2Real
{

	const Variable OutputContainer::generateName(std::string const& _name) const
	{
#ifdef _DEBUG
		return m_CurrentService->serviceName() + "." + _name;
#else
		return ++m_iVariableCounter;
#endif
	}

	void OutputContainer::beginConfiguration()
	{
		if (m_bIsConfigured)
		{
			std::cout << "TODO: error message" << std::endl;
			return;
		}

		m_ConfigurationPtr = ConfigMetadataPtr(new ConfigMetadata(m_ServiceName));
		m_CurrentConfiguration = m_ConfigurationPtr;
		m_iVariableCounter = 0;
	}

	const bool OutputContainer::endConfiguration()
	{
		if (m_bIsConfigured)
		{
			std::cout << "TODO: error message" << std::endl;
			return false;
		}

		m_bIsConfigured = true;
		return true;
	}

	void OutputContainer::beginSequence()
	{
		if (m_bIsConfigured)
		{
			std::cout << "TODO: error message, OutputContainer::beginSequence" << std::endl;
			return;
		}

		if (!m_CurrentService.isNull())
		{
			std::cout << "TODO: error message, OutputContainer::beginSequence" << std::endl;
			return;
		}

		GroupContainerPtr previousGroup = m_CurrentGroup;
		
		ServicePtr service = m_FrameworkPtr->createService("SequenceContainer", "Framework").second;
		
		m_CurrentGroup = service.unsafeCast< GroupContainer >();

		if (!previousGroup.isNull())
		{
			m_GroupContainers.push(previousGroup);
			
			previousGroup->addElement(m_CurrentGroup);
		}
		else
		{
			m_TopLevelGroup = m_CurrentGroup;
		}

		MetadataPtr newConfig = MetadataPtr(new ConfigMetadata(m_CurrentGroup->name()));

		m_CurrentConfiguration->insert(newConfig);
		
		m_CurrentConfiguration = newConfig.unsafeCast< ConfigMetadata >();

	}

	void OutputContainer::beginSynchronization()
	{
		if (m_bIsConfigured)
		{
			std::cout << "TODO: error message, OutputContainer::beginSynchronization" << std::endl;
			return;
		}

		if (!m_CurrentService.isNull())
		{
			std::cout << "TODO: error message, OutputContainer::beginSynchronization" << std::endl;
			return;
		}

		GroupContainerPtr previousGroup = m_CurrentGroup;
		
		ServicePtr service = m_FrameworkPtr->createService("SynchronizationContainer", "Framework").second;
		
		m_CurrentGroup = service.unsafeCast< GroupContainer >();

		if (!previousGroup.isNull())
		{
			m_GroupContainers.push(previousGroup);
			
			previousGroup->addElement(m_CurrentGroup);
		}
		else
		{
			m_TopLevelGroup = m_CurrentGroup;
		}

		MetadataPtr newConfig = MetadataPtr(new ConfigMetadata(m_CurrentGroup->name()));
		m_CurrentConfiguration->insert(newConfig);
		m_CurrentConfiguration = newConfig.unsafeCast< ConfigMetadata >();
	}

	const bool OutputContainer::endGroup()
	{
		if (m_bIsConfigured)
		{
			return false;
		}

		if (!m_CurrentService.isNull())
		{
			std::cout << "cannot begin new configuration" << std::endl;
			return false;
		}

		if (!m_GroupContainers.empty())
		{
			m_CurrentGroup = m_GroupContainers.top();
			m_GroupContainers.pop();

			MetadataPtr oldConfig = m_CurrentConfiguration->father();
			m_CurrentConfiguration = oldConfig.unsafeCast< ConfigMetadata >();
		}

		return true;
	}

	void OutputContainer::beginServiceConfiguration(std::string const& _name, std::string const& _plugin)
	{
		if (m_bIsConfigured)
		{
			std::cout << "TODO: error message, OutputContainer::beginServiceConfiguration" << std::endl;
			return;
		}

		if (!m_CurrentService.isNull())
		{
			std::cout << "TODO: error message, OutputContainer::beginServiceConfiguration" << std::endl;
			return;
		}

		ServicePtr service = m_FrameworkPtr->createService(_name, _plugin).second;
		if (!service.isNull())
		{
			m_CurrentService = service.unsafeCast< ServiceContainer >();
			
			MetadataPtr newConfig = MetadataPtr(new ConfigMetadata(m_CurrentService->name()));
			m_CurrentConfiguration->insert(newConfig);
			m_CurrentConfiguration = newConfig.unsafeCast< ConfigMetadata >();

			MetadataPtr serviceConfig = MetadataPtr(new ConfigMetadata(m_CurrentService->serviceName()));
			m_CurrentConfiguration->insert(serviceConfig);
			m_CurrentConfiguration = serviceConfig.unsafeCast< ConfigMetadata >();
		}
		else
		{
			std::cout << "TODO: error message, OutputContainer::beginServiceConfiguration" << std::endl;
		}
	}

	const bool OutputContainer::endServiceConfiguration()
	{

		m_CurrentService.assign(NULL);
		MetadataPtr oldConfig = m_CurrentConfiguration->father()->father();
		m_CurrentConfiguration = oldConfig.unsafeCast< ConfigMetadata >();

		return true;
	}

	const Variable OutputContainer::configureOutputParameter(std::string const& _name)
	{
		Variable var = generateName(_name);
		m_CurrentConfiguration->setOutputParameter< Variable >(_name, var);
		return var;
	}

	void OutputContainer::configureInputParameter(std::string const& _name, Variable const& _var)
	{
		m_CurrentConfiguration->setInputParameter< Variable >(_name, _var);
	}
}