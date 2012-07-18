#include "BlockOutletWidget.h"

#include <iostream>

using namespace std;
using namespace _2Real;

BlockOutletWidget::BlockOutletWidget(_2Real::app::OutletHandle& imageHandle, QWidget *parent) 
	: m_OutletHandle(imageHandle),
	  QGroupBox(parent)
{
	try
	{
		m_Layout = new QHBoxLayout();
		m_Layout->addWidget( new QLabel(QString::fromStdString( m_OutletHandle.getName() )) );

		m_ValueWidget = new QLabel(this);
		m_Layout->addWidget( m_ValueWidget );
		setLayout( m_Layout );

		// register data callback for _2Real Framework
		m_OutletHandle.registerToNewData( *this, &BlockOutletWidget::receiveData );

		// note that the moc is quite pedantic with regards to namespaces:
		// you must use the full symbol name here & in the function implementations
		// data between threads has to be sent around through signal and slots in QT !!!!
		qRegisterMetaType< _2Real::app::AppData >( "_2Real::app::AppData" );
		connect( this, SIGNAL( sendData( _2Real::app::AppData ) ), this, SLOT( updateData( _2Real::app::AppData ) ) );
	}
	catch(Exception& e)
	{
		cout << e.message() << e.what() << std::endl;
	}
}

void BlockOutletWidget::receiveData(_2Real::app::AppData const& data)
{
	//keep the data around as long as it and it's underlying pointers are needed, it is internally a share_ptr !!!!!
	emit sendData( data );			// since a copy of the incoming data is emitted
									// it does not matter how much time passes 
									// between this and 'updateData':
									// the data will for sure be alive when the pixmap is updated
}

void BlockOutletWidget::updateData(_2Real::app::AppData data) 
{
	try
	{
		if(m_OutletHandle.getTypename().find("img_uchar")==0)
		{	
			m_Width  = data.getData<ImageT<unsigned char>>().getWidth();
			m_Height = data.getData<ImageT<unsigned char>>().getHeight();
			unsigned char* ptr = data.getData<ImageT<unsigned char>>().getData();
			m_Img = QImage( ptr, m_Width,m_Height, QImage::Format_RGB888);
			m_Pixmap = QPixmap::fromImage(m_Img);

			dynamic_cast<QLabel*>(m_ValueWidget)->setPixmap(m_Pixmap);
		}
		else if( m_OutletHandle.getTypename() == "short" || m_OutletHandle.getTypename() == "unsigned short" ||			// handle numeric types all the same and display the number as string '1234'
			m_OutletHandle.getTypename() == "int" || m_OutletHandle.getTypename() == "unsigned int" || 
			m_OutletHandle.getTypename() == "long" || m_OutletHandle.getTypename() == "unsigned long" ||
			m_OutletHandle.getTypename() == "float" || m_OutletHandle.getTypename() == "double" ||  
			m_OutletHandle.getTypename() == "string")
		{
			dynamic_cast<QLabel*>(m_ValueWidget)->setText(QString::fromStdString(data.getDataAsString()));
		}
		else if( m_OutletHandle.getTypename() == "char")
		{
			stringstream strTmp;
			strTmp << data.getDataAsString() << " : " << data.getData<char>();
			dynamic_cast<QLabel*>(m_ValueWidget)->setText(QString::fromStdString( strTmp.str()));
		}
		else if( m_OutletHandle.getTypename() == "unsigned char")
		{
			stringstream strTmp;
			strTmp << data.getDataAsString() << " : " << data.getData<unsigned char>();
			dynamic_cast<QLabel*>(m_ValueWidget)->setText(QString::fromStdString( strTmp.str()));
		}
		else if( m_OutletHandle.getTypename() == "bool" )
		{
			stringstream strTmp;
			bool bTmp =  data.getData<bool>();
			if(bTmp)
			{
				strTmp << bTmp << " : true";
			}
			else
			{
				strTmp << bTmp << " : false";
			}
			dynamic_cast<QLabel*>(m_ValueWidget)->setText(QString::fromStdString( strTmp.str()));
		}


		update();
	}
	catch(Exception& e)
	{
		cout << e.message() << e.what() << std::endl;
	}
}