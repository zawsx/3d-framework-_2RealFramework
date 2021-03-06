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
#include "GeometryBlock.h"
#include "RessourceManagerBlock.h"

#include "OpenGl.h"
#include "RessourceManager.h"
#include "Datatypes.h"

#include <iostream>

using namespace std;
using namespace _2Real;
using namespace _2Real::bundle;
using namespace _2Real::gl;

FullscreenMeshBlock::FullscreenMeshBlock( ContextBlock &context ) :
	Block(), mManager( dynamic_cast< RessourceManagerBlock & >( context ) ), mContext( nullptr ),
	mVertexBufferObj( nullptr ), mTexcoordBufferObj( nullptr ), mIndexBufferObj( nullptr )
{
}

FullscreenMeshBlock::~FullscreenMeshBlock()
{
}

void FullscreenMeshBlock::setup( BlockHandle &block )
{
	try
	{
		mBlockHandle = block;
		mMeshWidthIn = mBlockHandle.getInletHandle( "MeshWidth" );
		mMeshHeightIn = mBlockHandle.getInletHandle( "MeshHeight" );
		mPrimitiveTypeIn = mBlockHandle.getInletHandle( "PrimitiveType" );
		mVerticesOut = mBlockHandle.getOutletHandle( "VertexPositions" );
		mTexcoordsOut = mBlockHandle.getOutletHandle( "VertexTexcoords" );
		mIndicesOut = mBlockHandle.getOutletHandle( "Indices" );

		if ( mContext == nullptr )
		{
			mContext = new Context( mManager.getRenderSettings(), mManager.getManager() );
		}

		mContext->setActive( true );
		calcAttribs(	mMeshWidthIn.getReadableRef< unsigned int >(),
						mMeshHeightIn.getReadableRef< unsigned int >(),
						mPrimitiveTypeIn.getReadableRef< int >()
					);
		mContext->finish();
		mContext->setActive( false );
	}
	catch( Exception & e )
	{
		cout << e.message() << " " << e.what() << endl;
		e.rethrow();
	}
	catch( std::exception & e )
	{
		cout << e.what() << endl;
		Exception exc( e.what() );
		throw exc;
	}
}

void FullscreenMeshBlock::calcAttribs( const unsigned int w, const unsigned int h, const int p )
{
	static unsigned int POINTS = 0;
	static unsigned int TRIANGLES = 1;
	static unsigned int LINES = 2;

	size_t vertexBufferSize = w * h * 3 * sizeof( float );
	size_t texcoordBufferSize = w * h * 2 * sizeof( float );
	
	size_t indexBufferSize;
	if ( p == PrimitiveType::TRIANGLES )	indexBufferSize = 6 * ( w-1 ) * ( h-1 ) * sizeof( unsigned int );
	else									indexBufferSize = w * h * sizeof( unsigned int );

	if ( mVertexBufferObj == nullptr || mVertexBufferObj->mSizeInBytes != vertexBufferSize )
	{
		mVertexBufferObj = mContext->createBufferObj();
		mVertexBuffer.reset( mVertexBufferObj );
	}

	if ( mTexcoordBufferObj == nullptr || mTexcoordBufferObj->mSizeInBytes != texcoordBufferSize )
	{
		mTexcoordBufferObj = mContext->createBufferObj();
		mTexcoordBuffer.reset( mTexcoordBufferObj );
	}

	if ( mIndexBufferObj == nullptr || mIndexBufferObj->mSizeInBytes != indexBufferSize )
	{
		mIndexBufferObj = mContext->createBufferObj();
		mIndexBuffer.reset( mIndexBufferObj );
	}

	const float stepU = 1.0f/static_cast< float >( w );
	const float stepV = 1.0f/static_cast< float >( h );
	const float stepX = 2.0f/static_cast< float >( w );
	const float stepY = 2.0f/static_cast< float >( h );

	vector< float > texcoords;
	texcoords.reserve( w*h*2 );

	vector< float > positions;
	positions.reserve( w*h*3 );

	for ( unsigned int i=0; i<h; ++i )
	{
		for ( unsigned int j=0; j<w; ++j )
		{
			float u = j*stepU;
			float v = i*stepV;
			float x = j*stepX - 1.f;
			float y = i*stepY - 1.f;
			texcoords.push_back( u );
			texcoords.push_back( v );
			positions.push_back( x );
			positions.push_back( y );
			positions.push_back( 0.0f );
		}
	}

	vector< unsigned int > indices;
	const unsigned int numTriangles = 2 * ( w-1 ) * ( h-1 );
	const unsigned int numPoints = w*h;

	if ( p == PrimitiveType::TRIANGLES )
	{
		const unsigned int numIndices = 3 * numTriangles;
		indices.reserve( numIndices );

		for ( unsigned int i=0; i<h-1; i+=2 )
		{
			for ( unsigned int j=0; j<w-1; j+=2 )
			{
				unsigned int i0 = i*w + j;
				unsigned int i1 = i*w + ( j+2 );
				unsigned int i2 = ( i+2 )*w + j;
				unsigned int i3 = ( i+2 )*w + ( j+2 );

				indices.push_back( i0 );
				indices.push_back( i1 );
				indices.push_back( i2 );
				indices.push_back( i2 );
				indices.push_back( i1 );
				indices.push_back( i3 );
			}
		}
	}
	else
	{
		const unsigned int numIndices = numPoints;
		indices.reserve( numIndices );

		for ( unsigned int i=0; i<h; ++i )
		{ 
			for ( unsigned int j=0; j<w; ++j )
			{
				unsigned int index = i*w + j;
				indices.push_back( index );
			}
		}
	}

	mVertexBufferObj->mLock.writeLock();
	mContext->updateBuffer( mVertexBufferObj, positions, GL_DYNAMIC_DRAW );
	mVertexBufferObj->mLock.unlock();

	mTexcoordBufferObj->mLock.writeLock();
	mContext->updateBuffer( mTexcoordBufferObj, texcoords, GL_DYNAMIC_DRAW );
	mTexcoordBufferObj->mLock.unlock();

	mIndexBufferObj->mLock.writeLock();
	mContext->updateBuffer( mIndexBufferObj, indices, GL_DYNAMIC_DRAW );
	mIndexBufferObj->mLock.unlock();
}

void FullscreenMeshBlock::update()
{
	try
	{
		if ( mMeshWidthIn.hasChanged() || mMeshHeightIn.hasChanged() || mPrimitiveTypeIn.hasChanged() )
		{
			mContext->setActive( true );
			const unsigned int w = mMeshWidthIn.getReadableRef< unsigned int >();
			const unsigned int h = mMeshHeightIn.getReadableRef< unsigned int >();
			const int primType = mPrimitiveTypeIn.getReadableRef< int >();
			calcAttribs( w, h, primType );
			mContext->finish();
			mContext->setActive( false );
		}

		mVerticesOut.getWriteableRef< Buffer >() = mVertexBuffer;
		mTexcoordsOut.getWriteableRef< Buffer >() = mTexcoordBuffer;
		mIndicesOut.getWriteableRef< Buffer >() = mIndexBuffer;
	}
	catch( Exception & e )
	{
		cout << e.message() << " " << e.what() << endl;
		e.rethrow();
	}
	catch( std::exception & e )
	{
		cout << e.what() << endl;
		Exception exc( e.what() );
		throw exc;
	}
}

void FullscreenMeshBlock::shutdown()
{
	try
	{
		mVertexBufferObj = nullptr;
		mTexcoordBufferObj = nullptr;
		mIndexBufferObj = nullptr;
		delete mContext;
	}
	catch( Exception & e )
	{
		cout << e.message() << " " << e.what() << endl;
		e.rethrow();
	}
	catch( std::exception & e )
	{
		cout << e.what() << endl;
		Exception exc( e.what() );
		throw exc;
	}
}