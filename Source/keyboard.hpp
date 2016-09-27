#ifndef KEYBOARD_FA51DF7F2A1D448BA605510668D180CE_HPP_
#define KEYBOARD_FA51DF7F2A1D448BA605510668D180CE_HPP_
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

		const uint8_t * mState;

		public:
			KeyboardEvent( bool const xKeyUp
				      , uint16_t const xScanCode
				      , const uint8_t * xState )
				: mKeyUp( xKeyUp )
				, mScanCode( xScanCode )
				, mState( xState )
			{
				;
			}

			bool getKeyUp( ) const { return mKeyUp; }

			uint16_t getScanCode( ) const { return mScanCode; }

			bool operator[] ( SDL_Scancode const xIdx ) { return (bool)mState[ xIdx ]; }
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
										callback( KeyboardEvent( !pressedLastTime[ii], ii, state ) );
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



static class KeyboardScancodeMap
{
	char sScanCodes[ SDL_NUM_SCANCODES ];

	public:
		KeyboardScancodeMap( )
		{
			for ( int32_t ii = 0; ii < SDL_NUM_SCANCODES; ++ii )
			{
				sScanCodes[ ii ] = ' ';
			}

			sScanCodes[ SDL_SCANCODE_0 ] = '0';
			sScanCodes[ SDL_SCANCODE_1 ] = '1';
			sScanCodes[ SDL_SCANCODE_2 ] = '2';
			sScanCodes[ SDL_SCANCODE_3 ] = '3';
			sScanCodes[ SDL_SCANCODE_4 ] = '4';
			sScanCodes[ SDL_SCANCODE_5 ] = '5';
			sScanCodes[ SDL_SCANCODE_6 ] = '6';
			sScanCodes[ SDL_SCANCODE_7 ] = '7';
			sScanCodes[ SDL_SCANCODE_8 ] = '8';
			sScanCodes[ SDL_SCANCODE_9 ] = '9';

			sScanCodes[ SDL_SCANCODE_A ] = 'A';
			sScanCodes[ SDL_SCANCODE_B ] = 'B';
			sScanCodes[ SDL_SCANCODE_C ] = 'C';
			sScanCodes[ SDL_SCANCODE_D ] = 'D';
			sScanCodes[ SDL_SCANCODE_E ] = 'E';
			sScanCodes[ SDL_SCANCODE_F ] = 'F';
			sScanCodes[ SDL_SCANCODE_G ] = 'G';
			sScanCodes[ SDL_SCANCODE_H ] = 'H';
			sScanCodes[ SDL_SCANCODE_I ] = 'I';
			sScanCodes[ SDL_SCANCODE_J ] = 'J';
			sScanCodes[ SDL_SCANCODE_K ] = 'K';
			sScanCodes[ SDL_SCANCODE_L ] = 'L';
			sScanCodes[ SDL_SCANCODE_M ] = 'M';
			sScanCodes[ SDL_SCANCODE_N ] = 'N';
			sScanCodes[ SDL_SCANCODE_O ] = 'O';
			sScanCodes[ SDL_SCANCODE_P ] = 'P';
			sScanCodes[ SDL_SCANCODE_Q ] = 'Q';
			sScanCodes[ SDL_SCANCODE_R ] = 'R';
			sScanCodes[ SDL_SCANCODE_S ] = 'S';
			sScanCodes[ SDL_SCANCODE_T ] = 'T';
			sScanCodes[ SDL_SCANCODE_U ] = 'U';
			sScanCodes[ SDL_SCANCODE_V ] = 'V';
			sScanCodes[ SDL_SCANCODE_W ] = 'W';
			sScanCodes[ SDL_SCANCODE_X ] = 'X';
			sScanCodes[ SDL_SCANCODE_Y ] = 'Y';
			sScanCodes[ SDL_SCANCODE_Z ] = 'Z';
		}

		char operator[]( uint16_t const xCode )
		{
			return sScanCodes[ xCode ];
		}


} sSDLKeyboardScancodeMap;

} // namespace sisu
#endif // KEYBOARD_FA51DF7F2A1D448BA605510668D180CE_HPP_
