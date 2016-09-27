#ifndef MOUSE_CFE633E4760D4F6C873F6FDACA7CE8D6_HPP_
#define MOUSE_CFE633E4760D4F6C873F6FDACA7CE8D6_HPP_

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

	typedef std::function<void(MouseEventInfo)> OnMouseEventCallback;

	class Mouse
	{
		mutex mM;
		std::vector< OnMouseEventCallback > mCallbacks;
		bool mInitialized;
		gear< uint32_t, int64_t > mMouseListener;
		event mQuit;

		MouseEventInfo mLast;

		inline bool _eventsAreDifferent( MouseEventInfo & xLhs, MouseEventInfo const & xRhs )
		{
			return !( xLhs.state      == xRhs.state      &&
			          xLhs.x          == xRhs.x          &&
			          xLhs.y          == xRhs.y          &&
			          xLhs.wheelMoved == xRhs.wheelMoved &&
			          xLhs.wheelUp    == xRhs.wheelUp );
		}

		public:
			Mouse( );

			void listen( int64_t const xSleepInterval = 0 );

			void registerCallback( OnMouseEventCallback xCallback );

			void stopListening( );
	};
} // namespace sisu

#endif // MOUSE_CFE633E4760D4F6C873F6FDACA7CE8D6_HPP_
