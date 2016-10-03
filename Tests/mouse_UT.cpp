#if 0
#include "test.hpp"
#include "mouse.hpp"

using namespace sisu;

namespace {

	class mouse_UT : public context
	{
		public:
			mouse_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
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

			auto resolveClickStateName = []( eClickState const xState )->const char *
			{

				switch ( xState )
				{
					case eClickState_Up:
						return "Up";

					case eClickState_Down:
						return "Down";

					case eClickState_Continue:
						return "Continue";

					case eClickState_None:
						return "None";

					default:
						return "Invalid";
				}
			};


			std::cout 
				  << "left: "   << ( resolveClickStateName( xEvent.leftState   ) ) << " "
				  << "middle: " << ( resolveClickStateName( xEvent.middleState ) ) << " "
				  << "right: "  << ( resolveClickStateName( xEvent.rightState  ) ) << " "
				  << ( xEvent.wheelHasMovedUp( )      ? ", wheel up "     : "" )
				  << ( xEvent.wheelHasMovedDown( )    ? ", wheel down "   : "" )
				  << " ]"
				  << std::endl;



			if ( xEvent.leftState   == eClickState_Continue &&
			     xEvent.middleState == eClickState_Continue &&
			     xEvent.rightState  == eClickState_Continue )
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
