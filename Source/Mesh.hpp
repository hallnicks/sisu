#ifndef MESH_8C3EED53D91548D09965107BF6A8431A_HPP_
#define MESH_8C3EED53D91548D09965107BF6A8431A_HPP_

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
	glm::vec3 Tangent
		, Bitangent;
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
		// Vertex Tangent
	        glEnableVertexAttribArray(3);
	        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
	        // Vertex Bitangent
	        glEnableVertexAttribArray(4);
	        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

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
			        //glDrawElements(GL_LINES, mIndices.size(), GL_UNSIGNED_INT, 0);
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

} // namespace sisu
#endif // MESH_8C3EED53D91548D09965107BF6A8431A_HPP_
