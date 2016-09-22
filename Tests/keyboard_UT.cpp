#if 0
#include "test.hpp"
#include "threadgears.hpp"

#include <string>
#include <iostream>
#include <functional>
#include <conio.h>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <SDL.h>

using namespace sisu;

namespace {

	class keyboard_UT : public context
	{
		public:
			keyboard_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};

	class KeyboardEvent
	{
		const Uint8 * mEvent;

		public:
			KeyboardEvent( const Uint8 * xEvent )
			: mEvent( xEvent )
			{
				;
			}

			bool operator[]( uint16_t const xScanCode ) const
			{
				return mEvent[ xScanCode ];
			}
	};

	typedef std::function<void(KeyboardEvent)> OnKeyboardEventCallback;

	class Keyboard
	{
		mutex mM;
		std::vector< OnKeyboardEventCallback > mCallbacks;
		bool mInitialized;
		gear<uint32_t, int64_t> mKeyboardListener;
		event mQuit;

		public:
			Keyboard( )
				: mM( )
				, mCallbacks( )
				, mKeyboardListener( [&]( int64_t xSleepIntervalNs )->uint32_t
				{
					while ( !mQuit.isSet( ) )
					{
						if ( xSleepIntervalNs > 0 )
						{
							sleep::ns( xSleepIntervalNs );
						}

						const Uint8 * state = SDL_GetKeyboardState( NULL );

						mM.run([&]( )
						{
							for ( auto callback : mCallbacks )
							{
								callback( KeyboardEvent( state ) );
							}
						});
					}
				})
				, mQuit( )
			{
				;
			}


			void listen( int64_t const xSleepInterval = 0 )
			{
				mKeyboardListener( xSleepInterval );
			}

			void registerCallback( OnKeyboardEventCallback xCallback )
			{
				mM.run([&]() { mCallbacks.push_back( xCallback ); });
			}

			void stopListening( )
			{
				mQuit.set( );
			}
	};


} // namespace

TEST(keyboard_UT, KeyboardHandlerCallback)
{
	{
		if ( SDL_Init ( SDL_INIT_VIDEO ) < 0 )
		{

			std::cerr << "SDL_Init ( .. ) failed: " << SDL_GetError( ) <<std::endl;
			exit( -1 );
		}

		SDL_Window * window = SDL_CreateWindow( "SDL2 OpenGL"
                                    			, SDL_WINDOWPOS_CENTERED
		                                        , SDL_WINDOWPOS_CENTERED
		                                        , 1
		                                        , 1
		                                        , SDL_WINDOW_OPENGL      | SDL_WINDOW_BORDERLESS    |
		                                          SDL_WINDOW_SHOWN       | SDL_WINDOW_INPUT_GRABBED |
                		                          SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS );

		SDL_GLContext context = SDL_GL_CreateContext( window );

		SDL_GL_SetSwapInterval( 1 );

		SDL_MaximizeWindow( window );
		SDL_RaiseWindow( window );
		SDL_SetWindowGrab( window, SDL_TRUE );

		event quit;

		OnKeyboardEventCallback  onKeyboardCharacter = [&] ( KeyboardEvent const & xEvent )
		{
			if ( xEvent[SDL_SCANCODE_Q] )
			{
				std::cout << "Q pressed. Exiting.." << std::endl;
				quit.set( );
			}
		};


		Keyboard kb;

		kb.registerCallback(onKeyboardCharacter);
		kb.listen( );

		while ( !quit.isSet( ) )
		{
			SDL_PumpEvents( );
			SDL_GL_SwapWindow( window );
		}

		kb.stopListening( );

		SDL_DestroyWindow(window);

		SDL_Quit( );

	}
}

#endif
