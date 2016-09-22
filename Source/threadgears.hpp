#ifndef THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_
#define THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_

// This file is part of sisu.

// sisu is free software: you can redistribute it and/or modify // it under the terms of the GNU General Public License as published by 
// the Free Software Foundation, either version 3 of the License, or // (at your option) any later version.

// sisu is distributed in the hope that it will be useful, // but WITHOUT ANY WARRANTY; without even the implied warranty of // 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the // GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.

#include <pthread.h>
#include <iostream>
#include <functional>
#include <vector>
#include <deque>
#include <unordered_map>
#include <time.h>
#include <errno.h>
#include <algorithm>
#include <string>

namespace sisu {

namespace _initializers {

class _pthreadInitializer
{
	static pthread_once_t const mOnce = PTHREAD_ONCE_INIT;
};

} // namespace _initializers

class mutex
{
	pthread_mutex_t mMutex;

	public:
		mutex( const pthread_mutexattr_t * xAttributes = NULL );
		~mutex( );

		void lock( );
		void unlock( );
		void run( std::function<void(void)> xLambda );
};


class event
{
	mutex mM;
	bool mSet;

		public:
			event( )
				: mM( )
				, mSet( false )
			{
				;
			}

		bool isSet( )
		{
			bool isSet = false;
			mM.run([&](){ isSet = mSet; });
			return isSet;
		}

		void set( )
		{
			mM.run([&](){ mSet = true; });
		}

		void reset( )
		{
			mM.run([&](){ mSet = false; });
		}


};

class sleep
{
	static int _sleep( timespec * const xDuration );

	public:
		static void ms( int64_t const xMilliseconds );
		static void ns( int64_t const xNanoseconds );

};

template< typename XReturnType, typename XParameterType >
class gear
{
	struct _PThreadStatus
	{
		_PThreadStatus( )
		{

		}

		void join( )
		{
			if ( mStatus != 0 )
			{
				std::cerr << "Thread with status " << mStatus << "detected. " << std::endl;
			}

		     	if ( pthread_join( mThread, NULL ) != 0 )
			{
				std::cerr << "pthread_join() failed. " << std::endl;
				exit(-1);
			}
		}

		pthread_t mThread;
		int32_t mStatus;
	};

	class _ThreadParameters
	{
		static XParameterType _dummy;

		public:
			_ThreadParameters( )
				: mGear( NULL )
				, mParameters( _dummy )
				, mID( 0 )
			{
				;
			}

			_ThreadParameters( gear * xGear
					  , XParameterType xParameters
					  , uint32_t xID )
				: mGear( xGear )
				, mParameters( xParameters )
				, mID( xID )
			{
				;
			}

			_ThreadParameters & operator=( const _ThreadParameters & xRhs )
			{
				mGear 	    = xRhs.mGear;
				mParameters = xRhs.mParameters;
				mID	    = xRhs.mID;
			}

			gear *		mGear;
			XParameterType 	mParameters;
			uint32_t 	mID;
	};

	std::function< XReturnType ( XParameterType const &) >  mLambda;

	std::deque< XReturnType > mResults;

	mutex mResultsMutex;

	typedef 	 std::unordered_map< uint32_t, _ThreadParameters > 		_ThreadParameterMap;
	typedef typename std::unordered_map< uint32_t, _ThreadParameters >::iterator 	_ThreadParameterMapIterator;

	typedef		 std::vector< _PThreadStatus > 		 _ThreadVector;
	typedef typename std::vector< _PThreadStatus >::iterator _ThreadVectorIterator;

	_ThreadParameterMap mThreadParameters;
	_ThreadVector 	    mThreads;

	bool mJoined;

	inline static uint32_t _nextThreadID( )
	{
		static uint32_t _threadIDCounter = 1; // 0 is reserved.
		return ++_threadIDCounter;
	}

	inline static void * _actualPosixThreadSink( void * xParameter )
	{
		void * result = NULL;

		if (xParameter == NULL)
		{
			std::cerr << "Null gear passed to pthread sink." << std::endl;
			exit( -1 );
		}

		_ThreadParameters * p = static_cast< _ThreadParameters * > ( xParameter );

		result = p->mGear->_actualPosixThreadWork(  p->mParameters );

		p->mGear->_deleteThreadParameter( p->mID );

		return result;
	}

	inline void * _actualPosixThreadWork( XParameterType xParameters )
	{
		void * returnValue = NULL;

		mResultsMutex.lock( );

		mResults.push_back( mLambda( xParameters ) );

		returnValue = static_cast< void * >( &mResults.back( ) );

		mResultsMutex.unlock( );

		return returnValue;
	}

	void _deleteThreadParameter( uint32_t const xID )
	{
		_ThreadParameterMapIterator it = mThreadParameters.find( xID );

		if ( it == mThreadParameters.end( ) )
		{
			std::cerr << "Thread parameter object not found where expected." << std::endl;
			exit( -1 );
		}

		mThreadParameters.erase( it );
	}

	public:
		gear( std::function< XReturnType ( XParameterType ) > xLambda )
			: mLambda( xLambda )
			, mResults( )
			, mThreadParameters( )
			, mThreads( )
			, mJoined( false )
		{
			;
		}

		~gear( )
		{
			join( );
		}

		void join( )
		{
			if ( !mJoined )
			{
				for ( _ThreadVectorIterator it = mThreads.begin(); it != mThreads.end(); ++it )
				{
					it->join();
				}

				mJoined = true;
			}
		}


		gear< XReturnType, XParameterType > & operator( )( XParameterType xParams )
		{
			if ( mJoined )
			{
				std::cerr << "Behavior of this operator is undefined after join( ) is called" << std::endl;
				exit( -1 );
			}


			mThreads.push_back( _PThreadStatus( ) );

			_PThreadStatus & status = mThreads.back( );

			uint32_t const nextThreadID = _nextThreadID( );

			mThreadParameters[ nextThreadID ] = _ThreadParameters( this, xParams, nextThreadID );

			_ThreadParameters  & pRef = mThreadParameters[ nextThreadID ];

			if ( mThreads.back( ).mStatus = pthread_create( &status.mThread
									, NULL
									, _actualPosixThreadSink
									, ( void *)( &pRef ) ) != 0 )
			{

				std::cerr << "Failed to create thread. " << mThreads.back( ).mStatus << std::endl;
				exit( -1 );
			}

			return *this; 
		}


		// NOT thread safe - call join() first before inspecting return values.
		XReturnType operator * ( )
		{
			if ( !mJoined )
			{
				std::cerr << "The behavior of this operator is undefined if the threads are not joined." << std::endl;
				exit( -1 );
			}

			XReturnType copy;
			mResultsMutex.lock( );
			copy = mResults.front( );
			mResults.pop_front( );
			mResultsMutex.unlock( );

			return copy;
		}

		int32_t size( )
		{
			if ( !mJoined )
			{
				std::cerr << "The behavior of size( ) is undefined if the threads are not joined." << std::endl;
				exit( -1 );
			}

			int32_t size;

			mResultsMutex.lock( );
			size = mResults.size( );
			mResultsMutex.unlock( );

			return size;
		}
};

template< typename XReturnType, typename XParameterType >
XParameterType gear<XReturnType, XParameterType>::_ThreadParameters::_dummy;

} // namespace sisu
#endif // THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_
