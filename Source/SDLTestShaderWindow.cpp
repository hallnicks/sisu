#include "SDLTestShaderWindow.hpp"

namespace sisu {

SDLTestShaderWindow::SDLTestShaderWindow( ShaderSourcePair const & xShaderSources )
        : SDLTestWindow( )
        , mShader( xShaderSources )
{
        ;
}

SDLTestShaderWindow::SDLTestShaderWindow( ShaderPathPair const & xShaderPaths )
        : SDLTestWindow( )
        , mShader( xShaderPaths )
{
        ;
}

SDLTestShaderWindow::~SDLTestShaderWindow( )
{
        ;
}

void SDLTestShaderWindow::render( )
{
        glClear( GL_COLOR_BUFFER_BIT );
}

void SDLTestShaderWindow::run( )
{
        for ( int ii = 0; ii < 500; ++ii )
        {
                render( );
                SDL_GL_SwapWindow( mMainWindow );
        }
}

void SDLTestShaderWindow::initialize( OpenGLAttributes const & xOpenGLAttributes )
{
        SDLTestWindow::initialize( xOpenGLAttributes );
        mShader.initialize( );
        _checkForGLError( );
}



} // namespace sisu
