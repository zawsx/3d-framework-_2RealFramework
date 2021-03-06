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
#include <iostream>
#include <string>
#include <sstream>
#include "KinectOpenNIUserSkeletonBlock.h"
#include "_2RealDatatypes.h"

using namespace _2Real;
using _2Real::bundle::BlockHandle;
using _2Real::Exception;
using std::cout;
using std::endl;
using std::string;

void KinectOpenNIUserSkeletonBlock::setup( BlockHandle &block )
{
	try
	{
		setGeneratorType(_2RealKinectWrapper::USERIMAGE);
		KinectOpenNIBlockBase::setup(block);
		m_IsAlignedToColorInletHandle = block.getInletHandle("IsAlignedToColor");
		m_IsWorldCoordinatesInletHandle = block.getInletHandle("IsWorldCoordinates");
		m_SkeletonsOutletHandle = block.getOutletHandle("Skeletons");
		m_NrOfUsersOutletHandle = block.getOutletHandle("NrOfUsers");
		m_NrOfSkeletonsOutletHandle = block.getOutletHandle("NrOfSkeletons");
		m_UsersCenterOfMass = block.getOutletHandle("UsersCenterOfMass");
		m_iNrOfUsers = m_iNrOfSkeletons = 0;
	}
	catch ( Exception &e )
	{
		cout << e.message() << endl;
		e.rethrow();
	}
}

void KinectOpenNIUserSkeletonBlock::update()
{
	try
	{
		if(m_OpenNIDeviceManager->getNumberOfConnectedDevices()<=0)	// if there is no cameras connected there is nothing todo so return
		{
			m_WidthOutletHandle.discard();
			m_HeightOutletHandle.discard();
			m_ImageOutletHandle.discard();
			return;
		}

		if(m_iCurrentDevice!=-1)
		{
			// depth to color alignment switched 
			bool bIsAlignedToColor = m_IsAlignedToColorInletHandle.getReadableRef<bool>();
			if( bIsAlignedToColor != m_bIsAlignedToColor)
			{
				m_OpenNIDeviceManager->setAlignToColor(m_iCurrentDevice, bIsAlignedToColor);
				m_bIsAlignedToColor = bIsAlignedToColor;
			}

			// world coords
			m_bIsWorldCoordinates = m_IsWorldCoordinatesInletHandle.getReadableRef<bool>();

			// get and set nr of detected users
			int iNrOfUsers = m_OpenNIDeviceManager->getNumberOfUsers(m_iCurrentDevice);
			if(iNrOfUsers != m_iNrOfUsers)
			{
				m_iNrOfUsers = iNrOfUsers;
				m_NrOfUsersOutletHandle.getWriteableRef<int>() = m_iNrOfUsers;
			}
			else
			{
				m_NrOfUsersOutletHandle.discard();
			}
			//get and set nr of skeletons
			int iNrOfSkeletons = m_OpenNIDeviceManager->getNumberOfSkeletons(m_iCurrentDevice);
			if(iNrOfSkeletons != m_iNrOfSkeletons)
			{
				m_iNrOfSkeletons = iNrOfSkeletons;
				m_NrOfSkeletonsOutletHandle.getWriteableRef<int>() = m_iNrOfSkeletons;
			}
			else
			{
				m_NrOfSkeletonsOutletHandle.discard();
			}

			if(m_iNrOfUsers>0)
			{
				std::vector<Point> points;
				for(int i=0; i<m_iNrOfUsers; i++)
				{
					points.push_back(m_OpenNIDeviceManager->getUsersCenterOfMass(m_iCurrentDevice, i, m_bIsWorldCoordinates));
				}
				m_UsersCenterOfMass.getWriteableRef<std::vector< _2Real::Point >>() = points;
			}
			else
			{
				m_SkeletonsOutletHandle.discard();
			}

			// get and set skeletons
			m_SkeletonsOutletHandle.getWriteableRef<std::vector< _2Real::Skeleton >>() = m_OpenNIDeviceManager->getSkeletons(m_iCurrentDevice, m_bIsWorldCoordinates);
		}

		// call update of base class for getting device and possible user image
		KinectOpenNIBlockBase::update();
	}
	catch ( Exception &e )
	{
		cout << e.message() << endl;
		e.rethrow();
	}
}
