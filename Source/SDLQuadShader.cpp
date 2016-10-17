#include "SDLQuadShader.hpp"

namespace sisu {

void SDLQuadShader::render( )
{
        SDLTestShaderWindow::render( );
        mShader( [&] ( )
        {
                glEnableVertexAttribArray( mVertexPos2DLocation );
                glBindBuffer( GL_ARRAY_BUFFER, mVBO );

                glVertexAttribPointer( mVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL );

                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
                glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

                glDisableVertexAttribArray( mVertexPos2DLocation );
        });
}

SDLQuadShader::SDLQuadShader( )                 // Vertex
        : SDLTestShaderWindow( ShaderSourcePair("#version 140\n"
                                                 "in vec2 LVertexPos2D;"
                                                 "void main() {"
                                                 " gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 );"
                                                 "}"
                                                // Fragment
                                               ,"#version 140\n"
                                                 "out vec4 LFragment;"
                                                 "void main() {"
                                                 " LFragment = vec4( 1.0, 1.0, 1.0, 1.0 );"
                                                 "}" ) )
        , mVertexPos2DLocation( -1 )
        , mVBO( 0 )
        , mIBO( 0 )
{
        ;
}

void SDLQuadShader::initialize( OpenGLAttributes const & xAttributes )
{
        SDLTestShaderWindow::initialize( xAttributes );

        // TODO: Automate these procedural things so it can be used like a key.
        // i.e myshaderProgram["LVertexPos2D"] = new value
        // structure should be map<GLint, std::string> or similar
        mVertexPos2DLocation = glGetAttribLocation( mShader.getProgramID( ), "LVertexPos2D" );

        if( mVertexPos2DLocation == -1 )
        {
                std::cerr << "GLSL program variable LVertexPOS2D not found." << std::endl;
                exit( -1 );
        }

        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );


        GLfloat vertexData[] =
                {
                        -0.5f, -0.5f,
                         0.5f, -0.5f,
                         0.5f,  0.5f,
                        -0.5f,  0.5f
                };

        GLuint indexData[] = { 0, 1, 2, 3 };

        glGenBuffers( 1, &mVBO );
        glBindBuffer( GL_ARRAY_BUFFER, mVBO );
        glBufferData( GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW );

        glGenBuffers( 1, &mIBO );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW );
}


} // namespace sisu
