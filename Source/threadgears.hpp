#ifndef THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_
#define THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_

#include <cstdlib>
#include <cstddef>
#include <pthread.h>
#include <iostream>
#include <sched.h>

namespace sisu
{
	enum eSchedPolicy { eFifo = 0, eRoundRobin = 1, eOther = 2 };

#define XPT(xPR,...)    if ( xPR(__VA_ARGS__) != 0 ) {\
                                std::cerr << #xPR << "(" << #__VA_ARGS__ << ")" << std::endl;\
                                exit( -1 );\
                        }

#define ARGS size_t xStackSize\
                , size_t xCpuSetSize\
                , size_t xGuardSize\
                , int xPriority\
                , eSchedPolicy xPolicy\
                , int xInheritScheduler\
                , bool xCreateDetached\
                , bool xSystemScope

#define PARAMS xStackSize\
                , xCpuSetSize\
                , xGuardSize\
                , xPriority\
                , eSchedPolicy xPolicy\
                , xInheritScheduler\
                , xCreateDetached\
                , xSystemScope

	template < typename XRet, typename XSink, typename XArg >
	class posixthread
	{
		struct thread_state
		{
        		pthread_t Handle;

		        pthread_attr_t Attrib;

	                thread_state(ARGS) : Attrib( ), Handle( 0 )
	                {
	                        cpu_set_t cpus;

	                        XPT(pthread_attr_setaffinity_np, &Attrib, xCpuSetSize, &cpus);

	                        XPT(pthread_attr_setdetachstate, &Attrib, xCreateDetached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);

	                        XPT(pthread_attr_setguardsize, &Attrib, xGuardSize);

	                        XPT(pthread_attr_setinheritsched, &Attrib, xInheritScheduler);

	                        sched_param const param = { xPriority };

	                        XPT(pthread_attr_setschedparam, &Attrib, &param);

	                        XPT(pthread_attr_setstacksize, &Attrib, xStackSize);

	                        int polices[3] = { SCHED_FIFO, SCHED_RR, SCHED_OTHER };

	                        XPT(pthread_attr_setschedpolicy, &Attrib, polices[static_cast<int>(xPolicy)]);

	                        XPT(pthread_attr_setscope, &Attrib, xSystemScope ? PTHREAD_SCOPE_SYSTEM : PTHREAD_SCOPE_PROCESS);

	                        XPT(pthread_attr_init, &Attrib);
        	        }

                	~thread_state( ) { XPT(pthread_attr_destroy, &Attrib); }
		} m;


		typedef XRet ( XSink::* XFEvent )( XArg );

		typedef void* (*ThreadStart)( void* );

		class XCB
		{
			XFEvent mXF;
			XSink & mSink;
			XArg & mArg;

			public:
				XCB( XSink & xSink, XFEvent xF, XArg & xArg )
					: mSink( xSink ), mXF( xF ), mArg( xArg ) { }

				XRet operator ( )( void ) { return ( mSink.*mXF )( mArg ); }
		} mXCB;

		static void * startRoutine( void * xObject )
		{
			static_cast< XCB * >( xObject )( );
			return static_cast<void*>( 0 );
		}

		static const unsigned int DefStackSize = 2048;
		static const size_t DefCpuSetSize = sizeof( cpu_set_t );
		static const size_t DefaultGuardSize;

		public:
			posixthread( XSink & 	xSink
					, XFEvent 	xF
					, XArg & 	xArg
					, unsigned int 	xStackSize		= DefStackSize
                			, unsigned int 	xCpuSetSize      	= sizeof( cpu_set_t )
                			, unsigned int 	xGuardSize       	= 0
                			, int 		xPriority               = 1
	                		, eSchedPolicy 	xPolicy          	= eFifo
	                		, int 		xInheritScheduler       = 1
	                		, bool 		xCreateDetached         = false
	                		, bool 		xSystemScope            = false )

				: mXCB( xSink, xF, xArg )

				, m( xStackSize
					, xCpuSetSize
					, xGuardSize
					, xPriority
					, xPolicy
					, xInheritScheduler
					, xCreateDetached
					, xSystemScope )
				{
					;
				}

			~posixthread( ) { join( ); }

			void detach( ) { XPT(pthread_detach, &m.Handle); }

			posixthread & operator ( )( void )
			{
				XPT(pthread_create, &m.Handle, &m.Attrib, &startRoutine, &mXCB);
				return *this;
			}

			void join( ) { XPT(pthread_join, m.Handle, NULL); }

			bool isDetached( )
			{
				int detachState;
				XPT(pthread_attr_getdetachstate, &m.Attrib, &detachState);
				return (detachState == PTHREAD_CREATE_DETACHED);
			}

			bool isSystemScoped( )
			{
        			int scope;
				XPT(pthread_attr_getscope, &m.Attrib, &scope);
			        return (scope == PTHREAD_SCOPE_SYSTEM);
			}

			cpu_set_t getAffinityNp( )
			{
				cpu_set_t t;
				size_t s;
				XPT(pthread_attr_getaffinity_np, &m.Attrib, &s, &t);
				return t;
			}

			unsigned int getGuardSize( )
			{
				cpu_set_t t;
			        size_t s;
			        XPT(pthread_attr_getaffinity_np, &m.Attrib, &s, &t);
			        return t;
			}

			eSchedPolicy getSchedPolicy( )
			{
				eSchedPolicy e;
				int p;
				XPT(pthread_attr_getschedpolicy, &m.Attrib, &p);
				switch ( p )
				{
			                case SCHED_FIFO:        { e = eFifo;            } break;
			                case SCHED_FIFO:        { e = eRoundRobin;      } break;
			                default:                { e = eOther;           } break;
			        }
			        return e;
			}

			unsigned int getStackSize( )
			{
				size_t size;
				XPT(pthread_attr_getstacksize, &m.Attrib, &size);
				return size;
			}
	};

} // namespace sisu

#endif // THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_
