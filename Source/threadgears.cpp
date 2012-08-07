#ifndef THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_
#define THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_

#include <pthread.h>

#include <cstdlib>
#include <iostream>

namespace threadgears
{

#define XPT(xPR,...) 	if ( xPR(__VA_ARGS__) != 0 )\
			{\
				std::cerr << #xPR << "(" << #__VA_ARGS__ << ")" << std::endl;\
				exit( -1 );\
			}

	template < typename XRet, typename XSink, typename XArg >
	class thread
	{
		typedef XRet(XSink::*XFEvent)(XArg);
		typedef void*(*ThreadStart)(void*);

		pthread_attr_t mAttributes;

		pthread_t mHandle;

		class XCB
		{
			XFEvent mFunction;

			XSink & mSink;

			XArg & mArg;

			public:
				XCB( XSink & xSink, XFEvent xF, XArg & xArg )
					: mSink( xSink ), mFunction( xF ), mArg( xArg ) { }

				XRet operator ( )( void ) { return ( mSink.*mFunction )( mArg ); }

		} mXCB;

		static void * startRoutine( void * xObject )
		{
			static_cast< XCB * >( xObject )( );
			return NULL;
		}

		public:
			enum eSchedPolicy { eFifo = SCHED_FIFO, eRoundRobin = SCHED_RR, eOther = SCHED_OTHER };

			thread( XSink & xSink
				, XFEvent xF
				, XArg & xArg
				, size_t xStackSize 	= 2048
				, size_t xCpuSetSize 	= sizeof( cpu_set_t )
				, size_t xGuardSize	= 0
				, int xPriority 	= 1
				, eSchedPolicy xPolicy  = eFifo
				, int xInheritScheduler = 1
				, bool xCreateDetached 	= false
				, bool xSystemScope 	= false )
					: mAttributes( 0 )
					, mHandle( 0 )
					, mXCB( xSink, xF, xArg )
			{
				XPT(pthread_attr_init, &mAttributes);

				cpu_set_t cpus;

				XPT(pthread_attr_setaffinity_np, &mAttributes, xCpuSetSize, &cpus);

				XPT(pthread_attr_setdetachstate, &mAttributes, xCreateDetached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);

				XPT(pthread_attr_setguardsize, &mAttributes, xGuardSize);

				XPT(pthread_attr_setinheritsched, &mAttributes, xInheritScheduler);

				struct sched_param { int sched_priority; } param = { xPriority };

				XPT(pthread_attr_setschedparam, &mAttributes, &param);

				XPT(pthread_attr_setstacksize, &mAttributes, xStackSize);

				XPT(pthread_attr_setschedpolicy, &mAttributes, static_cast<int>(xPolicy));

				XPT(pthread_attr_setscope, &mAttributes, xSystemScope ? PTHREAD_SCOPE_SYSTEM : PTHREAD_SCOPE_PROCESS);
			}

			~thread( ) { join( ); }

			bool isDetached( )
			{
				int detachState;

				XPT(pthread_attr_getdetachstate, &mAttributes, &detachState);

				return (detachState == PTHREAD_CREATE_DETACHED);
			}

			thread & operator ( ) ( void )
			{
				XPT(pthread_create, &mHandle, &mAttributes, &startRoutine, &mXCB);
				return *this;
			}

			cpu_set_t getAffinityNp( )
			{
				cpu_set_t t;

				size_t s;

				XPT(pthread_attr_getaffinity_np, &mAttributes, &s, &t);

				return t;
			}

			size_t getGuardSize( )
			{
				size_t guardSize;

				XPT(pthread_attr_getguardsize, &mAttributes, &guardSize);

				return guardSize;
			}

			size_t getStackSize( )
			{
				size_t size;

				XPT(pthread_attr_getstacksize, &mAttributes, &size);

				return size;
			}

			eSchedPolicy getSchedPolicy( )
			{
				eSchedPolicy e;

				int p;

				XPT(pthread_attr_getschedpolicy, &mAttributes, &p);

				switch ( p )
				{
					case SCHED_FIFO: 	{ e = eFifo; 		} break;
					case SCHED_FIFO: 	{ e = eRoundRobin; 	} break;
					default: 		{ e = eOther; 		} break;
				}

				return e;
			}

			bool isSystemScoped( )
			{
				int scope;

				XPT(pthread_attr_getscope, &mAttributes, &scope);

				return (scope == PTHREAD_SCOPE_SYSTEM);
			}


			void detach( ) { XPT(pthread_detach, &mHandle); }

			void join( ) { XPT(pthread_join, mHandle, NULL); }
	};

#undef XPT

} // namespace threadgears

#endif // THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_
