#if 0
#include "threadgears.hpp"

#include <pthread.h>

#include <cstdlib>
#include <iostream>

namespace threadgears
{

#define TMPL template < typename XRet, typename XSink, typename XArg >

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

class thread_state
{
	pthread_t Handle;

	pthread_attr_t Attrib;

	public:
		thread_state(ARGS) : Attrib( ), Handle( 0 )
		{
		        cpu_set_t cpus;

		        XPT(pthread_attr_setaffinity_np, &Attrib, xCpuSetSize, &cpus);

		        XPT(pthread_attr_setdetachstate, &Attrib, xCreateDetached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);

		        XPT(pthread_attr_setguardsize, &Attrib, xGuardSize);

		        XPT(pthread_attr_setinheritsched, &Attrib, xInheritScheduler);

		        struct sched_param { int sched_priority; } const param = { xPriority };

		        XPT(pthread_attr_setschedparam, &Attrib, &param);

		        XPT(pthread_attr_setstacksize, &Attrib, xStackSize);

		        int polices[3] = { SCHED_FIFO, SCHED_RR, SCHED_OTHER };

		        XPT(pthread_attr_setschedpolicy, &Attrib, polices[static_cast<int>(xPolicy)]);

		        XPT(pthread_attr_setscope, &Attrib, xSystemScope ? PTHREAD_SCOPE_SYSTEM : PTHREAD_SCOPE_PROCESS);

			XPT(pthread_attr_init, &Attrib);
		}

		~thread_state( ) { XPT(pthread_attr_destroy, &Attrib); }
};

TMPL
inline thread_state * thread::initState(ARGS) { return new thread_state(PARAMS); }

TMPL
thread::~thread( )
{
	join( );

	delete m;
}

bool thread::isDetached( )
{
	int detachState;

//	XPT(pthread_attr_getdetachstate, &m->Attrib, &detachState);

	return (detachState == PTHREAD_CREATE_DETACHED);
}

thread & operator thread::( )( void )
{
//	XPT(pthread_create, &m->Handle, &m->Attrib, &startRoutine, &mXCB);

	return *this;
}

cpu_set_t thread::getAffinityNp( )
{
	cpu_set_t t;

	size_t s;

//	XPT(pthread_attr_getaffinity_np, &m->Attrib, &s, &t);

	return t;
}

size_t thread::getGuardSize( )
{
	size_t guardSize;

//	XPT(pthread_attr_getguardsize, &m->Attrib, &guardSize);

	return guardSize;
}

size_t thread::getStackSize( )
{
	size_t size;

//	XPT(pthread_attr_getstacksize, &m->Attrib, &size);

	return size;
}

eSchedPolicy thread::getSchedPolicy( )
{
	eSchedPolicy e;

	int p;

//	XPT(pthread_attr_getschedpolicy, &m->Attrib, &p);

	switch ( p )
	{
		case SCHED_FIFO: 	{ e = eFifo; 		} break;
		case SCHED_FIFO: 	{ e = eRoundRobin; 	} break;
		default: 		{ e = eOther; 		} break;
	}

	return e;
}

bool thread::isSystemScoped( )
{
	int scope;

//	XPT(pthread_attr_getscope, &m->Attrib, &scope);

	return (scope == PTHREAD_SCOPE_SYSTEM);
}


TMPL
void thread::detach( )
{
//	XPT(pthread_detach, &m->Handle);
}

void thread::join( )
{
//	XPT(pthread_join, m->Handle, NULL);
}

} // namespace threadgears
#endif
