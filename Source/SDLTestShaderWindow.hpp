#ifndef SDL_TEST_SHADER_WINDOW_51593F130F8B4F6AB94CCCB70B576573_HPP_
#define SDL_TEST_SHADER_WINDOW_51593F130F8B4F6AB94CCCB70B576573_HPP_

#include "SDLTest.hpp"
#include "SDLShader.hpp"

namespace sisu {

class SDLTestShaderWindow : public SDLTestWindow
{
        protected:
                SDLShader mShader;

        public:
                SDLTestShaderWindow( ShaderSourcePair const & xShaderSources );
                SDLTestShaderWindow( ShaderPathPair const & xShaderPaths );
                ~SDLTestShaderWindow( );

                virtual void render( );

                virtual void run( );

                virtual void initialize( OpenGLAttributes const & xOpenGLAttributes );
};

} // namespace sisu

#endif // SDL_TEST_SHADER_WINDOW_51593F130F8B4F6AB94CCCB70B576573_HPP_
