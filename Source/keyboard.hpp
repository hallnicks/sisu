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

			bool getKeyUp( )   const { return mKeyState == eKeyState_Up;   }
			bool getKeyDown( ) const { return mKeyState == eKeyState_Down; }

			uint16_t getScanCode( ) const { return mScanCode; }

			uint8_t operator[] ( SDL_Scancode const xIdx ) { return mState[ xIdx ]; }
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

			sScanCodesShift[ SDL_SCANCODE_A ] = 'a';
			sScanCodesShift[ SDL_SCANCODE_B ] = 'b';
			sScanCodesShift[ SDL_SCANCODE_C ] = 'c';
			sScanCodesShift[ SDL_SCANCODE_D ] = 'd';
			sScanCodesShift[ SDL_SCANCODE_E ] = 'e';
			sScanCodesShift[ SDL_SCANCODE_F ] = 'f';
			sScanCodesShift[ SDL_SCANCODE_G ] = 'g';
			sScanCodesShift[ SDL_SCANCODE_H ] = 'h';
			sScanCodesShift[ SDL_SCANCODE_I ] = 'i';
			sScanCodesShift[ SDL_SCANCODE_J ] = 'j';
			sScanCodesShift[ SDL_SCANCODE_K ] = 'k';
			sScanCodesShift[ SDL_SCANCODE_L ] = 'l';
			sScanCodesShift[ SDL_SCANCODE_M ] = 'm';
			sScanCodesShift[ SDL_SCANCODE_N ] = 'n';
			sScanCodesShift[ SDL_SCANCODE_O ] = 'o';
			sScanCodesShift[ SDL_SCANCODE_P ] = 'p';
			sScanCodesShift[ SDL_SCANCODE_Q ] = 'q';
			sScanCodesShift[ SDL_SCANCODE_R ] = 'r';
			sScanCodesShift[ SDL_SCANCODE_S ] = 's';
			sScanCodesShift[ SDL_SCANCODE_T ] = 't';
			sScanCodesShift[ SDL_SCANCODE_U ] = 'u';
			sScanCodesShift[ SDL_SCANCODE_V ] = 'v';
			sScanCodesShift[ SDL_SCANCODE_W ] = 'w';
			sScanCodesShift[ SDL_SCANCODE_X ] = 'x';
			sScanCodesShift[ SDL_SCANCODE_Y ] = 'y';
			sScanCodesShift[ SDL_SCANCODE_Z ] = 'z';



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
			if (xShiftPressed)
				std::cout << "Shift is pressed." << std::endl;
			return xShiftPressed ? sScanCodesShift[ xCode ] : sScanCodes[ xCode ];
		}



} sSDLKeyboardScancodeMap;

} // namespace sisu
#endif // KEYBOARD_FA51DF7F2A1D448BA605510668D180CE_HPP_
