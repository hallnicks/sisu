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

	class mouse_UT : public context
	{
		public:
			mouse_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};

	class MouseEvent
	{
		uint32_t const mEvent;
		public:
			MouseEvent( uint32_t const xEvent )
			: mEvent( xEvent )
			{
				;
			}
	};

	typedef std::function<void(MouseEvent)> OnMouseEventCallback;

	class Mouse
	{
		mutex mM;
		std::vector< OnMouseEventCallback > mCallbacks;
		bool mInitialized;
		gear<uint32_t, int64_t> mMouseListener;
		event mQuit;
		uint32_t  mLast;
		public:
			Mouse( )
				: mM( )
				, mCallbacks( )
				, mMouseListener( [&]( int64_t xSleepIntervalNs )->uint32_t
				{
					while ( !mQuit.isSet( ) )
					{
						if ( xSleepIntervalNs > 0 )
						{
							sleep::ns( xSleepIntervalNs );
						}

						uint32_t const state = SDL_GetMouseState( NULL, NULL );

						if ( mLast != state )
						{
							mM.run([&]( )
							{
								for ( auto callback : mCallbacks )
								{
									callback( MouseEvent( state ) );
								}
							});
						}

						mLast = state;
					}
				})
				, mQuit( )
			{
				;
			}

			void listen( int64_t const xSleepInterval = 0 )
			{
				mMouseListener( xSleepInterval );
			}

			void registerCallback( OnMouseEventCallback xCallback )
			{
				mM.run([&]() { mCallbacks.push_back( xCallback ); });
			}

			void stopListening( )
			{
				mQuit.set( );
			}
	};


} // namespace

TEST(mouse_UT, MouseHandlerCallback)
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

		OnMouseEventCallback  onMouseCharacter = [&] ( MouseEvent const & xEvent )
		{
			std::cout << "Got mouse event!" <<std::endl;
			/*
			if ( xEvent.isPressed( eMouseButton_Left )
			{
				std::cout << "Left click pressed. Exiting.." << std::endl;
				quit.set( );
			}
			*/
		};


		Mouse kb;

		kb.registerCallback(onMouseCharacter);
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
