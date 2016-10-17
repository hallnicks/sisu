#ifndef SDL_SHADER_B8AD752008C64DC4A63B98A080A9E05E_HPP_
#define SDL_SHADER_B8AD752008C64DC4A63B98A080A9E05E_HPP_

#ifndef OPENGLES
#include <GL/glew.h>
#else
#include <GLES2/gl2.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <functional>

namespace sisu {

struct ShaderSourcePair
{
        ShaderSourcePair( const char * xV, const char* xF )
                : VertexSource( xV )
                , FragmentSource( xF )
        {
                ;
        }

        char const * const VertexSource;
        char const * const FragmentSource;
};

struct ShaderPathPair
{
        ShaderPathPair( const char * xV, const char * xF )
                : VertexPath( xV )
                , FragmentPath( xF )
        {
                ;
        }

        char const * const VertexPath;
        char const * const FragmentPath;
};

class SDLShader
{
        std::string mVertexShaderSource
                  , mFragmentShaderSource;

        protected:
                GLuint mProgramID;

        public:
                SDLShader( ShaderSourcePair const & xShaderSources );
                SDLShader( ShaderPathPair const & xShaderPaths );
                ~SDLShader( );

                class Uniforms
                {
                        GLuint const mProgramID;

                        Uniforms( GLuint const xProgramID )
                                : mProgramID( xProgramID )
                        {
                                ;
                        }

                        friend class SDLShader;

                        public:
                                GLint operator [ ] ( const char * xName )
                                {
                                        return glGetUniformLocation( mProgramID, xName );
                                }

                                void setUniform1i( const char * xName, GLint const xValue )
                                {
                                        glUniform1i( (*this)[ xName ], xValue );
                                }

                                void setUniform1f( const char * xName, GLfloat const xValue )
                                {
                                        glUniform1f( (*this)[ xName ], xValue );
                                }

                                void setUniformMatrix4fv( const char * xName, glm::mat4 const & xMatrix )
                                {
                                        glUniformMatrix4fv( (*this)[ xName ], 1, GL_FALSE, glm::value_ptr( xMatrix ) );
                                }

                                void setUniformVector3f( const char * xName, glm::vec3 const & xValue )
                                {
                                        glUniform3f( (*this)[ xName ], xValue.x, xValue.y, xValue.z );
                                }
                };

                virtual void initialize( );

                virtual void operator( )( std::function<void(void)> xLambda );

                Uniforms getUniforms() { return Uniforms( mProgramID ); }

                GLuint getProgramID( ) const { return mProgramID; }

}; // class SDLShader


} // namespace sisu

#endif // SDL_SHADER_B8AD752008C64DC4A63B98A080A9E05E_HPP_

