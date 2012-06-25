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

#include "_2RealUpdateTrigger.h"

namespace _2Real
{

	class AbstractTimeBasedTrigger : public UpdateTrigger
	{

	public:

		AbstractTimeBasedTrigger( const long timeslice ) : m_ElapsedTime( 0 ), m_DesiredTime( timeslice ), m_Condition( false ) {}
		virtual ~AbstractTimeBasedTrigger() {}
		void reset() { m_Condition = false; }
		bool isOk() const { return m_Condition; }

		virtual bool tryTrigger( const long time ) = 0;

	protected:

		bool	m_Condition;
		long	const m_DesiredTime;
		long	m_ElapsedTime;

	};

	template< typename Condition >
	class TimeBasedTrigger : public AbstractTimeBasedTrigger
	{

	public:

		TimeBasedTrigger( const long timeslice ) : AbstractTimeBasedTrigger( timeslice ) {}
		bool tryTrigger( const long time )
		{
			m_ElapsedTime += time;
			if ( !m_Condition && m_TriggerCondition( m_ElapsedTime, m_DesiredTime ) )
			{
				m_ElapsedTime = 0;
				m_Condition = true;
			}

			return m_Condition;
		}

	private:

		Condition		m_TriggerCondition;

	};

}