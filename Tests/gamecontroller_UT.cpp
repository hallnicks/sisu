#if 0
#include "test.hpp"
#include "threadgears.hpp"
#include "word.hpp"

#include <string>
#include <iostream>
#include <functional>
#include <conio.h>
#include <map>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL.h>

using namespace sisu;

namespace {

	class gameController_UT : public context
	{
		public:
			gameController_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};

	union GameControllerEventInfo
	{
		int64_t internal_state;

		struct _GameControllerEventInfo
		{
			void setButton( int8_t const xSDL_CONTROLLER_BUTTON )
			{
				std::cout << __PRETTY_FUNCTION__ << " is not implemented." << std::endl;
				//exit( -1 );
			}
		} data;
	};

	typedef std::function<void(GameControllerEventInfo)> OnGameControllerEventCallback;

	class GameController
	{
		mutex mM;
		event mQuit;
		gear<uint32_t, int64_t > mGamePadListener;
                std::vector< OnGameControllerEventCallback >	mCallbacks;
		//std::vector< SDL_GameController * > 		mControllers;
		std::map< uint32_t, SDL_GameController * > 	mControllers;

		int64_t mLast;

		static constexpr SDL_GameControllerButton sButtons[ SDL_CONTROLLER_BUTTON_MAX ] = { SDL_CONTROLLER_BUTTON_A
							 					  , SDL_CONTROLLER_BUTTON_B
												  , SDL_CONTROLLER_BUTTON_X
												  , SDL_CONTROLLER_BUTTON_Y
												  , SDL_CONTROLLER_BUTTON_BACK
												  , SDL_CONTROLLER_BUTTON_GUIDE
												  , SDL_CONTROLLER_BUTTON_START
												  , SDL_CONTROLLER_BUTTON_LEFTSTICK
												  , SDL_CONTROLLER_BUTTON_RIGHTSTICK
												  , SDL_CONTROLLER_BUTTON_LEFTSHOULDER
												  , SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
												  , SDL_CONTROLLER_BUTTON_DPAD_UP
												  , SDL_CONTROLLER_BUTTON_DPAD_DOWN
												  , SDL_CONTROLLER_BUTTON_DPAD_LEFT
												  , SDL_CONTROLLER_BUTTON_DPAD_RIGHT };

		public:
			GameController( )
				: mM( )
				, mQuit( )
				, mGamePadListener( [ & ] ( int64_t xSleepIntervalNs )
				  {
					do
					{
						// Identify and open all game controllers.
						for ( int32_t ii = 0; ii < SDL_NumJoysticks( ); ++ii )
						{
							// TODO: Support Joysticks, exotic controllers supported by SDL.
							if ( mControllers.find( ii ) == mControllers.end( ) &&
							     SDL_IsGameController( ii ) )
							{
								SDL_GameController * controller = NULL;

								if ( ( controller = SDL_GameControllerOpen( ii ) ) == NULL )
								{
									std::cout << "SDL_GameControllerOpen( " << ii << " ) failed: " << SDL_GetError( ) << std::endl;
									exit( -1 );
								}

								std::cout << "Found controller." << std::endl;

								mControllers[ ii ] = controller;
							}
						}

						for ( auto && controllerPair : mControllers )
						{
							GameControllerEventInfo eventInfo = { 0 };

							for ( SDL_GameControllerButton ii = SDL_CONTROLLER_BUTTON_A;
								 		       ii < SDL_CONTROLLER_BUTTON_MAX;
										       ii = ( SDL_GameControllerButton )( ( int32_t )ii + 1 ) )
							{
								if ( SDL_GameControllerGetButton( controllerPair.second, ii ) )
								{
									std::cout << "Button press! " << ii << std::endl;
									eventInfo.data.setButton( ii );
								}
							}

							if ( eventInfo.internal_state != mLast )
							{
                                                	        mM.run([&]( )
                                        	                {
                                	                                for ( auto callback : mCallbacks )
                        	                                        {
                	                                                        callback( eventInfo );
        	                                                        }
	                                                        });

								mLast = eventInfo.internal_state;
							}
						}

					} while ( !mQuit.isSet( ) );

					return 0;
				  } )
				, mCallbacks( )
				, mControllers( )
				, mLast( 0 )

			{
				;
			}

			~GameController( )
			{
				for ( auto && ii : mControllers )
				{
					SDL_GameControllerClose( ii.second );
				}
			}

			//TODO: repeated code should go in base class for keyboard, mouse and gamepad controller. 
			void listen( int64_t const xSleepInterval = 0 )
			{
				mGamePadListener( xSleepInterval );
			}

			void registerCallback( OnGameControllerEventCallback xCallback )
			{
                                mM.run([&]() { mCallbacks.push_back( xCallback ); });
			}

			void stopListening( )
			{
				mQuit.set( );
			}
	};
} // namespace


#if 0
TEST(gameController_UT, SetGameControllerFlags)
{
	GameControllerEventInfo info = { 0 };

	std::cout << info.internal_state << std::endl;
	std::cout << word<int64_t>( info.state )<< std::endl;

	info.state = 1;

	std::cout << info.internal_state << std::endl;
	std::cout << word<int64_t>( info.state ) << std::endl;

	info.state = 0;

	#if 0
	for ( int32_t ii = 0; ii < SDL_CONTROLLER_BUTTON_MAX; ++ii )
	{
		info.setButton( ii );

		word<int64_t> bytes( info.internal_state );

		std::cout << info.internal_state << std::endl;
		std::cout << bytes << std::endl;
		std::cout << "deref: " << *bytes << std::endl;
	}
	#endif



	BLOCK_EXECUTION;
}
#endif

TEST(gameController_UT, GameControllerHandlerCallback)
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

		OnGameControllerEventCallback  onGameControllerUpdate = [&] ( GameControllerEventInfo const & xEvent )
		{
			std::ios::fmtflags const f( std::cout.flags( ) );

			std::cout << "[ GameController: ";

			std::cout.flags( f );

			std::cout
				  //<< ( xEvent.isLeftClickPressed( )   ? ", left click "   : "" )
				  << " ]"
				  << std::endl;
		};


		GameController gc;

		gc.registerCallback(onGameControllerUpdate);
		gc.listen( );

		while ( !quit.isSet( ) )
		{
			SDL_GameControllerUpdate( );
			SDL_PumpEvents( );
			SDL_GL_SwapWindow( window );
		}

		gc.stopListening( );

		SDL_DestroyWindow(window);

		SDL_Quit( );

	}
}
#endif
