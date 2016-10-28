#include "SDLColorSwitcher.hpp"
#include "AndroidLogWrapper.hpp"

namespace sisu {

void SDLColorSwitcher::render( )
{
	uint32_t const flags = SDL_GetWindowFlags( mMainWindow );

	if ( ! ( flags & SDL_WINDOW_BORDERLESS ) )
	{
		SISULOG( "Window wasn't borderless!" );
		exit(-1);
	}

	if ( ! ( flags & SDL_WINDOW_MAXIMIZED ) )
	{
		SDL_MaximizeWindow( mMainWindow );
		SDL_RaiseWindow( mMainWindow );
		SDL_SetWindowGrab( mMainWindow, SDL_TRUE );
	}

// TODO: Bug? Flag is never set on linux.
#ifdef WIN32
	if ( ! ( flags & SDL_WINDOW_INPUT_FOCUS) )
	{
		SISULOG( "Failed to get window focus." );
		exit( -1 );
	}
#endif

	glClearColor( _randomGLfloat( )
		    , _randomGLfloat( )
		    , _randomGLfloat( )
		    , _randomGLfloat( ) );

	glClear( GL_COLOR_BUFFER_BIT );

	SDL_GL_SwapWindow( mMainWindow );
}

SDLColorSwitcher::SDLColorSwitcher( )
	: SDLTestWindow( )
{
	;
}

void SDLColorSwitcher::run( )
{
	for ( int ii = 0; ii < 250; ++ii )
	{
		render( );
	}

	_hide( );
}

} // namespace sisu
