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

#include <sstream>
#include <stdint.h>
#include <memory>

//stdio is not pulled in automatically on unix (ottona)
#ifdef _UNIX
	#include <string.h>
#endif

namespace _2Real
{

	template< typename T >
	class ImageT;

	class ImageChannelOrder
	{

	public:

		enum { RED = 0, GREEN = 1, BLUE = 2, ALPHA = 3, INVALID = 255 };
		enum CHANNEL_CODE { RGBA, BGRA, ARGB, ABGR, RGBX, BGRX, XRGB, XBGR, RGB, BGR, R, G, B, A };

		ImageChannelOrder( const CHANNEL_CODE channelCode );
		ImageChannelOrder( ImageChannelOrder const& src );
		ImageChannelOrder& operator=( ImageChannelOrder const& src );

		const uint8_t	getRedOffset() const { return m_RedOffset; }
		const uint8_t	getGreenOffset() const { return m_GreenOffset; }
		const uint8_t	getBlueOffset() const { return m_BlueOffset; }
		const uint8_t	getAlphaOffset() const { return m_AlphaOffset; }
		const bool		hasAlpha() const { return (m_AlphaOffset != INVALID); }
		const uint8_t	getNumberOfChannels() const { return m_NumChannels; }
		int				getCode() const { return m_ChannelCode; }

		const bool operator==( ImageChannelOrder const& rhs ) const { return m_ChannelCode == rhs.m_ChannelCode; }
		const bool operator!=( ImageChannelOrder const& rhs ) const { return m_ChannelCode != rhs.m_ChannelCode; }

		const char *toString() const;

	private:

		void			set(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha, const uint8_t channelCount);

		CHANNEL_CODE	m_ChannelCode;
		uint8_t			m_RedOffset;
		uint8_t			m_GreenOffset;
		uint8_t			m_BlueOffset;
		uint8_t			m_AlphaOffset;
		uint8_t			m_NumChannels;

		friend std::ostream &operator << ( std::ostream &ostr, const ImageChannelOrder &ico );
		friend bool operator==( const ImageChannelOrder &lhs, ImageChannelOrder::CHANNEL_CODE rhs );
		friend bool operator==( ImageChannelOrder::CHANNEL_CODE lhs, const ImageChannelOrder &rhs );
		friend bool operator!=( const ImageChannelOrder &lhs, ImageChannelOrder::CHANNEL_CODE rhs );
		friend bool operator!=( ImageChannelOrder::CHANNEL_CODE lhs, const ImageChannelOrder &rhs );
	};

	inline std::ostream &operator << ( std::ostream &ostr, const ImageChannelOrder &ico )			{	return ostr << ico.toString();		}
	inline bool operator==( const ImageChannelOrder &lhs, ImageChannelOrder::CHANNEL_CODE rhs )		{	return lhs.m_ChannelCode == rhs;	}
	inline bool operator==( ImageChannelOrder::CHANNEL_CODE lhs, const ImageChannelOrder &rhs )		{	return lhs == rhs.m_ChannelCode;	}
	inline bool operator!=( const ImageChannelOrder &lhs, ImageChannelOrder::CHANNEL_CODE rhs )		{	return lhs.m_ChannelCode != rhs;	}
	inline bool operator!=( ImageChannelOrder::CHANNEL_CODE lhs, const ImageChannelOrder &rhs )		{	return lhs != rhs.m_ChannelCode;	}

	template< typename T >
	class ImageChannelT
	{

	private:

		class ChannelObject
		{

		public:

			ChannelObject(T *data, const bool ownsData, const uint32_t width, const uint32_t height, const uint32_t bytesPerRow, const uint8_t inc) :
				m_Data(data),
				m_IsDataOwner(ownsData),
				m_Width(width),
				m_Height(height),
				m_RowPitch(bytesPerRow),
				m_Increment(inc)
			{
			}

			~ChannelObject()
			{
				if (m_IsDataOwner)
				{
					delete [] m_Data;
				}
			}

			uint32_t		m_Width, m_Height, m_RowPitch;
			T				*m_Data;
			uint8_t			m_Increment;
			bool			m_IsDataOwner;
		};

		std::auto_ptr< ChannelObject >		m_ChannelObject;

	public:

		ImageChannelT& operator=( ImageChannelT const& src )
		{
			if ( this == &src )
			{
				return *this;
			}

			T const* srcData = src.getData();
			size_t sz = src.getWidth() * src.getHeight();
			T *data = new T[ sz ];
			memcpy( data, srcData, sz*sizeof( T ) );
			m_ChannelObject = std::auto_ptr< ChannelObject >( new ChannelObject( data, true, src.getWidth(), src.getHeight(), src.getRowPitch(), src.getIncrement() ) );

			return *this;
		}

		ImageChannelT( ImageChannelT const& src )
		{
			T const* srcData = src.getData();
			size_t sz = src.getWidth() * src.getHeight();
			T *data = new T[ sz ];
			memcpy( data, srcData, sz*sizeof( T ) );
			m_ChannelObject = std::auto_ptr< ChannelObject >( new ChannelObject( data, true, src.getWidth(), src.getHeight(), src.getRowPitch(), src.getIncrement() ) );
		}

		ImageChannelT()
		{
			m_ChannelObject = std::auto_ptr< ChannelObject >(new ChannelObject(nullptr, false, 0, 0, 0, 1));
		};

		ImageChannelT(const uint32_t width, const uint32_t height)
		{
			unsigned int rowBytes = width * sizeof(T);
			T *data = new T[width*height];
			m_ChannelObject = std::auto_ptr< ChannelObject >(new ChannelObject(data, false, width, height, rowBytes, 1));
		}

		ImageChannelT(T *data, const bool ownsData, const uint32_t width, const uint32_t height, const int32_t bytesPerRow, const uint8_t inc)
		{
			m_ChannelObject = std::auto_ptr< ChannelObject >(new ChannelObject(data, ownsData, width, height, bytesPerRow, inc));
		}

		const uint32_t	getWidth() const { return m_ChannelObject->m_Width; }
		const uint32_t	getHeight() const { return m_ChannelObject->m_Height; }
		const uint32_t	getRowPitch() const { return m_ChannelObject->m_RowPitch; }
		const uint8_t	getIncrement() const { return m_ChannelObject->m_Increment; }

		T *				getData() { return m_ChannelObject->m_Data; }
		T const*		getData() const { return m_ChannelObject->m_Data; }

		class iterator
		{

		public:

			iterator(ImageChannelT< T > &channel) :
				m_Increment(channel.getIncrement()), m_RowPitch(channel.getRowPitch()),
				m_Width(channel.getWidth()), m_Height(channel.getHeight())
			{
				m_LinePtr = reinterpret_cast< uint8_t * >(channel.getData());
				m_Ptr = reinterpret_cast< T * >(m_LinePtr);
				m_StartX = m_X = 0;
				m_StartY = m_Y = 0;
				m_EndX = m_Width;
				m_EndY = m_Height;
				--m_Y;
				m_LinePtr -= m_RowPitch;
			}

			T &				v() const { return *m_Ptr; }
			const int32_t	x() const { return m_X; }
			const int32_t	y() const { return m_Y; }

			bool nextPixel()
			{
				++m_X;
				m_Ptr += m_Increment;
				return m_X < m_EndX;
			}

			bool nextLine()
			{
				++m_Y;
				m_LinePtr += m_RowPitch;
				m_Ptr = reinterpret_cast< T * >(m_LinePtr);
				m_Ptr -= m_Increment;
				m_X = m_StartX - 1;
				return m_Y < m_EndY;
			}

		private:

			uint8_t				m_Increment;
			uint8_t				*m_LinePtr;
			T					*m_Ptr;
			uint32_t			m_RowPitch, m_Width, m_Height;
			int32_t				m_X, m_Y, m_StartX, m_StartY, m_EndX, m_EndY;

		};

		class const_iterator
		{

		public:

			const_iterator(ImageChannelT<T> const& channel) :
				m_Increment(channel.getIncrement()), m_RowPitch(channel.getRowPitch()),
				m_Width(channel.getWidth()), m_Height(channel.getHeight())
			{
				m_LinePtr = reinterpret_cast< uint8_t const* >(channel.getData());
				m_Ptr = reinterpret_cast< T const* >(m_LinePtr);
				m_StartX = m_X = 0;
				m_StartY = m_Y = 0;
				m_EndX = m_Width();
				m_EndY = m_Height();
				--m_Y;
				m_LinePtr -= m_RowPitch;
			}

			const T &		v() const { return *m_Ptr; }
			const int32_t	x() const { return m_X; }
			const int32_t	y() const { return m_Y; }

			const bool nextPixel()
			{
				++m_X;
				m_Ptr += m_Increment;
				return m_X < m_EndX;
			}

			const bool nextLine()
			{
				++m_Y;
				//m_LinePtr += m_RowIncrement;
				m_Ptr = reinterpret_cast< T const* >(m_LinePtr);
				//m_Ptr -= m_Inc;
				m_X = m_StartX - 1;
				return m_Y < m_EndY;
			}

		private:

			uint8_t				m_Increment;
			uint8_t				const* m_LinePtr;
			T					const* m_Ptr;
			uint32_t			m_RowPitch, m_Width, m_Height;
			int32_t				m_X, m_Y, m_StartX, m_StartY, m_EndX, m_EndY;

		};

		iterator		iter() { return iterator(*this); }
		const_iterator	const_iter() const { return const_iterator(*this); }

	};

	template< typename T >
	class ImageT
	{

	public:

		ImageT()
		{
			m_ImageObject = std::auto_ptr< ImageObject >( new ImageObject(nullptr, false, 0, 0, 0, ImageChannelOrder::RGBA) );
		}

		ImageT& operator=( ImageT const& src )
		{
			if ( this == &src )
			{
				return *this;
			}

			T* srcData = src.getData();
			size_t sz = src.getWidth() * src.getChannelOrder().getNumberOfChannels() * src.getHeight();
			T *data = new T[ sz ];

			memcpy( data, srcData, sz*sizeof( T ) );

			m_ImageObject = std::auto_ptr< ImageObject >( new ImageObject( data, true, src.getWidth(), src.getHeight(), src.getRowPitch(), src.getChannelOrder() ) );

			return *this;
		}

		ImageT( ImageT const& src )
		{
			T* srcData = src.getData();
			size_t sz = src.getWidth() * src.getChannelOrder().getNumberOfChannels() * src.getHeight();
			T *data = new T[ sz ];

			memcpy( data, srcData, sz*sizeof( T ) );

			m_ImageObject = std::auto_ptr< ImageObject >( new ImageObject( data, true, src.getWidth(), src.getHeight(), src.getRowPitch(), src.getChannelOrder() ) );
		}

		ImageT( const uint32_t width, const uint32_t height, ImageChannelOrder const& channelOrder )
		{
			uint32_t rowBytes = width * sizeof(T) * channelOrder.getNumberOfChannels();
			T *data = new T[width*channelOrder.getNumberOfChannels()*height];
			m_ImageObject = std::auto_ptr< ImageObject >( new ImageObject(data, true, width, height, rowBytes, channelOrder) );
		}

		ImageT(T *data, const bool ownsData, const uint32_t width, const uint32_t height, ImageChannelOrder const& channelOrder)
		{
			uint32_t rowBytes = width * sizeof(T) * channelOrder.getNumberOfChannels();
			m_ImageObject = std::auto_ptr< ImageObject >( new ImageObject(data, ownsData, width, height, rowBytes, channelOrder) );
		}

		void assign( T *data, const bool ownsData, const uint32_t width, const uint32_t height, ImageChannelOrder const& channelOrder )
		{
			uint32_t rowBytes = width * sizeof(T) * channelOrder.getNumberOfChannels();
			m_ImageObject = std::auto_ptr< ImageObject >( new ImageObject(data, ownsData, width, height, rowBytes, channelOrder) );
		}

		bool operator==( ImageT< T > const& other ) const
		{
			return ( m_ImageObject.get() == other.m_ImageObject.get() || *m_ImageObject.get() == *other.m_ImageObject.get() );
		}

		const uint32_t				getWidth() const { return m_ImageObject->m_Width; }
		const uint32_t				getHeight() const { return m_ImageObject->m_Height; }
		const uint32_t				getRowPitch() const { return m_ImageObject->m_RowPitch; }

		T *							getData() { return m_ImageObject->m_ImageData; }
		T *const					getData() const { return m_ImageObject->m_ImageData; }

		bool						hasAlpha() const { return m_ImageObject->m_ChannelOrder.hasAlpha(); }
		ImageChannelOrder const&	getChannelOrder() const { return m_ImageObject->m_ChannelOrder; }
		void						setChannelOrder(ImageChannelOrder const& channelOrder) { m_ImageObject->setChannelOrder(channelOrder); }
		uint8_t						getNumberOfChannels() const { return m_ImageObject->m_ChannelOrder.getNumberOfChannels(); }
		uint8_t						getRedOffset() const { return m_ImageObject->m_ChannelOrder.getRedOffset(); }
		uint8_t						getGreenOffset() const { return m_ImageObject->m_ChannelOrder.getGreenOffset(); }
		uint8_t						getBlueOffset() const { return m_ImageObject->m_ChannelOrder.getBlueOffset(); }
		uint8_t						getAlphaOffset() const { return m_ImageObject->m_ChannelOrder.getAlphaOffset(); }

		ImageChannelT< T > &		getChannelRed() { return m_ImageObject->m_Channels[ImageChannelOrder::RED]; }
		ImageChannelT< T > &		getChannelGreen() { return m_ImageObject->m_Channels[ImageChannelOrder::GREEN]; }
		ImageChannelT< T > &		getChannelBlue() { return m_ImageObject->m_Channels[ImageChannelOrder::BLUE]; }
		ImageChannelT< T > &		getChannelAlpha() { return m_ImageObject->m_Channels[ImageChannelOrder::ALPHA]; }
		ImageChannelT< T > const&	getChannelRed() const { return m_ImageObject->m_Channels[ImageChannelOrder::RED]; }
		ImageChannelT< T > const&	getChannelGreen() const { return m_ImageObject->m_Channels[ImageChannelOrder::GREEN]; }
		ImageChannelT< T > const&	getChannelBlue() const { return m_ImageObject->m_Channels[ImageChannelOrder::BLUE]; }
		ImageChannelT< T > const&	getChannelAlpha() const { return m_ImageObject->m_Channels[ImageChannelOrder::ALPHA]; }

		bool isEmpty() const		{ return ( m_ImageObject->m_Width == 0 || m_ImageObject->m_Height == 0 || m_ImageObject->m_ImageData == nullptr ); }

	private:

		struct ImageObject
		{

			ImageObject(T *data, const bool ownsData, const uint32_t width, const uint32_t height, const uint32_t rowBytes, ImageChannelOrder const& order) :
				m_Width(width),
				m_Height(height),
				m_RowPitch(rowBytes),
				m_ImageData(data),
				m_IsDataOwner(ownsData),
				m_ChannelOrder(order)
			{
				initChannels();
			}

			bool operator==( ImageObject const& other ) const
			{
				if ( m_ChannelOrder != other.m_ChannelOrder ) return false;
				else if ( m_Width != other.m_Width ) return false;
				else if ( m_Height != other.m_Height ) return false;
				else if ( m_RowPitch != other.m_RowPitch ) return false;
				else
				{
					T *pThis = m_ImageData;
					T *pOther = other.m_ImageData;
					unsigned int sz = m_Width * m_Height * m_ChannelOrder.getNumberOfChannels();
					for ( unsigned int i=0; i<sz; ++i )
					{
						if ( *pThis != *pOther ) return false;
						++pThis;
						++pOther;
					}

					return true;
				}
			}

			~ImageObject()
			{
				if (m_IsDataOwner)
				{
					delete [] m_ImageData;
				}
			}

			void initChannels()
			{
				m_Channels[ImageChannelOrder::RED] =
				ImageChannelT< T >(m_ImageData + m_ChannelOrder.getRedOffset(), false, m_Width, m_Height, m_RowPitch, m_ChannelOrder.getNumberOfChannels());
				m_Channels[ImageChannelOrder::GREEN] =
				ImageChannelT< T >(m_ImageData + m_ChannelOrder.getGreenOffset(), false, m_Width, m_Height, m_RowPitch, m_ChannelOrder.getNumberOfChannels());
				m_Channels[ImageChannelOrder::BLUE] =
				ImageChannelT< T >(m_ImageData + m_ChannelOrder.getBlueOffset(), false, m_Width, m_Height, m_RowPitch, m_ChannelOrder.getNumberOfChannels());
				if(m_ChannelOrder.hasAlpha())
				{
					m_Channels[ImageChannelOrder::ALPHA] =
					ImageChannelT< T >(m_ImageData + m_ChannelOrder.getAlphaOffset(), false, m_Width, m_Height, m_RowPitch, m_ChannelOrder.getNumberOfChannels());
				}
			}

			void setChannelOrder(ImageChannelOrder const& order)
			{
				m_ChannelOrder = order;
				initChannels();
			}

			uint32_t								m_Width;
			uint32_t								m_Height;
			uint32_t								m_RowPitch;
			T										*m_ImageData;
			bool									m_IsDataOwner;
			ImageChannelOrder						m_ChannelOrder;
			ImageChannelT< T >						m_Channels[4];

		};

		std::auto_ptr< ImageObject >			m_ImageObject;

	public:

		class iterator
		{

		public:

			iterator(ImageT< T > &image) :
				m_RedOffset(image.getRedOffset()), m_GreenOffset(image.getGreenOffset()),
				m_BlueOffset(image.getBlueOffset()), m_AlphaOffset(image.getAlphaOffset()),
				m_NumChannels(image.getNumberOfChannels()), m_Pitch(image.getRowPitch()),
				m_Width(image.getWidth()), m_Height(image.getHeight())
			{
				m_LinePtr = reinterpret_cast< uint8_t * >(image.getData());
				m_Ptr = reinterpret_cast< T * >(m_LinePtr);

				m_StartX = m_X = 0;
				m_StartY = m_Y = 0;
				m_EndX = m_Width;
				m_EndY = m_Height;

				--m_Y;
				m_LinePtr -= m_Pitch;
			}

			const uint32_t	x() const { return m_X; }
			const uint32_t	y() const { return m_Y; }
			T &				r() const { return m_Ptr[m_RedOffset]; }
			T &				g() const { return m_Ptr[m_GreenOffset]; }
			T &				b() const { return m_Ptr[m_BlueOffset]; }
			T &				a() const { return m_Ptr[m_AlphaOffset]; }

			bool nextPixel()
			{
				++m_X;
				m_Ptr += m_NumChannels;
				return m_X < m_EndX;
			}

			bool nextLine()
			{
				++m_Y;
				m_LinePtr += m_Pitch;
				m_Ptr = reinterpret_cast< T* >(m_LinePtr);
				m_Ptr -= m_NumChannels;
				m_X = m_StartX - 1;
				return m_Y < m_EndY;
			}

		private:

			uint8_t				m_RedOffset, m_GreenOffset, m_BlueOffset, m_AlphaOffset, m_NumChannels;
			uint8_t				*m_LinePtr;
			T					*m_Ptr;
			int32_t				m_Pitch, m_Width, m_Height;
			int32_t				m_X, m_Y, m_StartX, m_StartY, m_EndX, m_EndY;

		};

		class const_iterator
		{

		public:

			const_iterator(ImageT<T> const& image) :
				m_RedOffset(image.getRedOffset()), m_GreenOffset(image.getGreenOffset()),
				m_BlueOffset(image.getBlueOffset()), m_AlphaOffset(image.getAlphaOffset()),
				m_NumChannels(image.getNumberOfChannels()), m_Pitch(image.getRowPitch()),
				m_Width(image.getWidth()), m_Height(image.getHeight())
			{
				m_LinePtr = reinterpret_cast< uint8_t const* >(image.getData());
				m_Ptr = reinterpret_cast< T const* >(m_LinePtr);

				m_StartX = m_X = 0;
				m_StartY = m_Y = 0;
				m_EndX = m_Width;
				m_EndY = m_Height;

				--m_Y;
				m_LinePtr -= m_Pitch;
			}

			const int32_t	x() const { return m_X; }
			const int32_t	y() const { return m_Y; }
			T const&		r() const { return m_Ptr[m_RedOffset]; }
			T const&		g() const { return m_Ptr[m_GreenOffset]; }
			T const&		b() const { return m_Ptr[m_BlueOffset]; }
			T const&		a() const { return m_Ptr[m_AlphaOffset]; }

			bool nextPixel()
			{
				++m_X;
				m_Ptr += m_NumChannels;
				return m_X < m_EndX;
			}

			bool nextLine()
			{
				++m_Y;
				m_LinePtr += m_Pitch;
				m_Ptr = reinterpret_cast< T const* >(m_LinePtr);
				m_Ptr -= m_NumChannels;
				m_X = m_StartX - 1;
				return m_Y < m_EndY;
			}

		private:

			uint8_t				m_RedOffset, m_GreenOffset, m_BlueOffset, m_AlphaOffset, m_NumChannels;
			uint8_t				const* m_LinePtr;
			T					const* m_Ptr;
			int32_t				m_Pitch, m_Width, m_Height;
			int32_t				m_X, m_Y, m_StartX, m_StartY, m_EndX, m_EndY;

		};

		iterator			iter() { return iterator(*this); }
		const_iterator		const_iter() const { return const_iterator(*this); }

	};

	typedef ImageT< unsigned char >		Image8U;
	typedef ImageT< unsigned short >	Image16U;
	typedef ImageT< float >				Image32F;
	typedef ImageT< double >			Image64F;
}
