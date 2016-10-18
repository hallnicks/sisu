#ifdef POPEN_UT
#include "test.hpp"

#include "threadgears.hpp"
#include "keyboard.hpp"

#include <SDL2/SDL.h>
#include <windows.h>

#include "ChildProcessController.hpp"

using namespace sisu;

namespace {

class popen_UT : public context
{
	public:
		popen_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(popen_UT, popenPipedFromSDLKeyboard)
{
	{
		Keyboard kb;
		event quit;

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
	                                                SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS    );

		SDL_GLContext context = SDL_GL_CreateContext( window );

		SDL_GL_SetSwapInterval( 1 );

		SDL_RaiseWindow( window );

		SDL_SetWindowGrab( window, SDL_TRUE );

		std::vector<char> accum;

		kb.registerCallback( [&]( KeyboardEvent const & xEvent )
		{
	                bool const shiftPressed = xEvent[ SDL_SCANCODE_LSHIFT  ] || xEvent[ SDL_SCANCODE_RSHIFT  ];
	                bool const enterPressed = xEvent[ SDL_SCANCODE_RETURN  ] || xEvent[ SDL_SCANCODE_RETURN2 ];

			if ( !xEvent.getKeyUp( ) )
			{
				if ( !enterPressed )
				{
					char const c = sSDLKeyboardScancodeMap.resolveScanCode( xEvent.getScanCode( ), shiftPressed );

					std::cout << c;

					if ( c == '\b' )
					{
						std::cout << ' ';
					}

					accum.push_back( c );
				}
				else
				{
					accum.push_back('\0');

					if ( strcmp( accum.data( ), "quit" ) == 0 )
					{
						quit.set( );
					}
					else
					{
						ChildProcessController<4096> childProcess;

						childProcess.initialize( );

						auto onReceivePipeData = []( uint8_t * xBuffer, uint64_t xSize )
						{
							DWORD dwWritten;

							HANDLE parentStdOut = GetStdHandle( STD_OUTPUT_HANDLE );

							if ( !WriteFile( parentStdOut, xBuffer, xSize, &dwWritten, NULL ) )
							{
								std::cerr << "WriteFile( .. ) failed." << std::endl;
								exit( -1 );
							}
						};

						if ( !childProcess.runCommand( accum.data( ), onReceivePipeData ) )
						{
							std::cerr << " !! Invalid command: " << accum.data( ) << std::endl;
						}
					}

					accum.clear( );

					std::cout << std::endl <<  "[ sisu shell ] $ ";
				}
			}

			fflush( stdout );
		});

		kb.listen( );

		std::cout << std::endl <<  "[ sisu shell ] $ ";

		while ( !quit.isSet( ) )
		{
			SDL_PumpEvents( );
			SDL_GL_SwapWindow( window );
		}

		SDL_DestroyWindow( window );
		SDL_Quit( );

		kb.stopListening( );
	}
}
#endif // POPEN_UT
