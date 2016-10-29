#ifndef SKYBOX_657B9BE2244D4D169910F302DBBD70ED_HPP_
#define SKYBOX_657B9BE2244D4D169910F302DBBD70ED_HPP_

#include "SDLShader.hpp"
#include "DevILImage.hpp"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sisu {

class Skybox
{
	static GLfloat const sSkyboxVertices[ 3*6*6 ];
	static uint8_t const sFaceCount = 6;

	SDLShader mSkyboxShader;
	SDLShader mCubemapShader;

	GLuint mSkyboxVAO
	     , mSkyboxVBO
	     , mSkyboxTexID;

	uint32_t mW, mH;

	DevILImage mFaces[ sFaceCount ];
	GLubyte * mFaceData[ sFaceCount ];

	void _initializeVertexObjects( )
	{
		// Initialize VBO/VAo
		glGenVertexArrays( 1, &mSkyboxVAO );
		glGenBuffers( 1, &mSkyboxVBO );
		glBindVertexArray( mSkyboxVAO );
		glBindBuffer( GL_ARRAY_BUFFER, mSkyboxVBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof( sSkyboxVertices ), &sSkyboxVertices, GL_STATIC_DRAW );
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), ( GLvoid* )0 );
		glBindVertexArray( 0 );
	}

	void _loadCubemap( )
	{
		glGenTextures( 1, &mSkyboxTexID );

		glBindTexture( GL_TEXTURE_CUBE_MAP, mSkyboxTexID );

		_checkForGLError( "After bind texture" );


		for ( uint8_t ii = 0; ii < sFaceCount; ++ii )
		{
			DevILImage & face = mFaces[ ii ];

			mFaceData[ ii ] = face.toGLTextureBuffer( );

			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + ii
				    , 0
				    , GL_RGBA
				    , face.getWidth( )
				    , face.getHeight( )
				    , 0
				    , GL_RGBA
				    , GL_UNSIGNED_BYTE
				    , mFaceData[ ii ] );

			_checkForGLError( "After cube glTexImage2D" );

		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		_checkForGLError( "After cube map set up" );

	}

	public:
		Skybox( const char * xFaces[ sFaceCount ] )
			: mSkyboxShader( ShaderSourcePair("#version 300 es                                                  \n"
							  "layout (location = 0) in vec3 position;                          \n"
							  "out vec3 TexCoords;                                              \n"
							  "uniform mat4 projection;                                         \n"
							  "uniform mat4 view;                                               \n"
							  "void main( )                                                     \n"
							  "{								    \n"
							  "    vec4 pos = projection * view * vec4( position, 1.0 );        \n"
							  "    gl_Position = pos.xyww;                                      \n"
							  "    TexCoords = position;                                        \n"
							  "}"
						   	 ,"#version 300 es                                                  \n"
						    	  "precision mediump float;				            \n"
							  "in vec3 TexCoords;                                               \n"
							  "out vec4 color;                                                  \n"
							  "uniform samplerCube skybox;                                      \n"
							  "void main( )                                                     \n"
							  "{								    \n"
							  "	color = texture( skybox, TexCoords );                       \n"
							  "}                                                                \n") )

			, mCubemapShader( ShaderSourcePair("#version 300 es							  \n"
							   "layout (location = 0) in vec3 position;                         	  \n"
							   "layout (location = 1) in vec3 normal;			    	  \n"
							   "out vec3 Normal;						    	  \n"
							   "out vec3 Position;						    	  \n"
							   "uniform mat4 model;						    	  \n"
							   "uniform mat4 view; 						    	  \n"
							   "uniform mat4 projection;					    	  \n"
							   "void main( )						    	  \n"
							   "{								    	  \n"
							   "    gl_Position = projection * view * model * vec4( position, 1.0f ); \n"
							   "    Normal = mat3(transpose(inverse(model))) * normal;		  \n"
							   "    Position = vec3(model * vec4(position, 1.0f));                    \n"
							   "}                                                                     \n"
						          ,"#version 300 es							  \n"
						    	   "precision mediump float;				                  \n"
							   "in vec3 Normal;							  \n"
							   "in vec3 Position;							  \n"
							   "out vec4 color; 							  \n"
							   "uniform vec3 cameraPos;						  \n"
							   "uniform samplerCube skybox;						  \n"
							   "void main( )							  \n"
							   "{									  \n"
							   "    vec3 I = normalize( Position - cameraPos ); 			  \n"
							   "    vec3 R = reflect( I, normalize( Normal ) );			  \n"
							   "    color = texture( skybox, R );				   	  \n"
							   "}									  \n" ) )
			, mSkyboxVAO( 0 )
			, mSkyboxVBO( 0 )
			, mFaces( { xFaces[ 0 ]
				  , xFaces[ 1 ]
				  , xFaces[ 2 ]
				  , xFaces[ 3 ]
				  , xFaces[ 4 ]
				  , xFaces[ 5 ] } )
			, mFaceData( )
			, mW( 0 )
			, mH( 0 )
		{
			for ( uint8_t ii = 0; ii < sFaceCount; ++ii )
			{
				mFaceData[ ii ] = NULL;
			}
		}

		~Skybox( )
		{
			for ( uint8_t ii = 0; ii < sFaceCount; ++ii )
			{
				if ( mFaceData[ ii ] != NULL )
				{
					free( mFaceData[ ii ] );
				}
			}
		}

		void initialize( uint32_t const xW, uint32_t const xH )
		{
			mW = xW;
			mH = xH;

			mSkyboxShader.initialize( );
			mCubemapShader.initialize( );

			_initializeVertexObjects( );

			_loadCubemap( );
		}

		void render( glm::mat4 const & xViewMatrix, GLfloat const xFov )
		{
			glDepthFunc( GL_LEQUAL );
			glDepthMask ( GL_FALSE );

			mSkyboxShader([&]( ) {
				glm::mat4 view = glm::mat4( glm::mat3( xViewMatrix ) );

				glm::mat4 projection = glm::perspective( xFov, (float) mW / (float) mH, 0.1f, 100.0f );

				mSkyboxShader.getUniforms( ).setUniformMatrix4fv( "view"      , view);

				mSkyboxShader.getUniforms( ).setUniformMatrix4fv( "projection", projection  );

				glBindVertexArray( mSkyboxVAO );

				glActiveTexture( GL_TEXTURE0 );

				glBindTexture( GL_TEXTURE_CUBE_MAP, mSkyboxTexID );

				glDrawArrays( GL_TRIANGLES, 0, 6*6 );

				glBindVertexArray( 0 );

			} );

			glDepthFunc( GL_LESS );
			glDepthMask ( GL_TRUE );
		}
};

GLfloat const Skybox::sSkyboxVertices[3*6*6] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
};

} // namespace sisu

#endif // SKYBOX_657B9BE2244D4D169910F302DBBD70ED_HPP_
