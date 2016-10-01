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

	enum eKeyState
	{
		eKeyState_None = 0,
		eKeyState_Up,
		eKeyState_Down,
		eKeyState_Continue,
	};

	class KeyboardEvent
	{
		eKeyState const mKeyState;

		uint16_t const mScanCode;

		const uint8_t * mState;

		public:
			KeyboardEvent( eKeyState const xKeyState
				      , uint16_t const xScanCode
				      , const uint8_t * xState )
				: mKeyState( xKeyState )
				, mScanCode( xScanCode )
				, mState( xState )
			{
				;
			}

			bool getKeyUp( )       const { return mKeyState == eKeyState_Up;       }
			bool getKeyDown( )     const { return mKeyState == eKeyState_Down;     }
			bool getKeyContinue( ) const { return mKeyState == eKeyState_Continue; }

			uint16_t getScanCode( ) const { return mScanCode; }

			uint8_t operator[] ( SDL_Scancode const xIdx ) const { return mState[ xIdx ]; }
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

		        	                for ( uint16_t ii = 0; ii < SDL_NUM_SCANCODES; ++ii )
	        	        	        {
							eKeyState keyState = eKeyState_None;

							auto runCallbacks = [&]()
							{
								mM.run([&]( )
								{
									for ( auto callback : mCallbacks )
									{
										callback( KeyboardEvent( keyState, ii, state ) );
									}
								});
							};

		        	                        if ( state[ ii ] && !pressedLastTime[ ii ] )
		                	                {
								keyState = eKeyState_Down;
								runCallbacks( );
			                                }
							else if ( state[ ii ] && pressedLastTime[ ii ] )
							{
								keyState = eKeyState_Continue;
							}
		        	                        else if ( !state[ ii ] && pressedLastTime[ ii ] )
		                	                {
								keyState = eKeyState_Up;
								runCallbacks( );
			                                }

							/*
							if ( keyState != eKeyState_None )
							{
								runCallbacks( );
							}
							*/

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
	char sScanCodes	     [ SDL_NUM_SCANCODES ];
	char sScanCodesShift [ SDL_NUM_SCANCODES ];

	public:
		KeyboardScancodeMap( )
		{
			for ( int32_t ii = 0; ii < SDL_NUM_SCANCODES; ++ii )
			{
				sScanCodes      [ ii ] =
				sScanCodesShift [ ii ] = 0X00;
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

			sScanCodes[ SDL_SCANCODE_A ] = 'a';
			sScanCodes[ SDL_SCANCODE_B ] = 'b';
			sScanCodes[ SDL_SCANCODE_C ] = 'c';
			sScanCodes[ SDL_SCANCODE_D ] = 'd';
			sScanCodes[ SDL_SCANCODE_E ] = 'e';
			sScanCodes[ SDL_SCANCODE_F ] = 'f';
			sScanCodes[ SDL_SCANCODE_G ] = 'g';
			sScanCodes[ SDL_SCANCODE_H ] = 'h';
			sScanCodes[ SDL_SCANCODE_I ] = 'i';
			sScanCodes[ SDL_SCANCODE_J ] = 'j';
			sScanCodes[ SDL_SCANCODE_K ] = 'k';
			sScanCodes[ SDL_SCANCODE_L ] = 'l';
			sScanCodes[ SDL_SCANCODE_M ] = 'm';
			sScanCodes[ SDL_SCANCODE_N ] = 'n';
			sScanCodes[ SDL_SCANCODE_O ] = 'o';
			sScanCodes[ SDL_SCANCODE_P ] = 'p';
			sScanCodes[ SDL_SCANCODE_Q ] = 'q';
			sScanCodes[ SDL_SCANCODE_R ] = 'r';
			sScanCodes[ SDL_SCANCODE_S ] = 's';
			sScanCodes[ SDL_SCANCODE_T ] = 't';
			sScanCodes[ SDL_SCANCODE_U ] = 'u';
			sScanCodes[ SDL_SCANCODE_V ] = 'v';
			sScanCodes[ SDL_SCANCODE_W ] = 'w';
			sScanCodes[ SDL_SCANCODE_X ] = 'x';
			sScanCodes[ SDL_SCANCODE_Y ] = 'y';
			sScanCodes[ SDL_SCANCODE_Z ] = 'z';

			sScanCodes[ SDL_SCANCODE_MINUS        ] = '-';
			sScanCodes[ SDL_SCANCODE_EQUALS       ] = '=';
			sScanCodes[ SDL_SCANCODE_LEFTBRACKET  ] = '[';
			sScanCodes[ SDL_SCANCODE_RIGHTBRACKET ] = ']';
			sScanCodes[ SDL_SCANCODE_BACKSLASH    ] = '\\';
			sScanCodes[ SDL_SCANCODE_SEMICOLON    ] = ';';
			sScanCodes[ SDL_SCANCODE_APOSTROPHE   ] = '\'';
			sScanCodes[ SDL_SCANCODE_GRAVE        ] = '`';
			sScanCodes[ SDL_SCANCODE_COMMA        ] = ',';
			sScanCodes[ SDL_SCANCODE_PERIOD       ] = '.';
			sScanCodes[ SDL_SCANCODE_SLASH        ] = '/';


			sScanCodesShift[ SDL_SCANCODE_MINUS        ] = '_';
			sScanCodesShift[ SDL_SCANCODE_EQUALS       ] = '+';
			sScanCodesShift[ SDL_SCANCODE_LEFTBRACKET  ] = '{';
			sScanCodesShift[ SDL_SCANCODE_RIGHTBRACKET ] = '}';
			sScanCodesShift[ SDL_SCANCODE_BACKSLASH    ] = '|';
			sScanCodesShift[ SDL_SCANCODE_SEMICOLON    ] = ':';
			sScanCodesShift[ SDL_SCANCODE_APOSTROPHE   ] = '"';
			sScanCodesShift[ SDL_SCANCODE_GRAVE        ] = '~';
			sScanCodesShift[ SDL_SCANCODE_COMMA        ] = '<';
			sScanCodesShift[ SDL_SCANCODE_PERIOD       ] = '>';
			sScanCodesShift[ SDL_SCANCODE_SLASH        ] = '?';

			sScanCodesShift[ SDL_SCANCODE_0 ] = ')';
			sScanCodesShift[ SDL_SCANCODE_1 ] = '!';
			sScanCodesShift[ SDL_SCANCODE_2 ] = '@';
			sScanCodesShift[ SDL_SCANCODE_3 ] = '#';
			sScanCodesShift[ SDL_SCANCODE_4 ] = '$';
			sScanCodesShift[ SDL_SCANCODE_5 ] = '%';
			sScanCodesShift[ SDL_SCANCODE_6 ] = '^';
			sScanCodesShift[ SDL_SCANCODE_7 ] = '&';
			sScanCodesShift[ SDL_SCANCODE_8 ] = '*';
			sScanCodesShift[ SDL_SCANCODE_9 ] = '(';

			sScanCodesShift[ SDL_SCANCODE_A ] = 'A';
			sScanCodesShift[ SDL_SCANCODE_B ] = 'B';
			sScanCodesShift[ SDL_SCANCODE_C ] = 'C';
			sScanCodesShift[ SDL_SCANCODE_D ] = 'D';
			sScanCodesShift[ SDL_SCANCODE_E ] = 'E';
			sScanCodesShift[ SDL_SCANCODE_F ] = 'F';
			sScanCodesShift[ SDL_SCANCODE_G ] = 'G';
			sScanCodesShift[ SDL_SCANCODE_H ] = 'H';
			sScanCodesShift[ SDL_SCANCODE_I ] = 'I';
			sScanCodesShift[ SDL_SCANCODE_J ] = 'J';
			sScanCodesShift[ SDL_SCANCODE_K ] = 'K';
			sScanCodesShift[ SDL_SCANCODE_L ] = 'L';
			sScanCodesShift[ SDL_SCANCODE_M ] = 'M';
			sScanCodesShift[ SDL_SCANCODE_N ] = 'N';
			sScanCodesShift[ SDL_SCANCODE_O ] = 'O';
			sScanCodesShift[ SDL_SCANCODE_P ] = 'P';
			sScanCodesShift[ SDL_SCANCODE_Q ] = 'Q';
			sScanCodesShift[ SDL_SCANCODE_R ] = 'R';
			sScanCodesShift[ SDL_SCANCODE_S ] = 'S';
			sScanCodesShift[ SDL_SCANCODE_T ] = 'T';
			sScanCodesShift[ SDL_SCANCODE_U ] = 'U';
			sScanCodesShift[ SDL_SCANCODE_V ] = 'V';
			sScanCodesShift[ SDL_SCANCODE_W ] = 'W';
			sScanCodesShift[ SDL_SCANCODE_X ] = 'X';
			sScanCodesShift[ SDL_SCANCODE_Y ] = 'Y';
			sScanCodesShift[ SDL_SCANCODE_Z ] = 'Z';

			auto setIdx = [&]( SDL_Scancode xIdx, char const xC )
			{
				sScanCodes[ xIdx ] = sScanCodesShift [ xIdx ] = xC;
			};


			setIdx( SDL_SCANCODE_RETURN, '\n');
			setIdx( SDL_SCANCODE_SPACE,  ' ' );
			setIdx( SDL_SCANCODE_TAB,    '\t');

		}

		char resolveScanCode( uint16_t const xCode, bool const xShiftPressed )
		{
			return xShiftPressed ? sScanCodesShift[ xCode ] : sScanCodes[ xCode ];
		}



} sSDLKeyboardScancodeMap;

} // namespace sisu
#endif // KEYBOARD_FA51DF7F2A1D448BA605510668D180CE_HPP_
