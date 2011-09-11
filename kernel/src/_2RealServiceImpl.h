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

#include "_2RealAbstractRef.h"
#include "_2RealAbstractContainer.h"

#include <list>
#include <map>

namespace _2Real
{

	/**
	*	service container
	*	makes sure the user service is updated and stuff
	*/

	class IService;
	class ServiceSlot;
	class ServiceValue;

	class ServiceContainer : public AbstractContainer
	{

	public:

		/**
		*	returns value of setup param
		*/
		void getParameterValue(std::string const& _name, AbstractRef *const _param) throw(...);

		/**
		*	registers ref of input variable
		*/
		void registerInputSlot(std::string const& _name, AbstractRef *const _var) throw(...);

		/**
		*	registers ref of output variable
		*/
		void registerOutputSlot(std::string const& _name, AbstractRef *const _var) throw(...);

		/**
		*	throws if user defined service is null
		*/
		ServiceContainer(IService *const _service, IdentifierImpl *const _id) throw(...);

		/**
		*	this one should not be called, ever
		*/
		ServiceContainer(ServiceContainer const& _src) throw(...);

		/**
		*	this one should not be called, ever
		*/
		ServiceContainer& operator=(ServiceContainer const& _src) throw(...);

		/**
		*	destructor also performs shutdown & destroys user defined service
		*/
		~ServiceContainer();

		/**
		*	called by service factory after service creation
		*/
		void addInputSlot(unsigned int const& id, ServiceSlot *const _slot) throw(...);

		/**
		*	called by service factory after service creation
		*/
		void addOutputSlot(unsigned int const& id, ServiceSlot *const _slot) throw(...);

		/**
		*	called by service factory after service creation
		*/
		void addSetupValue(unsigned int const& id, ServiceValue *const _value) throw(...);

		/**
		*	calls setup function of service when called for the first time
		*/
		void checkConfiguration() throw(...);

		/**
		*	
		*/
		void run() throw(...);

		/**
		*	
		*/
		void update() throw(...);

		/**
		*	
		*/
		void shutdown() throw(...);

		/**
		*	
		*/
		IDs setupParamIDs() const throw(...);

		/**
		*	
		*/
		IDs inputSlotIDs() const throw(...);

		/**
		*	
		*/
		IDs outputSlotIDs() const throw(...);

		/**
		*	
		*/
		std::list< ServiceSlot * > inputSlots() throw(...);

		/**
		*	
		*/
		std::list< ServiceSlot * > outputSlots() throw(...);

		/**
		*	
		*/
		std::list< ServiceValue * > setupParams() throw(...);

	private:

		/**
		*	slots are accessed by name
		*/
		typedef std::pair< const std::string, ServiceSlot * >	NamedParam;

		/**
		*	slots are accessed by name
		*/
		typedef std::map< const std::string, ServiceSlot * >	ParamMap;

		/**
		*	setup params are accessed by name
		*/
		typedef std::pair< const std::string, ServiceValue * >	NamedValue;

		/**
		*	setup params are accessed by name
		*/
		typedef std::map< const std::string, ServiceValue * >	ValueMap;

		/**
		*	input slots as map
		*/
		ParamMap												m_InputParams;

		/**
		*	
		*/
		IDs														m_InputIds;

		/**
		*	
		*/
		std::list< ServiceSlot * >								m_InputSlots;

		/**
		*	output slots
		*/
		ParamMap												m_OutputParams;

		/**
		*	
		*/
		IDs														m_OutputIds;

		/**
		*	
		*/
		std::list< ServiceSlot * >								m_OutputSlots;

		/**
		*	setup value
		*/
		ValueMap												m_SetupParams;

		/**
		*	
		*/
		IDs														m_SetupIds;

		/**
		*	
		*/
		std::list< ServiceValue * >								m_SetupValues;

		/**
		*	the user defined service
		*/
		IService												*m_Service;

	};

}