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

#include "keyboard.hpp"
#include "Stopwatch.hpp"

using namespace sisu;

namespace {

	class keyboard_UT : public context
	{
		public:
			keyboard_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
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
		                                        , 1920 / 2
		                                        , 1080 / 2
		                                        , SDL_WINDOW_OPENGL      | SDL_WINDOW_BORDERLESS    |
		                                          SDL_WINDOW_SHOWN       | SDL_WINDOW_INPUT_GRABBED |
                		                          SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS );

		SDL_GLContext context = SDL_GL_CreateContext( window );

		SDL_GL_SetSwapInterval( 1 );

		//SDL_MaximizeWindow( window );
		SDL_RaiseWindow( window );
		SDL_SetWindowGrab( window, SDL_TRUE );

		event quit;

		OnKeyboardEventCallback onKeyboardCharacter = [&] ( KeyboardEvent const & xEvent )
		{
			std::cout << "[ Key " << ( xEvent.getKeyUp( ) ? "up " : "down " ) << ": "
				  << std::hex << xEvent.getScanCode( ) << "]" << std::endl;

			if ( xEvent.getScanCode( ) == SDL_SCANCODE_Q )
			{
				std::cerr << "Q pressed. Exiting.. " << std::endl;
				quit.set( );
			}
		};


		Keyboard kb;

		kb.registerCallback( onKeyboardCharacter );
		kb.listen( );

		double accum = 0.0;


		while ( !quit.isSet( ) )
		{
			Stopwatch t;

			t.startMs( );

			SDL_PumpEvents( );

			SDL_GL_SwapWindow( window );

			if ( ( accum += t.stop( ) ) > 3000.0 )
			{
				std::cerr << "Timeout reached. Exiting.." << std::endl;
				break;
			}
		}

		kb.stopListening( );

		SDL_DestroyWindow(window);

		SDL_Quit( );

	}
}
#endif
