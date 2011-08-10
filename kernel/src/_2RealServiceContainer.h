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

#include "_2RealTypedefs.h"

#include "_2RealAbstractContainer.h"

#include <list>

#include "Poco/BasicEvent.h"

namespace _2Real
{
	class ServiceContainer : public AbstractContainer
	{

		friend class Framework;

	public:

		void start(bool const& _loop);
		void stop();
		void run();

		const bool setup(ConfigMetadataPtr const& _config);
		void shutdown();
		void update();

		void addListener(ServicePtr &_listener);
		void removeListener(ServicePtr &_listener);
		void serviceListener(DataPtr &_input);

		ServiceName const& serviceName() const { return m_ServicePtr->name(); }

	private:

		typedef std::list< Variable >	VariableList;

		std::list< DataPtr >				m_Data;
		Poco::Mutex							m_Lock;
		Poco::BasicEvent< DataPtr >			m_OutputEvent;
		VariableList						m_InputVariables;
		ServicePtr							m_ServicePtr;

	};
}