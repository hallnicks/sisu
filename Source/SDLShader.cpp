#include "SDLShader.hpp"
#include "ioassist.hpp"
#include "AndroidLogWrapper.hpp"

#include <iostream>

namespace sisu {

static inline void _printGLProgramLog( GLuint const xProgram )
{
        if ( !glIsProgram( xProgram ) )
        {
                std::cerr << "Parameter was not a valid GL program." << std::endl;
                exit( -1 );
        }

        int32_t logLength = 0, maxLength = 0;

        glGetProgramiv( xProgram, GL_INFO_LOG_LENGTH, &maxLength );

        char * infoLog = new char[ maxLength ];

        glGetProgramInfoLog( xProgram, maxLength, &logLength, infoLog );

        if( logLength > 0 )
        {
                std::cout << infoLog << std::endl;
        }

        delete[] infoLog;
}

static inline void _printGLShaderLog( GLuint const xShader )
{
        if ( !glIsShader( xShader ) )
        {
                std::cerr << "Parameter was not a valid GL shader." << std::endl;
                exit( -1 );
        }

        int32_t logLength = 0, maxLength = 0;

        glGetShaderiv( xShader, GL_INFO_LOG_LENGTH, &maxLength );

        char * infoLog = new char[ maxLength ];

        glGetShaderInfoLog( xShader, maxLength, &logLength, infoLog );

        if( logLength > 0 )
        {
                std::cout << infoLog << std::endl;
        }

        delete[] infoLog;
}


SDLShader::SDLShader( ShaderSourcePair const & xShaderSources )
        : mProgramID( 0 )
        , mVertexShaderSource( xShaderSources.VertexSource )
        , mFragmentShaderSource( xShaderSources.FragmentSource )
{
        SISULOG( "In SDLShader Ctor" );
}

SDLShader::SDLShader( ShaderPathPair const & xShaderPaths )
        : mProgramID( 0 )
        , mVertexShaderSource( fileToString( xShaderPaths.VertexPath ) )
        , mFragmentShaderSource( fileToString(  xShaderPaths.FragmentPath ) )
{
        SISULOG( "In SDLShader Ctor" );
}

SDLShader::~SDLShader( )
{
        if ( mProgramID != 0 )
        {
                glDeleteProgram( mProgramID );
        }
}

void SDLShader::initialize( )
{
        if ( mVertexShaderSource.compare( "" ) == 0 )
        {
                std::cerr << "Vertex Shader source was invalid." << std::endl;
                std::cerr << "mVertexShaderSource = "            << mVertexShaderSource << std::endl;
                exit( -1 );
        }

        if ( mFragmentShaderSource.compare( "" ) == 0 )
        {
                std::cerr << "Fragment Shader source was invalid." << std::endl;
                std::cerr << "mFragmentShaderSource = "            << mFragmentShaderSource << std::endl;
                exit( -1 );
        }

        mProgramID = glCreateProgram( );

        auto compileShader = []( GLenum const xShaderType, const GLchar * xSource ) -> GLuint
        {
                GLuint const shader = glCreateShader( xShaderType );

                glShaderSource( shader, 1, &xSource, NULL );

                glCompileShader( shader );

                GLint shaderCompiled = GL_FALSE;

                glGetShaderiv( shader, GL_COMPILE_STATUS, &shaderCompiled );

                if ( shaderCompiled != GL_TRUE )
                {
                        std::cout << "Failed to compile shader: " << std::endl << xSource << std::endl;

                        _printGLShaderLog( shader );

                        exit( -1 );
                }

                return shader;
        };

        GLuint vertexShader = 0, fragmentShader = 0;

        glAttachShader( mProgramID, vertexShader = compileShader( GL_VERTEX_SHADER, mVertexShaderSource.c_str( ) ) );

        glAttachShader( mProgramID, fragmentShader = compileShader( GL_FRAGMENT_SHADER, mFragmentShaderSource.c_str( ) ) );

        glLinkProgram( mProgramID );

        GLint programSuccess = GL_FALSE;

        glGetProgramiv( mProgramID, GL_LINK_STATUS, &programSuccess );

        if ( programSuccess != GL_TRUE )
        {
                std::cerr << "Failed to link shader." << std::endl;

                _printGLProgramLog( mProgramID );

                exit( -1 );
        }

        glDeleteShader( vertexShader );
        glDeleteShader( fragmentShader );
}

void SDLShader::operator( )( std::function<void(void)> xLambda )
{
        glUseProgram( mProgramID );
        xLambda( );
        glUseProgram( 0 );
}

} // namespace sisu
