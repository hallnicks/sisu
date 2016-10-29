#ifndef  MODEL_6A2355C9890F46DBBBDF7604C840817B_HPP_
#define  MODEL_6A2355C9890F46DBBBDF7604C840817B_HPP_

#include "TGAImage.hpp"
#include "Mesh.hpp"
#include "Texture2D.hpp"

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
        	const aiScene* scene = importer.ReadFile(path
							, aiProcess_Triangulate      |
							  aiProcess_FlipUVs          |
							  aiProcess_CalcTangentSpace |
							  aiProcess_GenNormals       |
                                                          aiProcess_SplitLargeMeshes |
							  aiProcess_OptimizeMeshes );
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

			if ( xMesh->mTextureCoords[ ii ] != NULL )
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

			vector3.x = xMesh->mTangents[ ii ] .x;
			vector3.y = xMesh->mTangents[ ii ] .y;
			vector3.z = xMesh->mTangents[ ii ] .z;
			vertex.Tangent = vector3;

			vector3.x = xMesh->mBitangents[ ii ] .x;
			vector3.y = xMesh->mBitangents[ ii ] .y;
			vector3.z = xMesh->mBitangents[ ii ] .z;
			vertex.Bitangent = vector3;

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


	void _loadPNGTexture( std::vector<Texture2D> & xTextures
		   	     , const char * xPath
			     , eTexture2DType const xLocalEquivalent )
	{
		DevILImage image( xPath );

                Texture2D texture( xLocalEquivalent );

		texture.initialize( image.getWidth( )
				   , image.getHeight( )
				   , image.toGLTextureBuffer( )
				   , aiString( xPath ) );

                xTextures.push_back(texture);
                mTextures.push_back(texture);

	}

	void _loadTGATexture( std::vector<Texture2D> & xTextures
		   	     , const char * xPath
			     , eTexture2DType const xLocalEquivalent )
	{
		TGAImage image( xPath );

                Texture2D texture( xLocalEquivalent );
		texture.initialize( image.getWidth( )
				   , image.getHeight( )
				   , image.toGLTextureBuffer( )
				   , aiString( xPath ) );

                xTextures.push_back(texture);
                mTextures.push_back(texture);

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

			std::cout << "Load texture " << ss.str( ).c_str( ) << std::endl;
			_loadPNGTexture( textures, ss.str( ).c_str( ), xLocalEquivalent );
			//_loadTGATexture( textures, ss.str( ).c_str( ), xLocalEquivalent );
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

} // namespace sisu

#endif //  MODEL_6A2355C9890F46DBBBDF7604C840817B_HPP_
