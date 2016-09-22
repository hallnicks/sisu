#ifndef SPRITE_P497F0A5CA9D34390BCB629AB6F728E0E_HPP_
#define SPRITE_P497F0A5CA9D34390BCB629AB6F728E0E_HPP_

#include "Texture.hpp"
#include "Rect.hpp"
#include "TexturedVertex2D.hpp"

#include <vector>

namespace sisu {

enum eSpriteOrigin
{
	eSpriteOrigin_Center,
	eSpriteOrigin_TopLeft,
	eSpriteOrigin_BottomLeft,
	eSpriteOrigin_TopRight,
	eSpriteOrigin_BottomRight
};

class Sprite : public Texture 
{
	protected:
		std::vector<Rect> mClips;
		
		GLuint 	 mVertexDataBuffer;
		GLuint * mIndexBuffers;
	
		virtual void _freeSprite( )
		{
			if ( mVertexDataBuffer != 0 )
			{
				glDeleteBuffers( 1, &mVertexDataBuffer );
				mVertexDataBuffer = 0;
			}	

			if ( mIndexBuffers != NULL )
			{
				glDeleteBuffers( mClips.size( ), mIndexBuffers );
				delete[] mIndexBuffers;
				mIndexBuffers = NULL;
			}

			mClips.clear( );
		}

		virtual void _freeTexture( )
		{
			_freeSprite( );
			Texture::_freeTexture( );
		}

	public:
		Sprite( ) 
			: mClips( )
			, mVertexDataBuffer( 0 )
			, mIndexBuffers( NULL )
		{
			;
		}

		~Sprite( )
		{
			_freeSprite( );
		}	

		int32_t addClipSprite( Rect const & xRect )
		{
			mClips.push_back( xRect );
			return mClips.size( ) - 1;
		}

		Rect getClip( int32_t const xIndex )
		{
			return mClips[ xIndex ];
		}
		
		void generateDataBuffer( eSpriteOrigin const xOrigin = eSpriteOrigin_Center )
		{
			if ( getTextureID( ) != 0 && mClips.size( ) > 0 )
			{
				int32_t const totalSprites = mClips.size( );

				TexturedVertex2D * vertexData = new TexturedVertex2D[ totalSprites * 4 ];
			
				mIndexBuffers = new GLuint[ totalSprites ];

				glGenBuffers( 1, &mVertexDataBuffer );

				glGenBuffers( totalSprites, mIndexBuffers );

				GLfloat const tW = getTextureWidth( );
				GLfloat const tH = getTextureHeight( );

				GLuint 	spriteIndices[4] = { 0, 0, 0, 0 };

				GLfloat  vTop 	 = 0.0f;
				GLfloat  vBottom = 0.0f;
				GLfloat	 vLeft   = 0.0f;
				GLfloat	 vRight  = 0.0f;

				for ( int32_t i = 0; i < totalSprites; ++i )
				{
					switch( xOrigin )
				        {
			  	              case eSpriteOrigin_TopLeft:
				                    vTop = 0.f;
				                    vBottom = mClips[ i ].h;
				                    vLeft = 0.f;
				                    vRight = mClips[ i ].w;
				                    break;

				                case eSpriteOrigin_TopRight:
				                    vTop = 0.f;
				                    vBottom = mClips[ i ].h;
				                    vLeft = -mClips[ i ].w;
				                    vRight = 0.f;
				                    break;

				                case eSpriteOrigin_BottomLeft:
				                    vTop = -mClips[ i ].h;
				                    vBottom = 0.f;
				                    vLeft = 0.f;
				                    vRight = mClips[ i ].w;
				                    break;

				                case eSpriteOrigin_BottomRight:
				                    vTop = -mClips[ i ].h;
				                    vBottom = 0.f;
				                    vLeft = -mClips[ i ].w;
				                    vRight = 0.f;
				                    break;

				                default:
				                    vTop = -mClips[ i ].h / 2.f;
				                    vBottom = mClips[ i ].h / 2.f;
				                    vLeft = -mClips[ i ].w / 2.f;
				                    vRight = mClips[ i ].w / 2.f;
			        	            break;
			            	} // switch

		                        //Top left
		                        vertexData[ spriteIndices[ 0 ] ].position.x = vLeft;
                		        vertexData[ spriteIndices[ 0 ] ].position.y = vTop;

		                        vertexData[ spriteIndices[ 0 ] ].texCoord.s =  (mClips[ i ].x) / tW;
		                        vertexData[ spriteIndices[ 0 ] ].texCoord.t =  (mClips[ i ].y) / tH;

		                        //Top right
		                        vertexData[ spriteIndices[ 1 ] ].position.x = vRight;
		                        vertexData[ spriteIndices[ 1 ] ].position.y = vTop;

		                        vertexData[ spriteIndices[ 1 ] ].texCoord.s =  (mClips[ i ].x + mClips[ i ].w) / tW;
		                        vertexData[ spriteIndices[ 1 ] ].texCoord.t =  (mClips[ i ].y) / tH;

		                        //Bottom right
		                        vertexData[ spriteIndices[ 2 ] ].position.x = vRight;
		                        vertexData[ spriteIndices[ 2 ] ].position.y = vBottom;

		                        vertexData[ spriteIndices[ 2 ] ].texCoord.s =  (mClips[ i ].x + mClips[ i ].w) / tW;
		                        vertexData[ spriteIndices[ 2 ] ].texCoord.t =  (mClips[ i ].y + mClips[ i ].h) / tH;

                		        //Bottom left
		                        vertexData[ spriteIndices[ 3 ] ].position.x = vLeft;
		                        vertexData[ spriteIndices[ 3 ] ].position.y = vBottom;

		                        vertexData[ spriteIndices[ 3 ] ].texCoord.s =  (mClips[ i ].x) / tW;
        		                vertexData[ spriteIndices[ 3 ] ].texCoord.t =  (mClips[ i ].y + mClips[ i ].h) / tH;
	
		                        //Bind sprite index buffer data
		                        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffers[ i ] );
		                        glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), spriteIndices, GL_STATIC_DRAW );

				} // for

				//Bind vertex data
		                glBindBuffer( GL_ARRAY_BUFFER, mVertexDataBuffer );
                		glBufferData( GL_ARRAY_BUFFER, totalSprites * 4 * sizeof(TexturedVertex2D), vertexData, GL_STATIC_DRAW );

		                //Deallocate vertex data
		                delete[] vertexData;
			}
			else
			{
				std::cerr << "Conditions are not met to generate buffers." << std::endl;
				exit( -1 );
			}
		}

		template< typename XRenderer >
		void render( int32_t const xIndex, XRenderer * const xShaderProgram )
		{
			if ( mVertexDataBuffer == 0 )
			{
				std::cerr << "Vertex data buffer was null." << std::endl;
				exit( -1 );
			}
			
			glBindTexture( GL_TEXTURE_2D, getTextureID( ) );

			xShaderProgram->enableVertexPointer( );
			xShaderProgram->enableTexCoordPointer( );
			
			glBindBuffer( GL_ARRAY_BUFFER, mVertexDataBuffer );

			xShaderProgram->setTexCoordPointer( sizeof(TexturedVertex2D), (GLvoid*)offsetof( TexturedVertex2D, texCoord ) );
			xShaderProgram->setVertexPointer( sizeof(TexturedVertex2D), (GLvoid*)offsetof( TexturedVertex2D, position ) );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffers[ xIndex ] );
			glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );
			
			xShaderProgram->disableVertexPointer( );
			xShaderProgram->disableTexCoordPointer( );
		}
		
		
};

} // namespace sisu

#endif // SPRITE_P497F0A5CA9D34390BCB629AB6F728E0E_HPP_

