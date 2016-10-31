#include "Mouse.hpp"
#include "AndroidLogWrapper.hpp"

namespace sisu {

Mouse::Mouse( )
	: mM( )
	, mCallbacks( )
	, mMouseListener( [&]( int64_t xSleepIntervalNs )->uint32_t
	{
		// Ugly hack for mouse wheel.
		SDL_Event pollEvent;

		SDL_Event pollEvents[25];

		while ( !mQuit.isSet( ) )
		{
			if ( xSleepIntervalNs > 0 )
			{
				sleep::ns( xSleepIntervalNs );
			}

			int32_t x, y;

			uint32_t const state = SDL_GetMouseState( &x, &y );

			bool const leftPressed   = state & SDL_BUTTON( SDL_BUTTON_LEFT   );
			bool const middlePressed = state & SDL_BUTTON( SDL_BUTTON_MIDDLE );
			bool const rightPressed  = state & SDL_BUTTON( SDL_BUTTON_RIGHT  );

			auto resolveClickState = [&]( bool const xPressed, bool const xPressedLastTime )
			{
				eClickState state = eClickState_None;

				if ( xPressed && xPressedLastTime )
				{
					state = eClickState_Continue;
				}
				else if ( xPressed && !xPressedLastTime )
				{
					state = eClickState_Down;
				}
				else if ( !xPressed && xPressedLastTime )
				{
					state = eClickState_Up;
				}

				return state;
			};

			eClickState leftClickState   = resolveClickState( leftPressed,   mPressedLast[ 0 ] )
				  , middleClickState = resolveClickState( middlePressed, mPressedLast[ 1 ] )
                                  , rightClickState  = resolveClickState( rightPressed,  mPressedLast[ 2 ] );

			mPressedLast[ 0 ] = leftPressed;
			mPressedLast[ 1 ] = middlePressed;
			mPressedLast[ 2 ] = rightPressed;

			SDL_PollEvent( &pollEvent );

			MouseEventInfo const current = { state
						       , x
						       , y
						       , pollEvent.type == SDL_MOUSEWHEEL
						       , pollEvent.wheel.y < 0
						       , pollEvent.wheel.y
						       , leftClickState
						       , middleClickState
						       , rightClickState };

			if ( _eventsAreDifferent( mLast, current )    ||
			     leftClickState   == eClickState_Continue ||
			     middleClickState == eClickState_Continue ||
			     rightClickState  == eClickState_Continue  )
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
	, mPressedLast( )
{
	SISULOG( "In Mouse Ctor" );
	for ( size_t ii = 0; ii < sizeof(mPressedLast)/sizeof(bool); ++ii )
	{
		mPressedLast[ ii ] = false;
	}
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

