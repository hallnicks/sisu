#include "threadgears.hpp"

namespace sisu {

mutex::mutex( const pthread_mutexattr_t * xAttributes ) 
  : mMutex ( )
{ 
	if ( pthread_mutex_init( &mMutex, xAttributes ) != 0 )
	{ 
		std::cerr << "pthread_mutex_init failed." << std::endl; 
		exit( -1 );
	}
}

mutex::~mutex( )
{
	lock( );

	if ( pthread_mutex_destroy( &mMutex ) != 0 )
	{
		std::cerr << "pthread_mutex_destroy failed." << std::endl;
		exit( -1 );
	}
}

void mutex::lock( )
{
	if ( pthread_mutex_lock( &mMutex ) != 0 )
	{
		std::cerr << "pthread_mutex_lock failed." << std::endl;
		exit( -1 );
	}
}

void mutex::unlock( )
{
	if ( pthread_mutex_unlock( &mMutex ) != 0 )
	{
		std::cerr << "pthread_mutex_lock failed." << std::endl;
		exit( -1 );
	}
}

void mutex::run( std::function<void(void)> xLambda )
{
	lock( );
	xLambda( );
	unlock( );
}

int sleep::_sleep( timespec * const xDuration )
{
	int32_t const sleepResult = nanosleep( xDuration, NULL );
	if ( sleepResult != 0 )
	{
		std::cerr << "Thread Sleep interrupted." << sleepResult << ", " << errno << std::endl;
		std::cerr << "Paramteter SS was " << xDuration->tv_sec << std::endl;
		std::cerr << "Paramteter NS was " << xDuration->tv_nsec << std::endl;
	}
}

void sleep::ms( int64_t const xMilliseconds ) 
{
	timespec duration;

	int64_t const nanoseconds = xMilliseconds * 1000000L;

	duration.tv_sec = nanoseconds / 1000000000ULL;
	duration.tv_nsec = nanoseconds - ( duration.tv_sec * 1000000000ULL );  
	_sleep( &duration );
}

void sleep::ns( int64_t const xNanoseconds ) 
{
	timespec duration;

	duration.tv_sec = 0;
	duration.tv_nsec = xNanoseconds;

	_sleep( &duration );
}

} // namespace sisu

