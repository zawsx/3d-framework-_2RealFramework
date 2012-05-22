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

#include "_2RealInlet.h"
#include "_2RealOutlet.h"
#include "_2RealException.h"
#include "_2RealData.h"
#include "_2RealParameterData.h"
#include "_2RealAbstractStateManager.h"

#include <iostream>

#include "Poco/Delegate.h"

namespace _2Real
{

	Inlet::Inlet(ParameterData const& metadata, BufferPolicy &policy, const unsigned int bufferSize) :
		Parameter(metadata),
		m_DataMutex(),
		m_CallbackMutex(),
		m_OutletsMutex(),
		m_ReceivedTable(bufferSize),
		m_CurrentTable(bufferSize),
		m_LinkedOutlets(),
		m_OverflowPolicy(policy),
		m_HasDefault(false),
		m_DefaultValue(),
		m_NrOfConsumed(0),
		m_LastTimestamp(0),
		m_IsSet(false),
		m_SetValue()
	{
		//if (metadata.hasDefaultValue())
		//{
			m_HasDefault = true;

			//default value has a timestamp of 0
			//meaning it will always fullfill the data-available requirement
			//but never the data-new requirement
			m_DefaultValue = TimestampedData(0, metadata.getDefaultValue());
			//m_ReceivedTable.insert(m_DefaultValue);
			m_CurrentTable.insert(m_DefaultValue);
		//}
	}

	Inlet::~Inlet()
	{
	}

	void Inlet::clearLinks()
	{
		for ( std::list< Outlet * >::iterator it = m_LinkedOutlets.begin(); it != m_LinkedOutlets.end(); ++it )
		{
			(*it)->removeListener( *this );
		}

		m_LinkedOutlets.clear();
	}

	const bool Inlet::hasDefault() const
	{
		return m_HasDefault;
	}

	void Inlet::removeConsumedItems()
	{
		//no sync, as this is only called after the update function is finished
		for (unsigned int i=0; i<m_NrOfConsumed; ++i)
		{
			m_CurrentTable.erase(m_CurrentTable.begin());
		}

		m_NrOfConsumed = 0;
	}

	void Inlet::syncBuffers()
	{
		Poco::FastMutex::ScopedLock lock(m_DataMutex);

		m_OverflowPolicy.copyData(m_CurrentTable, m_ReceivedTable);
		m_ReceivedTable.setNewMax(m_CurrentTable.getMaxSize() - m_CurrentTable.size());

		m_ReceivedTable.clear();

		//now holds time of oldest item in buffer
		if (!m_CurrentTable.empty())
		{
			m_LastTimestamp = m_CurrentTable.begin()->first;
		}
	}

	void Inlet::resetData()
	{
		if (m_IsSet)
		{
			m_ReceivedTable.insert(m_SetValue);

			std::pair< long, long > times = std::make_pair< long, long >(m_LastTimestamp, m_SetValue.first);
			m_DataReceived.notify(this, times);
		}
		else if (m_HasDefault)
		{
			m_ReceivedTable.insert(m_DefaultValue);

			std::pair< long, long > times = std::make_pair< long, long >(m_LastTimestamp, m_DefaultValue.first);
			m_DataReceived.notify(this, times);
		}
	}

	void Inlet::setData(TimestampedData const& data)
	{
		Poco::FastMutex::ScopedLock lock(m_DataMutex);

		resetLinks();

		m_IsSet = true;
		m_SetValue = data;

		//kills everything inside of received
		m_ReceivedTable.clear();
		m_ReceivedTable.insert(data);

		m_LastTimestamp = m_SetValue.first;
		std::pair< long, long > times = std::make_pair< long, long >(0, m_LastTimestamp);
		m_DataReceived.notify(this, times);
	}

	void Inlet::insertData(TimestampedData const& data)
	{
		Poco::FastMutex::ScopedLock lock(m_DataMutex);

		if (m_OverflowPolicy.insertData(data, m_ReceivedTable))
		{
			std::pair< long, long > times = std::make_pair< long, long >(m_LastTimestamp, data.first);
			m_DataReceived.notify(this, times);
		}
	}

	void Inlet::receiveData(Data &data)
	{
		Poco::FastMutex::ScopedLock lock(m_DataMutex);

		long timestamp = data.getTimestamp();

		if (m_OverflowPolicy.insertData(TimestampedData(timestamp, data.data()), m_ReceivedTable))
		{
			std::pair< long, long > times = std::make_pair< long, long >(m_LastTimestamp, timestamp);
			m_DataReceived.notify(this, times);
		}
	}

	EngineData Inlet::consumeDataItem()
	{
		//no sync necessary, this is only called from within a service's update function

		if (m_CurrentTable.empty())
		{
			throw Exception("internal exception: input slot " + getName() + " has no data.");
		}

		DataBuffer::iterator result = m_CurrentTable.begin();
		for (unsigned int i=0; i<m_NrOfConsumed; ++i)
		{
			++result;
		}
		m_NrOfConsumed++;

		return result->second;
	}

	const EngineData Inlet::getNewest() const
	{
		Poco::FastMutex::ScopedLock lock(m_DataMutex);

		if (m_CurrentTable.empty())
		{
			throw Exception("internal exception: input slot " + getName() + " has no data.");
		}

		DataBuffer::const_iterator result = m_CurrentTable.begin();
		return result->second;
	}

	void Inlet::linkWith(Outlet &outlet)
	{
		Poco::FastMutex::ScopedLock lock(m_OutletsMutex);

		std::list< Outlet * >::iterator it = std::find< std::list< Outlet * >::iterator, Outlet * >(m_LinkedOutlets.begin(), m_LinkedOutlets.end(), &outlet);
		if (it == m_LinkedOutlets.end())
		{
			m_LinkedOutlets.push_back(&outlet);
			outlet.addListener(*this);
		}
	}

	void Inlet::breakLink(Outlet &outlet)
	{
		Poco::FastMutex::ScopedLock lock(m_OutletsMutex);

		std::list< Outlet * >::iterator it = std::find< std::list< Outlet * >::iterator, Outlet * >(m_LinkedOutlets.begin(), m_LinkedOutlets.end(), &outlet);
		if (it != m_LinkedOutlets.end())
		{
			m_LinkedOutlets.erase(it);
			outlet.removeListener(*this);
		}
	}

	void Inlet::unlink(Outlet &outlet)
	{
		Poco::FastMutex::ScopedLock lock(m_OutletsMutex);

		std::list< Outlet * >::iterator it = std::find< std::list< Outlet * >::iterator, Outlet * >(m_LinkedOutlets.begin(), m_LinkedOutlets.end(), &outlet);
		if (it != m_LinkedOutlets.end())
		{
			m_LinkedOutlets.erase(it);
		}
	}

	void Inlet::resetLinks()
	{
		Poco::FastMutex::ScopedLock lock(m_OutletsMutex);

		for (std::list< Outlet * >::iterator it = m_LinkedOutlets.begin(); it != m_LinkedOutlets.end(); ++it)
		{
			(*it)->removeListener(*this);
		}

		m_LinkedOutlets.clear();
	}

	void Inlet::clearReceived()
	{
		Poco::FastMutex::ScopedLock lock(m_DataMutex);

		m_ReceivedTable.clear();
	}

	const bool Inlet::isLinked() const
	{
		Poco::FastMutex::ScopedLock lock(m_OutletsMutex);

		return (m_LinkedOutlets.size() > 0);
	}

	void Inlet::registerToDataReceived(AbstractStateManager &triggers)
	{
		m_DataReceived += Poco::delegate(&triggers, &AbstractStateManager::tryTriggerInlet);
	}

	void Inlet::unregisterFromDataReceived(AbstractStateManager &triggers)
	{
		m_DataReceived -= Poco::delegate(&triggers, &AbstractStateManager::tryTriggerInlet);
	}

}