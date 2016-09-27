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

	struct MouseEventInfo
	{
		uint32_t state;

		int32_t x, y;

		bool wheelMoved
		   , wheelUp;

		void operator=( MouseEventInfo const & xOther )
		{
			state 	   = xOther.state;
			x     	   = xOther.x;
			y     	   = xOther.y;
			wheelMoved = xOther.wheelMoved;
			wheelUp    = xOther.wheelUp;
		}

		bool isLeftClickPressed( )   const { return state & SDL_BUTTON(SDL_BUTTON_LEFT);   }
		bool isMiddleClickPressed( ) const { return state & SDL_BUTTON(SDL_BUTTON_MIDDLE); }
		bool isRightClickPressed( )  const { return state & SDL_BUTTON(SDL_BUTTON_RIGHT);  }

		bool wheelHasMovedUp( )   const { return wheelMoved && wheelUp;   }
		bool wheelHasMovedDown( ) const { return wheelMoved && !wheelUp;  }
	};

	bool operator!=( MouseEventInfo & xLhs, MouseEventInfo const & xRhs )
	{
		return !( xLhs.state      == xRhs.state      &&
		          xLhs.x          == xRhs.x          &&
		          xLhs.y          == xRhs.y          &&
		          xLhs.wheelMoved == xRhs.wheelMoved &&
		          xLhs.wheelUp    == xRhs.wheelUp );
	}

	typedef std::function<void(MouseEventInfo)> OnMouseEventCallback;

	class Mouse
	{
		mutex mM;
		std::vector< OnMouseEventCallback > mCallbacks;
		bool mInitialized;
		gear< uint32_t, int64_t > mMouseListener;
		event mQuit;

		MouseEventInfo mLast;
		public:
			Mouse( )
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

						if ( mLast != current )
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
		                                        , 1920/2
		                                        , 1080/2
		                                        , SDL_WINDOW_OPENGL      | SDL_WINDOW_BORDERLESS    |
		                                          SDL_WINDOW_SHOWN       | SDL_WINDOW_INPUT_GRABBED |
                		                          SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS );

		SDL_GLContext context = SDL_GL_CreateContext( window );

		SDL_GL_SetSwapInterval( 1 );

		SDL_RaiseWindow( window );
		SDL_SetWindowGrab( window, SDL_TRUE );

		event quit;

		OnMouseEventCallback  onMouseCharacter = [&] ( MouseEventInfo const & xEvent )
		{
			std::ios::fmtflags const f( std::cout.flags( ) );

			std::cout << "[ Mouse: "
				  << xEvent.x
				  << ", "
				  << xEvent.y
				  << " "
				  << std::hex
				  << xEvent.state;

			std::cout.flags( f );

			std::cout << ( xEvent.isLeftClickPressed( )   ? ", left click "   : "" )
				  << ( xEvent.isMiddleClickPressed( ) ? ", middle click " : "" )
				  << ( xEvent.isRightClickPressed( )  ? ", right click "  : "" )
				  << ( xEvent.wheelHasMovedUp( )      ? ", wheel up "     : "" )
				  << ( xEvent.wheelHasMovedDown( )    ? ", wheel down "   : "" )
				  << " ]"
				  << std::endl;

			if ( xEvent.isLeftClickPressed( ) && xEvent.isMiddleClickPressed( ) && xEvent.isRightClickPressed( ) )
			{
				std::cout << "All three buttons pressed. Exiting.." << std::endl;
				quit.set( );
			}

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
