#if 0
#ifndef THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_
#define THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_

#include <sched.h>

namespace threadgears
{
	enum eSchedPolicy { eFifo = 0, eRoundRobin = 1, eOther = 2 };

	class posix;

	template < typename XRet, typename XSink, typename XArg, typename XImpl >
	class osthread : thread
	{
		XImpl * impl

		typedef XRet(XSink::*XFEvent)(XArg);

		typedef void*(*ThreadStart)(void*);

		class XCB
		{
			XFEvent mXF;
			XSink & mSink;
			XArg & mArg;

			public:
				XCB(XSink & xSink, XFEvent xF, XArg & xArg)
					: mSink( xSink ), mXF( xF ), mArg( xArg ) { }

				XRet operator ( )( void ) { return (mSink.*mXF)(mArg); }
		} mXCB;

		static void * startRoutine( void * xObject )
		{
			static_cast< XCB * >( xObject )( );
			return static_cast<void*>(0);
		}

		static const unsigned int DefStackSize = 2048;
		static const size_t DefCpuSetSize = sizeof(cpu_set_t);
		static const size_t DefaultGuardSize;

#define ARGS size_t xStackSize        	= DefStackSize\
		, size_t xCpuSetSize    = sizeof( cpu_set_t )\
		, size_t xGuardSize     = 0\
		, int xPriority         = 1\
		, eSchedPolicy xPolicy  = eFifo\
		, int xInheritScheduler = 1\
		, bool xCreateDetached  = false\
		, bool xSystemScope     = false

		static inline thread_state * initState(ARGS);

		public:
			thread( XSink & xSink, XFEvent xF, XArg & xArg, ARGS)
	                        : mXCB( xSink, xF, xArg ), m( new XImpl(ARGS) ) { }
#undef ARGS

			~thread( ) { delete m; }

			void detach( ) { XImpl->detach( ); }

			thread & operator ( )( void ) { XImpl->( ); }

			void join( ) { XImpl->join( ); }

			bool isDetached( ) { return XImpl->isDetached( ); }

			bool isSystemScoped( ) { return XImpl->isSystemScoped( ); }

			cpu_set_t getAffinityNp( ) { return XImpl->getAffinityNp( ); }

			size_t getGuardSize( ) { return XImpl->getGuardSize( ); }

			eSchedPolicy getSchedPolicy( )
			{

			}

			size_t getStackSize( )
			{

			}

	};
} // namespace threadgears

#endif // THREADGEARS_72F99E37_FA22_494C_BAC4_87EF12b6351A_HPP_
#endif
