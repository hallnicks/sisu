#if 0
#include "test.hpp"
#include <string> 
#include <iostream>

#ifndef __linux__ 
#include <Windows.h> 
#endif

#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glut.h>
#include <stdio.h>

#include <iostream>
#include <fstream>

#include <SDL2/SDL_opengl.H>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{

class OpenGL_RenderTexturedQuadShader_UT : public context
{
	public:
		OpenGL_RenderTexturedQuadShader_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};


namespace {
	char const * SAMPLE_NAME = "www.g-truc.net - OpenGL 3: Image Rectangle";	
	GLint const SAMPLE_MAJOR = 3;
	GLint const SAMPLE_MINOR = 1;
	char const * VERTEX_SHADER_SOURCE = "resources/texture-rect.vert";
	char const * FRAGMENT_SHADER_SOURCE = "resources/texture-rect.frag";
	char const * TEXTURE_DIFFUSE = "resources/kueken640-rgb8.tga";

	struct vertex
	{
		vertex
		(
			glm::vec2 const & Position,
			glm::vec2 const & Texcoord
		) :
			Position(Position),
			Texcoord(Texcoord)
		{}

		glm::vec2 Position;
		glm::vec2 Texcoord;
	};

	GLsizei const VertexCount = 6;

	GLsizeiptr const VertexSize = VertexCount * sizeof(vertex);

	vertex const VertexData[VertexCount] =
	{
		vertex(glm::vec2(-2.0f,-1.5f), glm::vec2(0.0f, 480.0f)),
		vertex(glm::vec2( 2.0f,-1.5f), glm::vec2(640.0f, 480.0f)),
		vertex(glm::vec2( 2.0f, 1.5f), glm::vec2(640.0f, 0.0f)),
		vertex(glm::vec2( 2.0f, 1.5f), glm::vec2(640.0f, 0.0f)),
		vertex(glm::vec2(-2.0f, 1.5f), glm::vec2(0.0f, 0.0f)),
		vertex(glm::vec2(-2.0f,-1.5f), glm::vec2(0.0f, 480.0f))
	};


	class sample : SDLTest
	{
	public:
		sample(std::string const & Name, glm::ivec2 const & WindowSize);
		virtual ~sample();
	
		bool check() const;
	    	void begin(glm::ivec2 const & WindowSize);
	    	void end();
	    	void render();
		virtual void run( ) {
			render( ); 
		}

	
	private:
		void initProgram();
		void initArrayBuffer();
		void initTexture2D();

		GLuint ProgramName;
	
		GLuint BufferName;
		GLuint AttribPosition;
		GLuint AttribTexcoord;
		GLuint TextureRectName;

		GLuint UniformMVP;
		GLuint UniformDiffuse;
	};



} // namespace

//**********************************
// OpenGL 3: Image Rectangle
// 28/08/2009
//**********************************
// Christophe Riccio
// christophe.riccio@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

sample::sample( std::string const & Name, glm::ivec2 const & WindowSize )
{
	;
}

sample::~sample()
{
	;
}

bool sample::check() const
{
	GLint MajorVersion = 0;
	GLint MinorVersion = 0;
	glGetIntegerv( GL_MAJOR_VERSION, &MajorVersion );
	glGetIntegerv( GL_MINOR_VERSION, &MinorVersion );
	bool Version = MajorVersion == SAMPLE_MAJOR && MinorVersion >= SAMPLE_MINOR;
	return Version;
}

void sample::begin( glm::ivec2 const & WindowSize )
{
	// TODO: implement windowsize
	initProgram( );
	initArrayBuffer( );
	initTexture2D( );
}

void sample::end( )
{
	glDeleteBuffers( 1, &BufferName );
	glDeleteProgram( ProgramName );
	glDeleteTextures( 1, &TextureRectName );
}


void sample::render()
{
	// Compute the MVP (Model View Projection matrix)
	glm::mat4 Projection 	= glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 ViewRotateX 	= glm::rotate(ViewTranslate, 0.0f, glm::vec3(-1.f, 0.0f, 0.0f));
	glm::mat4 View 		= glm::rotate(ViewRotateX, 0.0f, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model 	= glm::mat4(1.0f);
	glm::mat4 MVP 		= Projection * View * Model;

	glViewport(0, 0, 640, 480);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Bind the program for use
	glUseProgram(ProgramName);

	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, TextureRectName);

	glBindBuffer(GL_ARRAY_BUFFER, BufferName);
	glVertexAttribPointer(AttribPosition, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(AttribTexcoord, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(AttribPosition);
	glEnableVertexAttribArray(AttribTexcoord);
		glDrawArrays(GL_TRIANGLES, 0, VertexCount);
	glDisableVertexAttribArray(AttribTexcoord);
	glDisableVertexAttribArray(AttribPosition);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);
}

void sample::initProgram()
{
	// Compile a shader
	GLuint VertexShaderName = 0;

	std::string Source0 = ""; //glf::loadFile(VERTEX_SHADER_SOURCE);
	char const * Source = Source0.c_str();
	VertexShaderName = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderName, 1, &Source, NULL);
	glCompileShader(VertexShaderName);

	// Compile a shader
	GLuint FragmentShaderName = 0;

	Source0 = ""; //TODO glf::loadFile(FRAGMENT_SHADER_SOURCE);
	Source = Source0.c_str();
	FragmentShaderName = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderName, 1, &Source, NULL);
	glCompileShader(FragmentShaderName);

	// Link a program
	ProgramName = glCreateProgram();
	glAttachShader(ProgramName, VertexShaderName);
	glAttachShader(ProgramName, FragmentShaderName);
	glDeleteShader(VertexShaderName);
	glDeleteShader(FragmentShaderName);
	glBindFragDataLocation(ProgramName, 0, "FragColor");
	glLinkProgram(ProgramName);

	UniformMVP = glGetUniformLocation(ProgramName, "MVP");
	UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
	AttribPosition = glGetAttribLocation(ProgramName, "Position");
	AttribTexcoord = glGetAttribLocation(ProgramName, "Texcoord");
	
	// Bind the program for use
	glUseProgram(ProgramName);

	// Set uniform value
	glUniform1i(UniformDiffuse, 0);

	// Unbind the program
	glUseProgram(0);
}

void sample::initArrayBuffer()
{
	glGenBuffers( 1, &BufferName) ;
	glBindBuffer( GL_ARRAY_BUFFER, BufferName );
	glBufferData( GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void sample::initTexture2D()
{
	GLint TextureSize = 0;
	glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &TextureSize);

	glGenTextures(1, &TextureRectName);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, TextureRectName);

	// Set filter
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//Only: GL_CLAMP, GL_CLAMP_TO_EDGE, and CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	//Only: GL_CLAMP, GL_CLAMP_TO_EDGE, and CLAMP_TO_BORDER

	// Set image
	//gli::image Image = gli::loadImage(TEXTURE_DIFFUSE);


	for(std::size_t Level = 0; Level < 3/*Image.levels()*/; ++Level)
	{
		glTexImage2D(
			GL_TEXTURE_RECTANGLE, 
			GLint(Level), 
			GL_RGB, 
			1,//GLsizei(Image[Level].width()), 
			1,//GLsizei(Image[Level].height()), 
			//GLsizei(1), //depth
			0,  
			GL_RGB, 
			GL_UNSIGNED_BYTE, 
			NULL//Image[Level].data()
			);
	}
}


// Program entry point - SDL manages the actual WinMain entry point for us
TEST(OpenGL_RenderTexturedQuadShader_UT, CreateOpenGLShaderApplicationWithoutExceptions)
{  
	glm::ivec2 ScreenSize = glm::ivec2(640, 480);

	sample Sample(SAMPLE_NAME, ScreenSize);

	if( Sample.check() )
	{
		Sample.begin(ScreenSize);
		while (1)
		{
			Sample.run();
		}

		Sample.end();
	}



	BLOCK_EXECUTION;
}
#endif
