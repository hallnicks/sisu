#ifndef CURSOR_RENDERER_433941D4E95E44E097093C29375922FC_HPP_
#define CURSOR_RENDERER_433941D4E95E44E097093C29375922FC_HPP_

#ifdef WIN32
#include <Windows.h>
#elif defined(LINUX)
#include <X11/Xlib.h>
#endif

#include "DevILImage.hpp"
#include "Sprite.hpp"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

namespace sisu {

class CursorRenderer
{
	Sprite * mCursor;

#ifdef LINUX
	Display * mDisplay;
	bool mCursorGrabbed;
#endif
	public:
		CursorRenderer( )
			: mCursor( NULL )
#ifdef LINUX
			, mDisplay( XOpenDisplay( NULL ) )
			, mCursorGrabbed( false )
#endif
		{
			;
		}

		~CursorRenderer( )
		{
#ifdef LINUX
			if ( mCursorGrabbed )
			{

			}

			if ( mDisplay != NULL )
			{
				XCloseDisplay( mDisplay );
			}
#endif
		}

		void initialize( uint32_t const xW, uint32_t const xH, const char * xFilename )
		{
			DevILImage image( xFilename );

	                Sprite * pT = new Sprite( );

	                pT->texData = image.toGLTextureBuffer( );

	                pT->tex.initialize( ( pT->w = xW )
	                                  , ( pT->h = xH )
	                                  , ( uint8_t* )pT->texData );

			mCursor = pT;
#if 0 // Linux
			Window w;
			int revert;

			XGetInputFocus( mDisplay, &w, &revert );

			int result = GrabSuccess;

			if ( result = XGrabPointer( mDisplay
					 	  , w
						  , False
						  , 0
						  , GrabModeAsync
						  , GrabModeAsync
						  , w
						  , None
						  , CurrentTime ) != GrabSuccess )
			{
				std::cerr << "XGrabPointer failed!" << std::endl;
				exit( -1 );
			}

			mCursorGrabbed = true;
#endif
		}

		void drawCursor( Overlay2D * xOverlay, uint32_t const xX, uint32_t const xY )
		{
			if ( xOverlay == NULL )
			{
				std::cerr << "NULL overlay passed to " << __PRETTY_FUNCTION__ << std::endl;
				exit( -1 );
			}

			if ( mCursor == NULL )
			{
				std::cerr << "Cursor was NULL. Was drawCursor( .. ) called before initialize( .. )?" << std::endl;
				exit( -1 );
			}

			glActiveTexture( GL_TEXTURE4 );

                        xOverlay->drawTexture( mCursor->tex
                                      	     , glm::vec2( xX, xY )
                                             , glm::vec2( mCursor->w, mCursor->h )
      	                                     , 0.0f
               	                             , glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
		}

		void hideExternalCursor( )
		{
#ifdef WIN32
// On windows, the pointer is known to reappear in the window if alt-tab is performed (happens often in games such as 
// skyrim). Therefore, we hide it every frame. It is an inexpensive call.

			ShowCursor( FALSE );
			SetCursor( NULL );
#elif defined(LINUX)
// On linux, we only grab the pointer once and pray the window does not reset its pointer.
// TODO: Fix cursor hiding on linux! Need help from someone who understands X11 lib better.
#else
			std::cerr << __PRETTY_FUNCTION__ << " is not implemented on this platform." << std::endl;
			exit( -1 );
#endif

		}

};

} // namespace sisu;

#endif // CURSOR_RENDERER_433941D4E95E44E097093C29375922FC_HPP_
