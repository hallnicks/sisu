#include "mouse.hpp"

namespace sisu {

Mouse::Mouse( )
	: mM( )
	, mCallbacks( )
	, mMouseListener( [&]( int64_t xSleepIntervalNs )->uint32_t
	{
		// Ugly hack for mouse wheel.
		SDL_Event pollEvent;

		while ( !mQuit.isSet( ) )
		{
			if ( xSleepIntervalNs > 0 )
			{
				sleep::ns( xSleepIntervalNs );
			}

			int32_t x, y;

			uint32_t const state = SDL_GetMouseState( &x, &y );

			SDL_PollEvent( &pollEvent );

			MouseEventInfo const current = { state
						       , x
						       , y
						       , pollEvent.type == SDL_MOUSEWHEEL
						       , pollEvent.wheel.y < 0 };

			if ( _eventsAreDifferent( mLast, current ) )
			{
				mM.run([&]( )
				{
					for ( auto callback : mCallbacks )
					{
						callback( current );
					}
				});
			}

			mLast = current;
		}
	})
	, mQuit( )
{
	;
}

void Mouse::listen( int64_t const xSleepInterval )
{
	mMouseListener( xSleepInterval );
}

void Mouse::registerCallback( OnMouseEventCallback xCallback )
{
	mM.run([&]() { mCallbacks.push_back( xCallback ); });
}

void Mouse::stopListening( )
{
	mQuit.set( );
}

} // namespace sisu
