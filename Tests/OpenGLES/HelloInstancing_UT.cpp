#ifdef OPENGLES_HELLOINSTANCING_UT
#ifdef OPENGLES

// TODO: Generally, we need to delete VAOs and VBos
#include "test.hpp"

#ifdef WIN32
#include <Windows.h>
#elif defined(LINUX)
#include <X11/Xlib.h>
#endif

#include "SDLTest.hpp"
#include "SDLShader.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"
#include "PNGImage.hpp"
#include "sisumath.hpp"
#include "mouse.hpp"
#include "keyboard.hpp"
#include "GLCharacterMap.hpp"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

namespace sisu {

struct Vertex
{
	glm::vec3 Position
		, Normals;
	glm::vec2 TexCoords;
	/*
	glm::vec3 Tangent
		, Bitangent;
	*/
};

class Mesh
{
	std::vector<Vertex>    mVertices;
	std::vector<GLuint>    mIndices;
	std::vector<Texture2D> mTextures;

	GLuint mVAO
	     , mVBO
	     , mEBO;


	void _setupMesh( )
	{
	        // Create buffers/arrays
	        glGenVertexArrays(1, &mVAO);
	        glGenBuffers(1, &mVBO);
	        glGenBuffers(1, &mEBO);

	        glBindVertexArray(mVAO);
        	// Load data into vertex buffers
	        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	        // A great thing about structs is that their memory layout is sequential for all its items.
	        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	        // again translates to 3/2 floats which translates to a byte array.
	        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), &mIndices[0], GL_STATIC_DRAW);

	        // Set the vertex attribute pointers
	        // Vertex Positions
	        glEnableVertexAttribArray(0);
	        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	        // Vertex Normals
	        glEnableVertexAttribArray(1);
	        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normals));
	        // Vertex Texture Coords
	        glEnableVertexAttribArray(2);
	        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
		/*
		// Vertex Tangent
	        glEnableVertexAttribArray(3);
	        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
	        // Vertex Bitangent
	        glEnableVertexAttribArray(4);
	        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));
		*/

	        glBindVertexArray(0);
	}

	public:
		Mesh( )
			: mVertices( )
			, mIndices( )
			, mTextures( )
		{
			;
		}

		void initialize( std::vector<Vertex>    xVertices
			       , std::vector<GLuint>    xIndices
			       , std::vector<Texture2D> xTextures )
		{
			mVertices = xVertices;
			mIndices  = xIndices;
			mTextures = xTextures;

			_setupMesh( );
		}


		void render( SDLShader & xShader )
		{

			xShader([&]( )
			{
				GLuint diffuseNr  = 1
				     , specularNr = 1
				     , normalNr   = 1
				     , heightNr   = 1;

				for(GLuint i = 0; i < mTextures.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
					// Retrieve texture number (the N in diffuse_textureN)
					std::stringstream ss;

            				std::string number, name;

					switch ( mTextures[ i].getType( ) )
					{
						case eTexture2DType_Diffuse:
						{
							ss << diffuseNr++; // Transfer GLuint to stream

							name = "texture_diffuse";
							break;
						}

						case eTexture2DType_Specular:
						{
					                ss << specularNr++; // Transfer GLuint to stream
							name = "texture_specular";
							break;
						}

						case eTexture2DType_Normal:
						{
							ss << normalNr++;
							name = "texture_normal";
							break;
						}

						case eTexture2DType_Height:
						{
							ss << heightNr++;
							name = "texture_height";
							break;
						}

						default:
						{
							std::cerr << "Invalid texture type." << std::endl;
							exit( -1 );
						}
					}

					number = ss.str();
					// Now set the sampler to the correct texture unit
					xShader.getUniforms( ).setUniform1i( ( name + number ).c_str( ), i );
					mTextures[ i ].bind( );
			        }

				xShader.getUniforms( ).setUniform1i( "material.shininess", 16.0f );

			        // Draw mesh
			        glBindVertexArray(mVAO);
			        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

        			// Always good practice to set everything back to defaults once configured.
				for (GLuint i = 0; i < mTextures.size(); i++)
			        {
				    	glActiveTexture(GL_TEXTURE0 + i);
					Texture2D::unbind( );
			        }
			} );
		}
};

class Model
{
	std::vector< Texture2D > mTextures;
	std::vector< Mesh > 	 mMeshes;
	std::string 		 mDirectory;

	bool mGammaCorrection;

	void _loadModel( const char * xPath )
	{
		std::string path( xPath );

		Assimp::Importer importer;
        	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
 	       // Check for errors
	        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	        {
        	    std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
	            return;
	        }
	        // Retrieve the directory path of the filepath
        	mDirectory = path.substr(0, path.find_last_of('/'));

	        // Process ASSIMP's root node recursively
       		_processNode(scene->mRootNode, scene);
	}

	void _processNode( aiNode* xNode, const aiScene* xScene )
	{
        // Process each mesh located at the current node
	        for(GLuint i = 0; i < xNode->mNumMeshes; i++)
	        {
	            // The node object only contains indices to index the actual objects in the scene.
	            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
	            aiMesh* mesh = xScene->mMeshes[ xNode->mMeshes[ i ] ];

	            mMeshes.push_back( _processMesh( mesh, xScene ) );
        	}

	        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	        for(GLuint i = 0; i < xNode->mNumChildren; i++)
	        {
	            _processNode( xNode->mChildren[ i ], xScene );
	        }
	}

	static void _loadVertices( std::vector <Vertex > & xVertices, aiMesh * xMesh )
	{

		for ( GLuint ii = 0; ii < xMesh->mNumVertices; ii++ )
		{
			Vertex vertex;

			glm::vec3 vector3;

			vector3.x = xMesh->mVertices[ ii ].x;
			vector3.y = xMesh->mVertices[ ii ].y;
			vector3.z = xMesh->mVertices[ ii ].z;

			vertex.Position = vector3;

			vector3.x = xMesh->mNormals[ ii ].x;
			vector3.y = xMesh->mNormals[ ii ].y;
			vector3.z = xMesh->mNormals[ ii ].z;

			vertex.Normals = vector3;

			if ( xMesh->mTextureCoords[ 0 ] != NULL )
			{
				glm::vec2 vector2;

				vector2.x = xMesh->mTextureCoords[0][ ii ].x;
				vector2.y = xMesh->mTextureCoords[0][ ii ].y;

				vertex.TexCoords = vector2;
			}
			else
			{
				vertex.TexCoords = glm::vec2( 0.0f, 0.0f );
			}

			/*
			vector3.x = xMesh->mTangents[ ii ] .x;
			vector3.y = xMesh->mTangents[ ii ] .y;
			vector3.z = xMesh->mTangents[ ii ] .z;

			vertex.Tangent = vector3;

			vector3.x = xMesh->mBitangents[ ii ] .x;
			vector3.y = xMesh->mBitangents[ ii ] .y;
			vector3.z = xMesh->mBitangents[ ii ] .z;

			vertex.Bitangent = vector3;

			*/

			xVertices.push_back( vertex );
		}

	}

	static void _loadIndices( std::vector < GLuint > & xIndices, aiMesh * xMesh )
	{
	        for(GLuint i = 0; i < xMesh->mNumFaces; i++)
        	{
	            	aiFace face = xMesh->mFaces[i];
        	    	// Retrieve all indices of the face and store them in the indices vector
	            	for(GLuint j = 0; j < face.mNumIndices; j++)
			{
        	        	xIndices.push_back(face.mIndices[j]);
			}
	        }
	}

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	std::vector<Texture2D> _loadMaterialTextures( aiMaterial* mat
						    , aiTextureType type
  						    , eTexture2DType const xLocalEquivalent )
	{
		std::vector<Texture2D> textures;
        	for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
        	{
	            aiString str;
	            mat->GetTexture(type, i, &str);
	            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
	            GLboolean skip = false;
			// TODO: use std::map to reduce this overhead
	            for(GLuint j = 0; j < mTextures.size(); j++)
	            {
	                if(mTextures[j].getIdentifier( ) == str)
	                {
	                    textures.push_back(mTextures[j]);
	                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
	                    break;
	                }
	            }
	            if(!skip)
	            {   // If texture hasn't been loaded already, load it

			std::stringstream ss;

			ss << mDirectory << "/" << str.C_Str( );

			PNGImage image( ss.str( ).c_str( ) );

			if ( !image.getIsValid( ) )
			{
				std::cerr << ss.str( ) << " was not found." << std::endl;
				exit( -1 );
			}

	                Texture2D texture( xLocalEquivalent );

			texture.initialize( image.getWidth( )
					   , image.getHeight( )
					   , image.toGLTextureBuffer( )
					   , aiString( ss.str( ).c_str( ) ) );

	                textures.push_back(texture);
	                mTextures.push_back(texture);
	            }
	        }
        	return textures;
	}

	void _loadTextures( std::vector< Texture2D > & xTextures, aiMesh * xMesh, const aiScene * xScene )
	{
	        if( xMesh->mMaterialIndex >= 0)
	        {
	        	aiMaterial* material = xScene->mMaterials[ xMesh->mMaterialIndex ];

			auto __loadMatTex = [&]( aiTextureType const xAssimpType, eTexture2DType const xLocalType )
			{
				std::vector< Texture2D > maps = _loadMaterialTextures( material, xAssimpType, xLocalType );
				xTextures.insert( xTextures.end( ), maps.begin( ), maps.end( ) );
			};

			__loadMatTex( aiTextureType_DIFFUSE , eTexture2DType_Diffuse  );
			__loadMatTex( aiTextureType_SPECULAR, eTexture2DType_Specular );
			__loadMatTex( aiTextureType_HEIGHT  , eTexture2DType_Normal   ); // ? Bug ? TODO: Ask joey.
			__loadMatTex( aiTextureType_AMBIENT , eTexture2DType_Height   );
		}
	}

	Mesh _processMesh( aiMesh * xMesh, const aiScene * xScene )
	{
		std::vector< Vertex >    vertices;
		std::vector< GLuint >    indices;
		std::vector< Texture2D > textures;

		Mesh ret;

		_loadVertices( vertices, xMesh );
		_loadIndices( indices, xMesh );
		_loadTextures( textures, xMesh, xScene );


		ret.initialize( vertices, indices, textures );


		return ret;
	}

	public:
		Model( )
			: mGammaCorrection( false )
		{
			;
		}

		void render( SDLShader & xShader )
		{
			for ( GLuint ii = 0; ii < mMeshes.size( ); ii++ )
			{
				mMeshes[ ii ].render( xShader );
			}
		}

		void initialize( const char * xPath )
		{
			_loadModel( xPath );
		}
};

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

	PNGImage mFaces[ sFaceCount ];
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
			PNGImage & face = mFaces[ ii ];

			if ( !face.getIsValid( ) )
			{
				std::cerr << "PNG resource not found." << std::endl;
				exit( -1 );
			}

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

class Quad
{
	GLuint mQuadVBO, mQuadVAO;

	public:
		Quad( )
			: mQuadVBO( 0 )
			, mQuadVAO( 0 )
		{
			;
		}

		void initialize( )
		{
			static GLfloat const vertices[] = { // Pos      // Tex
							    0.0f, 1.0f, 0.0f, 1.0f,
							    1.0f, 0.0f, 1.0f, 0.0f,
							    0.0f, 0.0f, 0.0f, 0.0f,

							    0.0f, 1.0f, 0.0f, 1.0f,
							    1.0f, 1.0f, 1.0f, 1.0f,
							    1.0f, 0.0f, 1.0f, 0.0f
	                                                   };

			glGenVertexArrays( 1, &mQuadVAO );

			glGenBuffers( 1, &mQuadVBO );

			glBindBuffer( GL_ARRAY_BUFFER, mQuadVBO );

        	        glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	                glBindVertexArray( mQuadVAO );

	                glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), ( GLvoid* )0 );
        	        glEnableVertexAttribArray( 0 );

	    		// TexCoord attribute
	    		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	    		glEnableVertexAttribArray(2);

        	        glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindVertexArray( 0 );
		}

		void render( Texture2D & xTexture, GLenum const xTextureUnit )
		{
                        glActiveTexture( xTextureUnit );

                        xTexture( [ & ]( )
       	                {
                                glBindVertexArray( mQuadVAO );

               	                glDrawArrays( GL_TRIANGLES, 0, 6 );

                                glBindVertexArray( 0 );
                        } );
		}
};

class Overlay2D
{
	SDLShader mSpriteShader;
	Quad mQuad;

	public:
		Overlay2D( )
			: mSpriteShader( ShaderSourcePair("#version 300 es                            			    \n"
      				   			  "layout(location = 0) in vec4 vertex;       			    \n"
	 	 				          "out vec2 TexCoords;                        			    \n"
						          "uniform mat4 model;                        			    \n"
						    	  "uniform mat4 projection;                   			    \n"
						    	  "void main()                               			    \n"
						    	  "{                                          			    \n"
						    	  "   TexCoords = vertex.zw;                  			    \n"
						    	  "   gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0); \n"
						    	  "}                                          			    \n"
						   	 ,"#version 300 es                                                  \n"
						    	  "precision mediump float;				            \n"
   	  		  			    	  "in vec2 TexCoords;				                    \n"
   	  		  			    	  "out vec4 color;				                    \n"
						    	  "uniform sampler2D image; 				            \n"
						    	  "uniform vec4 spriteColor;                                        \n"
						    	  "void main()                                                      \n"
						    	  "{                                                                \n"
						    	  "  color = spriteColor * texture(image, TexCoords);    	    \n"
						    	  "}                                                                \n") )
			, mQuad( )

		{
			;
		}

		void initialize( uint32_t const xW, uint32_t const xH )
		{
			mSpriteShader.initialize( );

			glm::mat4 projection = glm::ortho(  0.0f
	                                                  , static_cast<GLfloat>( xW )
	                                                  , static_cast<GLfloat>( xH )
	                                                  ,  0.0f
	                                                  , -1.0f
	                                                  ,  1.0f );

	                mSpriteShader( [ & ]( )
        	        {
				mSpriteShader.getUniforms( ).setUniform1i( "image", 0 );
				mSpriteShader.getUniforms( ).setUniformMatrix4fv( "projection", projection );
			});

			mQuad.initialize( );
		}

	        void drawTexture( Texture2D & xTexture
	                        , glm::vec2 const xPosition
	                        , glm::vec2 const xSize
	                        , GLfloat xRotate
	                        , glm::vec4 const xColor )
	        {
        	        mSpriteShader( [ & ] ( )
	                {
	                        glm::mat4 model;

        	                model = glm::translate( model, glm::vec3( xPosition, 0.0f ) );

	                        model = glm::translate( model, glm::vec3( 0.5f * xSize.x, 0.5f * xSize.y, 0.0f ) );

	                        model = glm::rotate(    model, xRotate, glm::vec3( 0.0f, 0.0f, 1.0f ) );

        	                model = glm::translate( model, glm::vec3( -0.5f * xSize.x, -0.5f * xSize.y, 0.0f ) );

                	        model = glm::scale(     model, glm::vec3( xSize, 1.0f ) );

	                        mSpriteShader.getUniforms( ).setUniformMatrix4fv( "model", model );

        	                mSpriteShader.getUniforms( ).setUniformVector4f( "spriteColor", xColor );

				mQuad.render( xTexture, GL_TEXTURE0 );
	                } );
	        }
};

template< typename XType >
class Oscillator
{
	XType mValue
	    , mMin
	    , mMax;

	XType const mStep;

	bool mReverse;

	public:
		Oscillator( XType const xInitial
			  , XType const xMin
			  , XType const xMax
			  , XType const xStep )
			: mValue( xInitial )
			, mMin( xMin )
			, mMax( xMax )
			, mStep( xStep )
			, mReverse( false )
		{
			;
		}

		void setMinMax( XType const xNewMin, XType const xNewMax )
		{
			mMin = xNewMin;
			mMax = xNewMax;
		}

		XType operator++( )
		{
			if ( !mReverse )
			{
				mValue -= mStep;
			}
			else
			{
			 	mValue += mStep;
			}

			if ( mValue <= mMin )
			{
				mReverse = true;
				mValue = mMin;
			}
			else if ( mValue >= mMax )
			{
				mReverse = false;
			}

			return mValue;
		}
};

enum eMovementDirection
{
	eMovementDirection_None,
	eMovementDirection_W,
	eMovementDirection_A,
	eMovementDirection_S,
	eMovementDirection_D,
};

enum eHorizontalCameraRotation
{
	eHorizontalCameraRotation_None,
	eHorizontalCameraRotation_Left,
	eHorizontalCameraRotation_Right,
};

enum eVerticalCameraRotation
{
	eVerticalCameraRotation_None,
	eVerticalCameraRotation_Up,
	eVerticalCameraRotation_Down,
};

class Camera
{
	static GLfloat const constexpr sMaxFov  = 1.00f;
	static GLfloat const constexpr sMinFov  = 0.05f;

	glm::vec3 mCameraPos
		, mCameraFront
		, mCameraUp;

	eMovementDirection mMovementDirection;

	eHorizontalCameraRotation mHorizontalCameraRotation;

	eVerticalCameraRotation mVerticalCameraRotation;

	GLfloat mYaw
	      , mPitch
	      , mFov
	      , mCameraSpeed;

	public:
		Camera( )
			: mCameraPos( glm::vec3(0.0f, 0.0f,  -3.0f) )
			, mCameraFront( glm::vec3(0.0f, 0.0f, -1.0f) )
			, mCameraUp( glm::vec3(0.0f, 1.0f,  0.0f) )
			, mMovementDirection( eMovementDirection_None )
			, mHorizontalCameraRotation( eHorizontalCameraRotation_None )
			, mVerticalCameraRotation( eVerticalCameraRotation_None )
			, mYaw(  -90.0f ) // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
			, mPitch( 0.0f )
			, mFov( sMaxFov / 2 )
		{
			;
		}

		void rotateCamera( GLfloat xXOffset, GLfloat xYOffset )
		{
			GLfloat sensitivity = 0.05;	// Change this value to your liking // TODO: Make this a member var configurable from settings screen
			xXOffset *= sensitivity;
			xYOffset *= sensitivity;

			mYaw   += xXOffset;
			mPitch += xYOffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (mPitch > 89.0f)
			{
			    mPitch = 89.0f;
			}

			if (mPitch < -89.0f)
			{
			    mPitch = -89.0f;
			}

			glm::vec3 front;
			front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
			front.y = sin(glm::radians(mPitch));
			front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
			mCameraFront = glm::normalize(front);
		}

		void setMovementDirection( eMovementDirection const xMovementDirection )
		{
			mMovementDirection = xMovementDirection;
		}

		void setHorizontalCameraRotation( eHorizontalCameraRotation const xHorizontalCameraRotation )
		{
			mHorizontalCameraRotation = xHorizontalCameraRotation;
		}

		void setVerticalCameraRotation( eVerticalCameraRotation const xVerticalCameraRotation )
		{
			mVerticalCameraRotation = xVerticalCameraRotation;
		}

		void setFov( GLfloat const xScrollOffset )
		{
			if (mFov >= sMinFov && mFov <= sMaxFov )
			{
			    mFov -= xScrollOffset;
			}

			if (mFov <= sMinFov )
			{
			    mFov = sMinFov;
			}

			if (mFov >= sMaxFov )
			{
			    mFov = sMaxFov;
			}
		}

		void moveCamera( GLfloat const xDeltaTime )
		{
			mCameraSpeed = 1.0f * xDeltaTime;

			switch ( mMovementDirection )
			{
				case eMovementDirection_W:
			    	{
					mCameraPos += mCameraSpeed * mCameraFront;
					break;
				}

				case eMovementDirection_S:
				{
					mCameraPos -= mCameraSpeed * mCameraFront;
					break;
				}

				case eMovementDirection_A:
		        	{
					mCameraPos -= glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed;
					break;
				}

				case eMovementDirection_D:
		        	{
					mCameraPos += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed;
					break;
				}

				default:
					break;
			}

		}

		void handleCameraRotation( )
		{
			static GLfloat const sMovementSpeed = 10.0f;

			switch ( mHorizontalCameraRotation )
			{
				case eHorizontalCameraRotation_Left:
				{
					rotateCamera( -sMovementSpeed, 0 );
					break;
				}

				case eHorizontalCameraRotation_Right:
				{
					rotateCamera( sMovementSpeed, 0 );
					break;
				}

				case eHorizontalCameraRotation_None:
				default:
					break;
			}

			switch ( mVerticalCameraRotation )
			{
				case eVerticalCameraRotation_Up:
				{
					rotateCamera( 0, -sMovementSpeed );
					break;
				}

				case eVerticalCameraRotation_Down:
				{
					rotateCamera( 0, sMovementSpeed );
					break;
				}

				case eVerticalCameraRotation_None:
				default:
					break;
			}
		}

		// TODO: Separate camera class
		glm::mat4 const getViewMatrix( )
		{
			return glm::lookAt( mCameraPos, mCameraPos + mCameraFront, mCameraUp );
		}

		GLfloat const getFOV( ) const { return mFov; }
};

class CubeRenderer
{
	static GLfloat const constexpr sFovStep = 0.05f;

    	static GLfloat const sVertices[5*6*6];
	static glm::vec3 const sCubePositions[10];

	Texture2D mSecondTexture
		, mThirdTexture
		, mFourthTexture
		, mFifthTexture
		, mSixthTexture;

	PNGImage mSecondPNGImage
	       , mThirdPNGImage
	       , mFourthPNGImage
	       , mFifthPNGImage
	       , mSixthPNGImage;

	std::vector< Oscillator< GLfloat > > mOscillators;
	GLfloat  mLastX, mLastY;

	SDLShader m3DCameraShader;
	GLuint mCubeVBO, mCubeVAO;

	Quad mPlane;

	bool mFirstMouse;

	// Deltatime
	GLfloat mDeltaTime, mLastFrame;

	Stopwatch mDeltaWatch;

	uint32_t mW, mH;

	Camera * mCamera;

	void _checkCamera( ) const
	{
		if ( mCamera == NULL )
		{
			std::cerr << "Null camera in " <<  __PRETTY_FUNCTION__ << std::endl;
			exit( -1 );
		}
	}

	void _render( std::function<void(void)> xLambda )
	{
		_checkCamera( );

		m3DCameraShader([&]( ) {
			glm::mat4 projection  = glm::perspective( mCamera->getFOV( ), (GLfloat) mW / (GLfloat) mH, 0.1f, 100.0f );

			m3DCameraShader.getUniforms( ).setUniformMatrix4fv("view"       , mCamera->getViewMatrix( ) );

			m3DCameraShader.getUniforms( ).setUniformMatrix4fv("projection", projection );

			xLambda( );

			_checkForGLError( "After render 3d" );
		} );
	}

	void _setModelMatrix( glm::vec3 const & xPosition
			    , GLfloat const xRotateAngle
			    , glm::vec3 const & xScale )
	{
		glm::mat4 model;

		model = glm::translate( model, xPosition );

		model = glm::rotate( model, xRotateAngle, glm::vec3( 1.0f, 0.3f, 0.5f ) );

        	model = glm::scale( model, xScale );

		m3DCameraShader.getUniforms( ).setUniformMatrix4fv( "model", model );
	}

	void _drawCubes( GLfloat const xAngleOffset )
	{
		for ( GLuint ii = 0; ii <  sizeof( sCubePositions ) / sizeof( glm::vec3 ); ii++ )
		{
			GLfloat const angle = xAngleOffset * ii;

			GLfloat const scale = ++mOscillators[ ii ];

			_setModelMatrix( sCubePositions[ ii ]
				       , angle
				       , glm::vec3( scale
					          , scale
					          , scale ) );

			glBindVertexArray( mCubeVAO );

			glDrawArrays( GL_TRIANGLES, 0, 6 * 6 );

			glBindVertexArray( 0 );
		}

	}

	void _drawCube( glm::vec3 const & xPosition
  		      , GLfloat const xAngle
		      , glm::vec3 const & xScale )
	{
		_setModelMatrix( xPosition, xAngle, xScale );

		glBindVertexArray( mCubeVAO );

		glDrawArrays( GL_TRIANGLES, 0, 6 * 6 );

		glBindVertexArray( 0 );

	}


	public:
		CubeRenderer( )
			: m3DCameraShader( ShaderSourcePair("#version 300 es                                                      \n"
		 					    "layout (location = 0) in vec3 position;                              \n"
							    "layout (location = 2) in vec2 texCoord;                              \n"
				   	  		    "out vec2 TexCoord;                                                   \n"
			 		   		    "uniform mat4 model;                                                  \n"
					   		    "uniform mat4 view;                                                   \n"
					   		    "uniform mat4 projection;                                             \n"
					   		    "void main()                                                          \n"
					    		    "{                                                                    \n"
					 		    "    gl_Position = projection * view * model * vec4(position, 1.0f);  \n"
							    "    TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);                   \n"
			 				    "}                                                                    \n"
		  	   	 			  , "#version 300 es                                                                       \n"
					    	            "precision mediump float;				                                   \n"
			 				    "in vec2 TexCoord;                                                                     \n"
							    "out vec4 color;                                                                       \n"
					  		    "uniform sampler2D ourTexture1;                                                        \n"
					  		    "uniform sampler2D ourTexture2;                                                        \n"
					  		    "void main()                                                                           \n"
							    "{                                                                                     \n"
					    		    "    color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2); \n"
					 		    "}                                                                                     \n" ) )
			, mSecondTexture( )
			, mThirdTexture( )
			, mFourthTexture( )
			, mFifthTexture( )
			, mSixthTexture( )
			, mSecondPNGImage( "resources/testinput/testinput14.png" ) // TODO:  Make textures dynamic! Add PBOs, etc
			, mThirdPNGImage(  "resources/testinput/testinput15.png" )
			, mFourthPNGImage( "resources/testinput/testinput17.png" )
			, mFifthPNGImage(  "resources/testinput/testinput18.png" )
			, mSixthPNGImage(  "resources/testinput/testinput19.png" )
			, mOscillators( sizeof( sCubePositions ) / sizeof( glm::vec3 ), Oscillator<GLfloat>( 1.0f, 1.0f, 2.0f, 0.025f ) )
			, mLastX( 0 )
			, mLastY( 0 )
			, mDeltaTime( 0.0f )
			, mLastFrame( 0.0f )
			, mDeltaWatch( )
			, mFirstMouse( true )
			, mW( 0 )
			, mH( 0 )
			, mCamera( NULL )
		{
			;
		}

		void initialize( uint32_t const xW, uint32_t const xH, Camera * const xCamera )
		{
			mW = xW;
			mH = xH;

			mCamera = xCamera;

			_checkCamera( );

			m3DCameraShader.initialize( );

			mPlane.initialize( );

			glEnable(GL_BLEND);
			glEnable( GL_DEPTH_TEST );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			glDepthFunc( GL_LESS );

			mLastX = mW / 2.0;
			mLastY = mH / 2.0;

	 		glGenVertexArrays(1, &mCubeVAO);
	   		glGenBuffers(1, &mCubeVBO);

	    		glBindVertexArray(mCubeVAO);

	    		glBindBuffer(GL_ARRAY_BUFFER, mCubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof( sVertices), sVertices, GL_STATIC_DRAW);

    			// Position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    			glEnableVertexAttribArray(0);
	    		// TexCoord attribute
	    		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	    		glEnableVertexAttribArray(2);

	    		glBindVertexArray(0); // Unbind VAO

			_checkForGLError( "After set vertices" );

			auto __initializeTex = [&]( PNGImage & xImage, Texture2D & xTex )
			{
				if ( !xImage.getIsValid( ) )
				{
					std::cerr << "Failed to load PNG image resource." << std::endl;
					exit( -1 );
				}

				xTex.initialize( xImage.getWidth( )
					       , xImage.getHeight( )
					       , xImage.toGLTextureBuffer( ) );
			};

			__initializeTex( mSecondPNGImage, mSecondTexture );
			__initializeTex( mThirdPNGImage , mThirdTexture  );
			__initializeTex( mFourthPNGImage, mFourthTexture );
			__initializeTex( mFifthPNGImage , mFifthTexture  );
			__initializeTex( mSixthPNGImage , mSixthTexture  );

			_checkForGLError( "After initialize texture" );

			m3DCameraShader([&](){
				m3DCameraShader.getUniforms( ).setUniform1i( "ourTexture1", 1 );
				m3DCameraShader.getUniforms( ).setUniform1i( "ourTexture2", 2 );
			});

			mDeltaWatch.startS( );
		}

		void onKeyboardEvent( KeyboardEvent const & xEvent )
		{
			_checkCamera( );

			// Camera controls
			if ( xEvent.getKeyUp( ) )
			{
 				mCamera->setMovementDirection( eMovementDirection_None );
				return;
			}

			switch ( xEvent.getScanCode( ) )
			{
				case SDL_SCANCODE_W:
	       			{
					mCamera->setMovementDirection( eMovementDirection_W );
					break;
				}

				case SDL_SCANCODE_S:
	       			{
					mCamera->setMovementDirection( eMovementDirection_S );
					break;
				}

				case SDL_SCANCODE_A:
        			{
					mCamera->setMovementDirection( eMovementDirection_A );
					break;
				}

				case SDL_SCANCODE_D:
        			{
					mCamera->setMovementDirection( eMovementDirection_D );
					break;
				}

				default:
        			{
					mCamera->setMovementDirection( eMovementDirection_None );
					break;
				}
			}
		}

		void onMouseEvent( MouseEventInfo const & xEvent )
		{
			_checkCamera( );

			if( mFirstMouse )
			{
				mLastX = xEvent.x;
				mLastY = xEvent.y;
				mFirstMouse = false;
			}

			GLfloat xoffset = 0.0f;

			xoffset = xEvent.x - mLastX;

			if ( xEvent.x <= 0 ) // Cursor is on the left side of the screen
			{
				mCamera->setHorizontalCameraRotation( eHorizontalCameraRotation_Left );
			}
			else if( xEvent.x >= ( mW - 1 ) ) // Cursor is on the right of the screen
			{
				mCamera->setHorizontalCameraRotation( eHorizontalCameraRotation_Right );
			}
			else
			{
				mCamera->setHorizontalCameraRotation( eHorizontalCameraRotation_None );
			}

			if ( xEvent.y <= 0 ) // Cursor is at the top of the screen
			{
				mCamera->setVerticalCameraRotation( eVerticalCameraRotation_Down );
			}
			else if( xEvent.y >= ( mH - 1 ) ) // Cursor is at the right of the screen
			{
				mCamera->setVerticalCameraRotation( eVerticalCameraRotation_Up );
			}
			else
			{
				mCamera->setVerticalCameraRotation( eVerticalCameraRotation_None );
			}

			GLfloat yoffset = mLastY - xEvent.y; // Reversed since y-coordinates go from bottom to left

			mLastX = xEvent.x;
			mLastY = xEvent.y;

			mCamera->rotateCamera( xoffset, yoffset );

			// TODO: Fix. Event has no 'offset' given, just whether the wheel has moved up or down.
			GLfloat const scrollOffset = xEvent.wheelHasMovedUp( ) ?
							     sFovStep : ( xEvent.wheelHasMovedDown( ) ?
								  	  -sFovStep : 0.0f );

			mCamera->setFov( scrollOffset );
		}

		void render( std::function<void(void)> xLambda
			   , glm::vec3 const & xPosition
			   , GLfloat const xRotate
			   , glm::vec3 const & xScale )
		{
			_render([&]() {

				_setModelMatrix( xPosition, xRotate, xScale );

				xLambda( );
			});
		}
		// TODO: Parameterize this so that translate, rotate, and scale are parameters.
		// We will draw a single cube instead (less efficient but more to our use case.)
		// Or specify a list of N cubes and their positions (better )
		void render3DScene( )
		{
			_render( [&]() {
				_setModelMatrix( glm::vec3( -10, -10, -10 )
					       , 90
					       , glm::vec3( 1000.0f
 						          , 1000.0f
						          , 1000.0f ) );

				mPlane.render( mFourthTexture, GL_TEXTURE1 );

				_checkForGLError( "After render plane." );

				glActiveTexture( GL_TEXTURE1 );

				mThirdTexture([&]( )
				{
					glActiveTexture( GL_TEXTURE2 );

					mSecondTexture([&]( )
					{
						_drawCubes( 20.0f );
					} );
				} );

				glActiveTexture( GL_TEXTURE1 );

				auto __drawCubeSeries = [&]( Texture2D & xTex, GLint const xPosition )
				{
					xTex([&]( )
					{
						_drawCube( glm::vec3( xPosition, -5, -5 )
							 , 0
							 , glm::vec3( 1.0f, 1.0f, 1.0f ) );
					} );
				};

				__drawCubeSeries( mSecondTexture, -1 );
				__drawCubeSeries( mThirdTexture , -2 );
				__drawCubeSeries( mFourthTexture, -3 );
				__drawCubeSeries( mFifthTexture , -4 );
				__drawCubeSeries( mSixthTexture , -5 );
			} );
		}

		void updateCameras( )
		{
			_checkCamera( );

			GLfloat const currentFrame = mDeltaWatch.stop( );

			mDeltaTime = currentFrame - mLastFrame;

			mLastFrame = currentFrame;

			mCamera->moveCamera( mDeltaTime );

			mCamera->handleCameraRotation( );

		}
};

    // Set up vertex data (and buffer(s)) and attribute pointers
GLfloat const CubeRenderer::sVertices[] = {
       	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec3 const CubeRenderer::sCubePositions[] = {
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
};

class  Sprite
{
	public:
		Sprite( )
			: tex( )
                        , texData( NULL )
			, w( 0 )
			, h( 0 )
			, x( 0 )
			, y( 0 )
			, initialized( false )
		{
                        ;
                }

		virtual ~Sprite( )
		{
			if (texData != NULL )
			{
				free( texData );
			}
		}

		bool initialized;

		Texture2D tex;

		GLubyte * texData;

		uint32_t w, h, x, y;

		void initialize( )
		{
			if ( initialized )
			{
				std::cerr << "Texture was already initialized." << std::endl;
				exit( -1 );
			}

			if ( texData == NULL )
			{
				std::cerr << "Attempted to initialize sprite with null data. " << std::endl;
				exit( -1 );
			}

			tex.initialize( w, h, texData );

			initialized = true;
		}
};

class TextRenderer
{
	GLCharacterMap mDefaultWriter;

	typedef std::map< char, Sprite * > SpriteFont;
	SpriteFont mSpriteFont;

	uint32_t mW, mH;

	Quad mQuad;

        static Sprite * _loadGLCharacterIntoFont( SpriteFont & xMap, GLCharacter * xGLChar )
        {
                if ( xGLChar == NULL )
                {
                        std::cerr << "GLCharacter was NULL." << std::endl;
                        exit( -1 );
                }

                if ( xMap.find( xGLChar->getCharacter( ) ) != xMap.end( ) )
                {
                        // character already loaded!
                        return xMap[ xGLChar->getCharacter( ) ];
                }

                Sprite * pT = new Sprite( );

                pT->texData = xGLChar->allocGLBuffer( );

                pT->tex.initialize( ( pT->w = xGLChar->getWidth( )  )
                                  , ( pT->h = xGLChar->getHeight( ) )
                                  , ( uint8_t* )pT->texData );

                xMap[ xGLChar->getCharacter( ) ] = pT;

                return pT;
        }

	public:
		TextRenderer( )
			: mDefaultWriter( "resources/terminus.ttf"
					, 32
					, "" )
			, mSpriteFont( )
			, mW( 0 )
			, mH( 0 )
		{
	                for ( auto && ii : mSpriteFont )
                        {
                                delete ii.second;
                        }
		}

		void initialize( uint32_t const xW, uint32_t const xH )
		{
			mW = xW;
			mH = xH;

                        for ( char c = '!'; c < '~'; c++ )
			{
				_loadGLCharacterIntoFont( mSpriteFont, mDefaultWriter[ c ] );
			}

			mQuad.initialize( );
		}

		void drawString( Overlay2D * xOverlay
				, const char * xString
			        , glm::vec2 const & xPosition )
		{
			GLuint offsetx = xPosition.x, offsety = xPosition.y;

			for ( size_t ii = 0; ii < strlen( xString ); ++ii )
			{
				//mDefaultWriter[ ii ];
				char const c = xString[ ii ];

				Sprite * pT = _loadGLCharacterIntoFont( mSpriteFont, mDefaultWriter[ c ] );

                                if ( c == '\n' )
                                {
                                        offsety += pT->h;
                                        offsetx = xPosition.x;
                                        continue;
                                }

                                if ( c == '\t' )
                                {
                                        offsetx += pT->w * 8;
                                        continue;
                                }

				glActiveTexture( GL_TEXTURE3 );

                                xOverlay->drawTexture( pT->tex
                                          	     , glm::vec2( offsetx, offsety )
	                                             , glm::vec2( pT->w, pT->h )
         	                                     , 0.0f
                  	                             , glm::vec4( 1.0f, 1.0f, 1.0f, 0.5f ) );

                                if ( ( offsetx += pT->w ) >= mW )
                                {
                                        offsetx = xPosition.x;
                                        offsety += pT->h;
                                }
			}
		}

		// TODO: fix..
		void drawString( CubeRenderer * xCubeRenderer
				, const char * xString
				, glm::vec3 const & xPosition
				, GLfloat const xRotate
				, glm::vec3 const & xScale )
		{

			GLuint offsetx = xPosition.x, offsety = xPosition.y;

			for ( size_t ii = 0; ii < strlen( xString ); ++ii )
			{
				char const c = xString[ ii ];

				Sprite * pT = _loadGLCharacterIntoFont( mSpriteFont, mDefaultWriter[ c ] );

                                if ( c == '\n' )
       	                        {
                                        offsety += pT->h;
                                        offsetx = xPosition.x;
                                        continue;
                                }

                                if ( c == '\t' )
                                {
                                        offsetx += pT->w * 8;
                                        continue;
                                }

				glActiveTexture( GL_TEXTURE3 );

				xCubeRenderer->render( [&]( ) { mQuad.render( pT->tex, GL_TEXTURE1 ); }
						     , glm::vec3( offsetx, offsety, xPosition.z )
						     , xRotate
						     , xScale );

                                if ( ( offsetx += pT->w ) >= mW )
                                {
                                        offsetx = xPosition.x;
                                        offsety += pT->h;
                                }
			}
		}
};

class CursorRenderer
{
	Sprite * mCursor;

#ifdef LINUX
	Display * mDisplay;
	bool mCursorGrabbed;
#endif
	public:
		CursorRenderer( )
			: mCursor( NULL )
#ifdef LINUX
			, mDisplay( XOpenDisplay( NULL ) )
			, mCursorGrabbed( false )
#endif
		{
			;
		}

		~CursorRenderer( )
		{
#ifdef LINUX
			if ( mCursorGrabbed )
			{

			}

			if ( mDisplay != NULL )
			{
				XCloseDisplay( mDisplay );
			}
#endif
		}

		void initialize( uint32_t const xW, uint32_t const xH, const char * xFilename )
		{
			PNGImage image( xFilename );

			if ( !image.getIsValid( ) )
			{
				std::cerr << "Failed to load cursor image " << xFilename << std::endl;
				exit( -1 );
			}

	                Sprite * pT = new Sprite( );

	                pT->texData = image.toGLTextureBuffer( );

	                pT->tex.initialize( ( pT->w = xW )
	                                  , ( pT->h = xH )
	                                  , ( uint8_t* )pT->texData );

			mCursor = pT;
#if 0 // Linux
			Window w;
			int revert;

			XGetInputFocus( mDisplay, &w, &revert );

			int result = GrabSuccess;

			if ( result = XGrabPointer( mDisplay
					 	  , w
						  , False
						  , 0
						  , GrabModeAsync
						  , GrabModeAsync
						  , w
						  , None
						  , CurrentTime ) != GrabSuccess )
			{
				std::cerr << "XGrabPointer failed!" << std::endl;
				exit( -1 );
			}

			mCursorGrabbed = true;
#endif
		}

		void drawCursor( Overlay2D * xOverlay, uint32_t const xX, uint32_t const xY )
		{
			if ( xOverlay == NULL )
			{
				std::cerr << "NULL overlay passed to " << __PRETTY_FUNCTION__ << std::endl;
				exit( -1 );
			}

			if ( mCursor == NULL )
			{
				std::cerr << "Cursor was NULL. Was drawCursor( .. ) called before initialize( .. )?" << std::endl;
				exit( -1 );
			}

			glActiveTexture( GL_TEXTURE4 );

                        xOverlay->drawTexture( mCursor->tex
                                      	     , glm::vec2( xX, xY )
                                             , glm::vec2( mCursor->w, mCursor->h )
      	                                     , 0.0f
               	                             , glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
		}

		void hideExternalCursor( )
		{
#ifdef WIN32
// On windows, the pointer is known to reappear in the window if alt-tab is performed (happens often in games such as 
// skyrim). Therefore, we hide it every frame. It is an inexpensive call.

			ShowCursor( FALSE );
			SetCursor( NULL );
#elif defined(LINUX)
// On linux, we only grab the pointer once and pray the window does not reset its pointer.
// TODO: Fix cursor hiding on linux! Need help from someone who understands X11 lib better.
#else
			std::cerr << __PRETTY_FUNCTION__ << " is not implemented on this platform." << std::endl;
			exit( -1 );
#endif

		}

};

} // namespace sisu;


using namespace sisu;

namespace {

class HelloInstancing_UT : public context
{
	public:
		HelloInstancing_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

class HelloInstancing : public SDLTestWindow
{
	Mouse mMouse;
	Keyboard mKeyboard;

	Overlay2D mOverlay2D;

	CubeRenderer mCubeRenderer;

	TextRenderer mTextRenderer;

	const char * mSkyboxFaces[ 6 ] = { "resources/testinput/right.png"
					 , "resources/testinput/left.png"
					 , "resources/testinput/top.png"
					 , "resources/testinput/bottom.png"
					 , "resources/testinput/back.png"
					 , "resources/testinput/front.png" };
	Skybox mSkybox;

	CursorRenderer mCursorRenderer;
	MouseEventInfo mCursorPosition;

	Camera mCamera;

	Texture2D mTexture;
	PNGImage mPNGImage;

	Oscillator<int32_t> mOscW, mOscH;

	Oscillator<GLfloat> mOscR, mOscAlpha;

	SDLShader mModelShader;

	Model mNanosuitGuy;

	void _initialize2DOverlay( )
	{
		mOverlay2D.initialize( mW, mH );

		if ( !mPNGImage.getIsValid( ) )
		{
			std::cerr << "Unit test resource testinput01.png not found or is corrupt." << std::endl;
			exit( -1 );
		}

		mTexture.initialize( mPNGImage.getWidth( )
				   , mPNGImage.getHeight( )
				   , mPNGImage.toGLTextureBuffer( ) );
	}

	void _renderNanosuitGuy( )
	{

		mModelShader([&]( ) {

		        glm::mat4 projection = glm::perspective(mCamera.getFOV( ), (float)mW/(float)mH, 0.1f, 100.0f);
		        mModelShader.getUniforms().setUniformMatrix4fv("projection", projection);
		        mModelShader.getUniforms().setUniformMatrix4fv("view", mCamera.getViewMatrix( ) );

		        // Draw the loaded model
			glm::mat4 model;
		        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // It's a bit too big for our scene, so scale it down
		        mModelShader.getUniforms().setUniformMatrix4fv("model", model);
			mNanosuitGuy.render( mModelShader );

		} );

	}

	protected:
		virtual void render( )
		{
			// Set the viewport
			glViewport( 0, 0, mW, mH );

			// Clear the color buffer
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

			mSkybox.render( mCamera.getViewMatrix( ), mCamera.getFOV( ) );

			//mCubeRenderer.render3DScene( );

			_renderNanosuitGuy( );

			glDepthMask( GL_FALSE );

			glActiveTexture( GL_TEXTURE5 );

			mOverlay2D.drawTexture( mTexture
		 			      , glm::vec2( 0, 0 )
 	  			 	      , glm::vec2( ++mOscW, ++mOscH )
					      , ++mOscR
			    		      , glm::vec4( dRand( 0.0f, 1.0f )
					       		 , dRand( 0.0f, 1.0f )
	                                       		 , dRand( 0.0f, 1.0f )
					       		 , ++mOscAlpha ) );

			std::stringstream versionString;

			versionString << "Vendor:       " << glGetString( GL_VENDOR                   ) << std::endl
				      << "Renderer:     " << glGetString( GL_RENDERER                 ) << std::endl
				      << "Version:      " << glGetString( GL_VERSION                  ) << std::endl
				      << "GLSL Version: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

			mTextRenderer.drawString( &mOverlay2D
						, versionString.str( ).c_str( )
					        , glm::vec2( mW / 3, mH / 2 ) );


			mCursorRenderer.drawCursor( &mOverlay2D
						  , mCursorPosition.x
						  , mCursorPosition.y );

			glDepthMask( GL_TRUE );


			_checkForGLError( "After render" );
		}

	public:
		HelloInstancing( )
			: SDLTestWindow( )
			, mMouse( )
			, mTexture( )
			, mOverlay2D( )
			, mCubeRenderer( )
			, mCursorPosition( )
			, mCamera( )
			, mTextRenderer( )
			, mSkybox( mSkyboxFaces )
			, mPNGImage( "resources/testinput/testinput16.png" )
			, mOscW( 0, 0, 0, 25 )
			, mOscH( 0, 0, 0, 25 )
			, mOscR( 0.0f, 0.0f, 360.0f, 0.10f )
			, mOscAlpha( 1.0f, 0.0f, 1.0f, 0.05f )
			, mModelShader( ShaderSourcePair( "#version 300 es                                                      \n"
							  "layout (location = 0) in vec3 position;				\n"
							  "layout (location = 1) in vec3 normal;				\n"
							  "layout (location = 2) in vec2 texCoords;				\n"
							  "out vec2 TexCoords;							\n"
							  "uniform mat4 model;							\n"
							  "uniform mat4 view;							\n"
							  "uniform mat4 projection;						\n"
							  "void main()								\n"
							  "{									\n"
							  "    gl_Position = projection * view * model * vec4(position, 1.0f);	\n"
							  "    TexCoords = texCoords;						\n"
							  "}									\n"
							 ,"#version 300 es                                                      \n"
				    	                  "precision mediump float;				                \n"
							  "in vec2 TexCoords;							\n"
							  "out vec4 color;							\n"
							  "uniform sampler2D texture_diffuse1;					\n"
							  "void main()								\n"
							  "{									\n"
							  "    color = vec4(texture(texture_diffuse1, TexCoords));		\n"
							  "}									\n" ) )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			mModelShader.initialize( );

			mNanosuitGuy.initialize( "resources/objects/nanosuit/nanosuit.obj" );

			_initialize2DOverlay( );

			mCubeRenderer.initialize( mW, mH, &mCamera );
			mTextRenderer.initialize( mW, mH );

			static uint32_t const sCursorWidth  = 32;
			static uint32_t const sCursorHeight = 32;

			mCursorRenderer.initialize( sCursorWidth
						  , sCursorHeight
						  , "resources/testinput/cursor.png" );

			mSkybox.initialize( mW, mH );

			mOscW.setMinMax( 0, mW );
			mOscH.setMinMax( 0, mH );

			mMouse.registerCallback([&](MouseEventInfo const & xEvent){

				mCursorPosition = xEvent;

				mCubeRenderer.onMouseEvent( xEvent );
			});

			mMouse.listen( );

			mKeyboard.registerCallback( [&]( KeyboardEvent const & xEvent ){
				mCubeRenderer.onKeyboardEvent( xEvent );
			});

			mKeyboard.listen( );
		}

		virtual void run( )
		{
			Stopwatch t;

			double accum = 0.0
			     , fps   = 0.0;

			uint32_t frames = 0;

			std::stringstream ss;

			do
			{
				t.startMs( );

				mCubeRenderer.updateCameras( );

				mCursorRenderer.hideExternalCursor( );

				render( );

				// TODO: Ideally, we should be able to compute the text size based on the font
				// for a given string and use that as our offset.
				mTextRenderer.drawString( &mOverlay2D
							, ss.str( ).c_str( )
							, glm::vec2( mW - 225, mH - 150 ) );

				ss.str("");

				_checkForGLError( );

				SDL_PumpEvents( );

				glFlush( );

				SDL_GL_SwapWindow( mMainWindow );


				if ( ( accum += t.stop( ) ) >= 600000.0 )
				{
					break;
				}

				++frames;

				fps = accum / frames;

				ss << "FPS: " << fps;


			} while ( 1 );

			mMouse.stopListening( );
			mKeyboard.stopListening( );

			_hide( );
		}
};

} // namespace

TEST(HelloInstancing_UT, HelloInstancing)
{
	HelloInstancing * window = new HelloInstancing;
	window->initialize( { 3, 0, 1, false, false } );
	window->run( );
	delete window;
}
#endif // OPENGLES
#endif // OPENGLES_HELLOINSTANCING_UT

