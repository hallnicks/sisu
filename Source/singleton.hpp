#ifndef SINGLETON_146411A4F4C54C02962A9B406EB0AD49_HPP_
#define SINGLETON_146411A4F4C54C02962A9B406EB0AD49_HPP_

#include "threadgears.hpp"

namespace sisu {

template< class XClass > class Singleton;

template < class XClass >
class ISingleton
{
	ISingleton( )
	{
		// Additional runtime check for singleton.
		static bool sConstructorCalled = false;

		if ( sConstructorCalled )
		{
			std::cerr << "ISingleton constructor called more than once." << std::endl;
			exit( -1 );
		}

		sConstructorCalled = true;
	}

	friend class Singleton< XClass >;
	friend XClass;
};

template< class XClass >
class LockedSingleton;

template< class XClass >
class Singleton
{
	Singleton( ) { }

	static XClass sInstance;
	static bool sIsMultiThreaded;

	public:
		static XClass * get(  )
		{
			if (sIsMultiThreaded)
			{
				std::cerr << "Single-threaded access attempted to multi-threaded singleton" << std::endl;
				exit( -1 );
			}

			return & sInstance;
		}

	friend class LockedSingleton < XClass >;
};

template< class XClass>
XClass Singleton<XClass>::sInstance;

template< class XClass>
bool Singleton<XClass>::sIsMultiThreaded = false;

template< class XClass >
class LockedSingleton
{
	LockedSingleton( )
	{
		Singleton< XClass >::sIsMultiThreaded = true;
	}

	static mutex sM;

	public:
		static void doLocked( std::function<void(XClass*)> xLambda )
		{
			sM.lock( );
			xLambda( Singleton<XClass>::get( ) );
			sM.unlock( );
		}
};

template< class XClass>
mutex LockedSingleton<XClass>::sM;

} // namespace sisu

#endif // SINGLETON_146411A4F4C54C02962A9B406EB0AD49_HPP_

