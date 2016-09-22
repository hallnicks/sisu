#include "SDLShaderTest.hpp"

namespace sisu {

void SDLShader::initialize( )
{
	if ( mVertexShaderSource.compare( "" ) == 0 )
	{
		std::cerr << "Vertex Shader source was invalid." << std::endl;
		std::cerr << "mVertexShaderSource = " 		 << mVertexShaderSource << std::endl;
		exit( -1 );
	}

	if ( mFragmentShaderSource.compare( "" ) == 0 )
	{
		std::cerr << "Fragment Shader source was invalid." << std::endl;
		std::cerr << "mFragmentShaderSource = " 	   << mFragmentShaderSource << std::endl;
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

SDLShader::SDLShader( ShaderSourcePair const & xShaderSources )
	: mProgramID( 0 )
	, mVertexShaderSource( xShaderSources.VertexSource )
	, mFragmentShaderSource( xShaderSources.FragmentSource )
{
	;
}

SDLShader::SDLShader( ShaderPathPair const & xShaderPaths )
	: mProgramID( 0 )
	, mVertexShaderSource( fileToString( xShaderPaths.VertexPath ) )
	, mFragmentShaderSource( fileToString(  xShaderPaths.FragmentPath ) )
{
	;
}

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

SDLShader::~SDLShader( ) 
{
	glDeleteProgram( mProgramID );
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

SDLShader & SDLShader::operator( )( std::function<void(void)> xLambda )
{
	glUseProgram( mProgramID );
	xLambda( );
	glUseProgram( 0 );
	return *this;
}

SDLTestShaderWindow & SDLTestShaderWindow::operator( )( std::function<void(void)> xLambda )
{
	mShader( xLambda );
	return *this;
}

void SDLTestShaderWindow::initialize( OpenGLAttributes const & xOpenGLAttributes )
{
	SDLTestWindow::initialize( xOpenGLAttributes );

	mShader.initialize( );

	_checkForGLError( );
}

void SDLQuadShader::render( )
{
	SDLTestShaderWindow::render( );
	(*this)( [&] ( )
	{
		glEnableVertexAttribArray( mVertexPos2DLocation );
		glBindBuffer( GL_ARRAY_BUFFER, mVBO );

		glVertexAttribPointer( mVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
		glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

		glDisableVertexAttribArray( mVertexPos2DLocation );
	});
}

SDLQuadShader::SDLQuadShader( ) 				 // Vertex
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
