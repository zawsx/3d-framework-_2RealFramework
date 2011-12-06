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

#include "_2RealIdentifier.h"
#include "_2RealException.h"

namespace _2Real
{

	class RunnableError
	{

	public:

		RunnableError(Exception const& exception, Identifier const& sender, Identifier const& system) :
			m_Exception(exception),
			m_System(system),
			m_Sender(sender)
		{
		}

		Identifier const& system()
		{
			return m_System;
		}

		Identifier const& sender()
		{
			return m_Sender;
		}

	private:

		Exception	m_Exception;
		Identifier	m_System;
		Identifier	m_Sender;

	};

}