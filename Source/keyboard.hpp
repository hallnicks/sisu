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


namespace sisu {

	class KeyboardEvent
	{
		bool const mKeyUp;
		uint16_t const mScanCode;

		public:
			KeyboardEvent( bool const xKeyUp
				      , uint16_t const xScanCode )
				: mKeyUp( xKeyUp )
				, mScanCode( xScanCode )
			{
				;
			}

			bool getKeyUp( ) const { return mKeyUp; }

			uint16_t getScanCode( ) const { return mScanCode; }
	};

	typedef std::function<void(KeyboardEvent)> OnKeyboardEventCallback;

	class Keyboard
	{
		mutex mM;
		event mQuit;
		std::vector< OnKeyboardEventCallback > mCallbacks;
		bool mInitialized;
		gear<uint32_t, int64_t> mKeyboardListener;

		public:
			Keyboard( )
				: mM( )
				, mQuit( )
				, mCallbacks( )
				, mInitialized( false )
				, mKeyboardListener( [ & ] ( int64_t const xSleepIntervalNs ) -> uint32_t
		  		{
			                bool pressedLastTime[ SDL_NUM_SCANCODES ];

					for ( uint16_t ii = 0; ii < SDL_NUM_SCANCODES; ++ii )
					{
						pressedLastTime [ ii ] = false;
					}

					while ( !mQuit.isSet( ) )
					{
						if ( xSleepIntervalNs > 0 )
						{
							sleep::ns( xSleepIntervalNs );
						}

						const Uint8 * state = SDL_GetKeyboardState( NULL );

		        	                for ( uint32_t ii = 0; ii < SDL_NUM_SCANCODES; ++ii )
	        	        	        {
							auto runCallbacks = [&]()
							{
								mM.run([&]( )
								{
									for ( auto callback : mCallbacks )
									{
										callback( KeyboardEvent( !pressedLastTime[ii], ii ) );
									}
								});
							};

		        	                        if ( state[ ii ] && !pressedLastTime[ ii ] )
		                	                {
								runCallbacks( );
			                                }
		        	                        else if ( !state[ ii ] && pressedLastTime[ ii ] )
		                	                {
								runCallbacks( );
			                                }

			                                pressedLastTime[ ii ] = state[ ii ];
						}
					}
				})
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


} // namespace sisu
