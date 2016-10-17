#ifndef SDL_QUAD_SHADER_79898C0D00B34F4B839795F00A519000_HPP_
#define SDL_QUAD_SHADER_79898C0D00B34F4B839795F00A519000_HPP_

#include "SDLTestShaderWindow.hpp"

namespace sisu {

class SDLQuadShader : public SDLTestShaderWindow
{
        GLint mVertexPos2DLocation;

        GLuint mVBO, mIBO;

        protected:
                virtual void render( );
        public:
                SDLQuadShader( );

                virtual void initialize( OpenGLAttributes const & xAttributes );
};

} // namespace sisu

#endif // SDL_QUAD_SHADER_79898C0D00B34F4B839795F00A519000_HPP_

