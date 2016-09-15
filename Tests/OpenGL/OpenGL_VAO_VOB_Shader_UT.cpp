#if 0
#include "test.hpp"
#include <string>
#include <iostream>

#ifndef __linux__
#include <Windows.h>
#endif


#include <SDL2/SDL.h>

/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, GLEW, standard IO, and strings
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glut.h>
#include <stdio.h>

#include <iostream>
#include <fstream>

// Windows specific: Uncomment the following line to open a console window for debug output
#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

// Simple OpenGL GLM/SDL demo
// Renders a rotating pyramid, showing 3 of 4 sides (allowing back faces to be seen)

// Using the gl3.h header (from http://www.opengl.org/registry/) can limit code to the core profile only
// GLEW is required on Windows but prob not on Linux or OSX Lion. On those platforms you should
// uncomment the next two lines and then comment out the following line. 
//#define GL3_PROTOTYPES 1
//#include <GL3/gl3.h>
#include <GL/glew.h>

#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{

class OpenGL_UT : public context
{
	public:
		OpenGL_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace


GLuint shaderprogram;
GLuint vao, vbo[2]; 
float r = 0;


const char * loadFile( const char * xFilename )
{
	int size;
	char * memblock;

	std::ifstream file( xFilename, std::ios::in | std::ios::binary | std::ios::ate );

	if ( file.is_open() )
	{
		size = (int) file.tellg( ); // get location of file pointer i.e. file size
		memblock = new char[ size+1 ]; // create buffer with space for null char
		file.seekg( 0, std::ios::beg );
		file.read( memblock, size );
		file.close( );
		memblock[ size ] = 0;
		std::cout << "file " << xFilename << " loaded" << std::endl;
	}
	else
	{
		std::cout << "Unable to open file " << xFilename << std::endl;
		exit(-1);
	}

	return memblock;
}


// Something went wrong - print SDL error message and quit
void exitFatalError( const char * xMessage )
{
	std::cout << xMessage << " ";
	std::cout << SDL_GetError( );
	SDL_Quit();
	exit( -1 );
}


// Set up rendering context
void setupRC( SDL_Window * & window, SDL_GLContext &context)
{
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) // Initialize video
	{
	        exitFatalError("Unable to initialize SDL"); 
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
 
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );  // double buffering on

	// Turn on x4 multisampling anti-aliasing (MSAA)
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4 );
 
    	window = SDL_CreateWindow( "SDL VAO Shader Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				   1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS );
	if ( window == NULL ) 
	{
        	exitFatalError("Unable to create window");
	}
 
	SDL_MaximizeWindow( window );
	SDL_RaiseWindow( window );
	SDL_SetWindowGrab( window, SDL_TRUE ); 

        uint32_t const flags = SDL_GetWindowFlags( window );

	if ( ! ( flags & SDL_WINDOW_INPUT_FOCUS) )
	{
		std::cout << "Failed to get window focus." << std::endl;
		exit( -1 );
	}

    	context = SDL_GL_CreateContext( window ); // Create opengl context and attach to window
 
   	SDL_GL_SetSwapInterval( 1 ); // set swap buffers to sync with monitor's vertical refresh rate
}

// printShaderError
// Display (hopefully) useful error messages if shader fails to compile or link
void printShaderError(GLint shader)
{
	int maxLength = 0;
	int logLength = 0;
	GLchar *logMessage;

	// Find out how long the error message is
	if (glIsShader(shader))
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
	else
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	if (maxLength > 0) // If message has length > 0
	{
		logMessage = new GLchar[maxLength];
		if (glIsShader(shader))
			glGetProgramInfoLog(shader, maxLength, &logLength, logMessage);
		else
			glGetShaderInfoLog(shader,maxLength, &logLength, logMessage);
		std::cout << "Shader Info Log:" << std::endl << logMessage << std::endl;
		delete [] logMessage;
	}
}


GLuint initShaders(const char * vertFile, const char *fragFile)
{
	GLuint p, f, v;	// Handles for shader program & vertex and fragment shaders

	v = glCreateShader(GL_VERTEX_SHADER); // Create vertex shader handle
	f = glCreateShader(GL_FRAGMENT_SHADER);	// " fragment shader handle

	const char *vertSource = loadFile(vertFile); // load vertex shader source
	const char *fragSource = loadFile(fragFile);  // load frag shader source
	
	// Send the shader source to the GPU
	// Strings here are null terminated - a non-zero final parameter can be
	// used to indicate the length of the shader source instead
	glShaderSource(v, 1, &vertSource,0);
	glShaderSource(f, 1, &fragSource,0);
	
	GLint compiled, linked; // return values for checking for compile & link errors

	// compile the vertex shader and test for errors
	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cout << "Vertex shader not compiled." << std::endl;
		printShaderError(v);
	} 

	// compile the fragment shader and test for errors
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cout << "Fragment shader not compiled." << std::endl;
		printShaderError(f);
	} 
	
	p = glCreateProgram(); 	// create the handle for the shader program
	glAttachShader(p,v); // attach vertex shader to program
	glAttachShader(p,f); // attach fragment shader to program

	glBindAttribLocation( p, 0, "in_Position" ); // bind position attribute to location 0
	glBindAttribLocation( p, 1, "in_Color"    ); // bind color    attribute to location 0

	glLinkProgram(p); // link the shader program and test for errors
	glGetProgramiv(p, GL_LINK_STATUS, &linked);

    	if(!linked)
	{
		std::cout << "Program not linked." << std::endl;
		printShaderError(p);
	}

	glUseProgram(p);  // Make the shader program the current active program

	delete [] vertSource; // Don't forget to free allocated memory
	delete [] fragSource; // We allocated this in the loadFile function...

	return p; // Return the shader program handle
}

void initGLscene( void )
{
	const GLfloat pyramid[15] = {     // a simple pyramid
		 0.0,  0.5,  0.0, // top
		-1.2, -0.5,  1.0, // front bottom left
		 1.0, -0.5,  1.0, // front bottom right
		 1.0, -0.5, -1.0, // back bottom right
		-1.2, -0.5, -1.0  }; // back bottom left

	const GLfloat colors[15] = {
		0.0 ,  0.4 ,  0.33, 
		1.0 ,  0.8 ,  0.66, 
		0.44,  0.9 ,  0.77, 
		0.66,  0.25,  1.0 , 
		1.0 ,  0.6 ,  0.88 }; 


	shaderprogram = initShaders( "resources/simple.vert","resources/simple.frag" ); // Create and start shader program
	glGenVertexArrays( 1, &vao ); // allocate & assign a Vertex Array Object (VAO)
	glBindVertexArray( vao ); // bind VAO as current object
	glGenBuffers( 2, vbo ); // allocate two Vertex Buffer Objects (VBO)

    	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] ); // bind first VBO as active buffer object
    	// Copy the vertex data from diamond to our VBO
    	// 15 * sizeof(GLfloat) is the size of the pyramid array, since it contains 15 GLfloat values 
    	glBufferData( GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), pyramid, GL_STATIC_DRAW );
	// Specify that position data is going into attribute index 0, and contains three floats per vertex
 	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    	glEnableVertexAttribArray( 0 );     // Enable attribute index 0 (position)
 
	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] ); // bind second VBO as active buffer object
    	// Copy the color data from colors to our buffer
    	// 15 * sizeof(GLfloat) is the size of the colors array, since it contains 15 GLfloat values
   	glBufferData( GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), colors, GL_STATIC_DRAW );
	// Specify that our color data is going into attribute index 1, and contains three floats per vertex 
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray( 1 );    // Enable attribute index 1 (color)

	// OpenGL is state based - we normally need to keep references to shader, VAO, VBO, etc., to allow
	// swapping between different ones. In this program there is only one shader program and one VAO

	glEnable( GL_DEPTH_TEST ); // enable depth testing
	// glEnable( GL_CULL_FACE ); // enable back face culling - try this and see what happens!
}


void draw( SDL_Window * window )
{
	if ( window == NULL )
	{
		std::cout << "Window was null." << std::endl;
		exit(-1);
	}

        glClearColor(1.0, 0.0, 0.0, 1.0); 
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 

	// Create perspective projection matrix

	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 1.0f, 100.f);

	// Apply model view transformations
	glm::mat4 identity( 1.0 ); 
	glm::mat4 translation = glm::translate( identity, glm::vec3( 0.0f, 0.0f, -4.0f ) );
	glm::mat4 rotation = glm::rotate( translation, r, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	glm::mat4 MVP = projection * rotation;  // Calculate final MVP matrix

	// pass MVP as uniform into shader
	int mvpIndex = glGetUniformLocation( shaderprogram, "MVP" );
	glUniformMatrix4fv( mvpIndex, 1, GL_FALSE, glm::value_ptr( MVP ) ); 

        glDrawArrays( GL_TRIANGLE_FAN, 0, 5 ); // draw the pyramid

	r+=0.5;

        SDL_GL_SwapWindow( window ); // swap buffers
}


void cleanup( )
{
	glUseProgram( 0 );
	glDisableVertexAttribArray( 0 );
	glDisableVertexAttribArray( 1 );
	glDeleteProgram( shaderprogram );
	glDeleteBuffers( 2, vbo );
	glDeleteVertexArrays( 1, &vao );
	// TODO: detach shaders
}


// Program entry point - SDL manages the actual WinMain entry point for us
TEST(OpenGL_UT, CreateOpenGLShaderApplicationWithoutExceptions)
{  
	SDL_Window * hWindow;
	SDL_GLContext glContext; 
 

	setupRC( hWindow, glContext ); 

	if  ( hWindow == NULL ) 
	{
		std::cout << "Get down!! (╯°□°)–︻╦╤─ – – –" << std::endl;
		exit(-1);
	}

	GLenum err = glewInit( );

	if ( GLEW_OK != err )
	{
		std::cout << "glewInit failed, aborting." << std::endl;
		exit (1);
	}



	if  ( hWindow == NULL ) 
	{
		std::cout << "Draw!! (╯°□°)–︻╦╤─ – – –" << std::endl;
		exit(-1);
	}

	initGLscene( );

	bool running = true; 
	SDL_Event sdlEvent; 


	for(int i = 0; i < 500; ++i)	
	{

		draw( hWindow ); 
	}

	cleanup( );

    
	SDL_GL_DeleteContext( glContext );
	SDL_DestroyWindow( hWindow );
        SDL_Quit( );
}
#endif
