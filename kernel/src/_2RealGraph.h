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

#include "_2RealEntity.h"
#include "_2RealException.h"

#include <list>

namespace _2Real
{

	/**
	*	system graphs, sequences and synchronizations are graphs
	*/

	class Runnable;
	typedef std::list< Runnable * >	RunnableList;

	class Graph
	{

	public:

		Graph();
		virtual ~Graph();

		const unsigned int childCount() const
		{
			return m_Children.size();
		}

		std::list< unsigned int > childIDs() const;

		std::list< Runnable * > & children()
		{
			return m_Children;
		}

		std::list< Runnable * > const& children() const
		{
			return m_Children;
		}

		bool const isChild(unsigned int const& id) const;
		bool const isInGraph(unsigned int const& id) const;

		Runnable *const			getChild(unsigned int const& id);
		Runnable *const			findChild(unsigned int const& id);
		Runnable const*const	findChild(unsigned int const& id) const;

		/**
		*	those are different depending on the concrete type of graph
		*/
		virtual void insertChild(Runnable *const child, unsigned int const& index) = 0;
		virtual void removeChild(unsigned int const& id) = 0;

	protected:

		RunnableList::iterator				iteratorId(unsigned int const& id);
		RunnableList::const_iterator		iteratorId(unsigned int const& id) const;
		RunnableList::iterator				iteratorPosition(unsigned int const& pos);
		RunnableList::const_iterator		iteratorPosition(unsigned int const& pos) const;

		RunnableList						m_Children;

	};

	class ChildNotFoundException : public Exception
	{

		friend class Graph;

		ChildNotFoundException() :
			Exception("")
		{
		}

	};

}