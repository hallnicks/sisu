#ifndef FONT_EAF969A3C426403DA705D9614D79046D_HPP_
#define FONT_EAF969A3C426403DA705D9614D79046D_HPP_

#include "Sprite.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sisu
{

enum eFontTextAligment
{
	eFontTextAlignment_Left 	= 1,
	eFontTextAlignment_Centered_H 	= 2,
	eFontTextAlignment_Right 	= 4,
	eFontTextAlignment_Top 		= 8,
	eFontTextAlignment_Centered_V   = 16,
	eFontTextAlignment_Bottom       = 32,
};

static struct _initializeFreeType
{
        FT_Library mLibrary;

        public:
                _initializeFreeType( )
                        : mLibrary( 0 )
                {
                        FT_Error const error = FT_Init_FreeType( &mLibrary );

                        if ( error || mLibrary == 0 )
                        {
                                std::cerr << "FT_Init_FreeType( .. ) failed. " << error << std::endl;
                                exit( -1 );
                        }

                        std::cout << "Initialized Freetype. " << std::endl;
                }

                ~_initializeFreeType( )
                {
                        FT_Done_FreeType( mLibrary );
                }
} sFT2;

class Font : public Sprite
{
	GLfloat mSpace, mLineHeight, mNewLine;

	GLfloat _subStringWidth( const char* xSubString )
	{
		GLfloat subWidth = 0.0f; 

		for ( int32_t ii = 0; ii < xSubString[ ii ] != '\0' && xSubString[ ii ] != '\n'; ++ii )
		{
			if ( xSubString[ ii ] == ' ' )
			{
				subWidth += mSpace;
			}
			else
			{
				GLuint const ascii = (unsigned char)xSubString[ ii ];
				subWidth += mClips[ ascii ].w;
			}
		}

		return subWidth;
	}

	GLfloat _stringHeight( const char* xString )
	{
		GLfloat height = mLineHeight;

		for ( int32_t ii = 0; xString[ ii ] != '\0'; ++ii )
		{
			if ( xString[ ii ] == '\n' )
			{
				height += mLineHeight;
			}
		}

		return height;
	}

	protected:
		virtual void _freeFont( )
		{
			Sprite::_freeTexture( );
		}

	public:
		Font( ) 
			: Sprite( ) 
			, mSpace( 0.0f )
			, mLineHeight( 0.0f )
			, mNewLine( 0.0f )
		{
			;
		}

		~Font( )
		{
			_freeFont( );
		}

		template< typename XRenderer >
		void render( GLfloat const xX
			   , GLfloat const xY
			   , std::string const & xText
			   , XRenderer * const xShaderProgram
			   , Rect * xArea = NULL
			   , int32_t xAlignFlags = eFontTextAlignment_Left )
		{
			
			if ( getTextureID( ) == 0 )
			{
				std::cerr << "font texture was not found. Cannot render." << std::endl;
				exit( -1 );
			}

			GLfloat dx = xX;
			GLfloat dy = xY;
			
			if ( xArea != NULL ) 
			{
				if ( xAlignFlags == 0 )
				{
					xAlignFlags = eFontTextAlignment_Left | eFontTextAlignment_Top;
				}	
	
				// horizonal alignment
				if ( xAlignFlags & eFontTextAlignment_Left )
				{
					dx = xArea->x;
				}
				else if( xAlignFlags & eFontTextAlignment_Centered_H )
				{
					dx = xArea->x + ( xArea->w + _subStringWidth( xText.c_str( ) ) ) / 2.0f;
				}
				else if ( xAlignFlags & eFontTextAlignment_Right )
				{
					dx = xArea->x + ( xArea->w - _subStringWidth( xText.c_str( ) ) );
				}

				// vertical alignment
				if ( xAlignFlags & eFontTextAlignment_Top )
				{
					dy = xArea->y;
				} 
				else if ( xAlignFlags & eFontTextAlignment_Centered_V )
				{
					dy = xArea->y + ( xArea->h - _stringHeight( xText.c_str( ) ) ) / 2.0f;
				}
				else if ( xAlignFlags & eFontTextAlignment_Bottom ) 
				{
					dy = xArea->y + ( xArea->h - _stringHeight( xText.c_str( ) ) );
				}
			}

			xShaderProgram->leftMultModelView( glm::translate<GLfloat>( glm::mat4( ), glm::vec3( dx, dy, 0.0f ) ) );

			glBindTexture( GL_TEXTURE_2D, getTextureID( ) );

			xShaderProgram->enableVertexPointer( );
			xShaderProgram->enableTexCoordPointer( );

			glBindBuffer( GL_ARRAY_BUFFER, mVertexDataBuffer );

			xShaderProgram->setTexCoordPointer( sizeof(TexturedVertex2D), (GLvoid*)offsetof(TexturedVertex2D, texCoord ) );
			xShaderProgram->setVertexPointer( sizeof(TexturedVertex2D), (GLvoid*)offsetof(TexturedVertex2D, position) );

			for ( uint32_t i = 0; i < xText.length( ); ++i ) 
			{
				if ( xText[ i ] == ' ' )
				{
					xShaderProgram->leftMultModelView( glm::translate<GLfloat>( glm::mat4( )
												   , glm::vec3( mSpace, 0.0f, 0.0f ) ) );
					xShaderProgram->updateModelView( );

					dx += mSpace;
				}
				else if ( xText[ i ] == '\n' )
				{
					GLfloat targetX = xX; 
					if ( xArea != NULL )
					{
						if ( xAlignFlags & eFontTextAlignment_Left )
						{
							targetX = xArea->x;
						} 
						else if ( xAlignFlags & eFontTextAlignment_Centered_H )
						{
							targetX = xArea->x + ( xArea->w - _subStringWidth( &xText.c_str()[ i + 1 ] ) ) / 2.f;
						}
						else if ( xAlignFlags & eFontTextAlignment_Right )
						{
							targetX = xArea->x + ( xArea->w - _subStringWidth( &xText.c_str()[ i + 1 ] ) );
						}
					}

					xShaderProgram->leftMultModelView( glm::translate<GLfloat>( glm::mat4( ), glm::vec3( targetX - dx, mNewLine, 0.0f ) ) );
					xShaderProgram->updateModelView( );

					dy += mNewLine;
					dx += targetX - dx;
				}
				else
				{
					// TODO: Add unicode support...
					GLuint ascii = (unsigned char)xText[ i ];

					xShaderProgram->updateModelView( );

					glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffers[ ascii ] );
		
					glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

					xShaderProgram->leftMultModelView( glm::translate<GLfloat>( glm::mat4(), glm::vec3( mClips[ ascii ].w, 0.f, 0.f ) ) );
					
					xShaderProgram->updateModelView( );

					dx += mClips[ ascii ].w;
				}
				
			}
			
			xShaderProgram->disableVertexPointer( );

			xShaderProgram->disableTexCoordPointer( );
		}

		void loadBitmap( std::string  const & xPath )
		{
			GLubyte const BLACK_PIXEL = 0x00;

			_freeFont( );

			Texture::fromFileRGBA8( xPath );
		
			GLfloat cellW = getImageWidth( );
			GLfloat cellH = getImageHeight( );

			GLuint top     = cellH;
			GLuint bottom  = 0;
			GLuint aBottom = 0;

			int pX = 0;
			int pY = 0;
			
			int bX = 0;
			int bY = 0;

			GLuint currentChar = 0;

			Rect nextClip( 0.0f, 0.0f, (GLfloat)cellW, (GLfloat)cellH);			

			for ( uint32_t rows = 0; rows < 16; ++rows )
			{
				for ( uint32_t cols = 0; cols < 16; ++cols )
				{
					bX = cellW * cols;
					bY = cellH * rows;

					 //Initialize clip
	                                nextClip.x = cellW * cols;
	                                nextClip.y = cellH * rows;

	                                nextClip.w = cellW;
	                                nextClip.h = cellH;

	                                //Find left side of character
	                                for( int pCol = 0; pCol < cellW; ++pCol )
	                                {
	                                        for( int pRow = 0; pRow < cellH; ++pRow )
	                                        {
        	                                    //Set pixel offset
                	                                pX = bX + pCol;
	                                                pY = bY + pRow;
		
        	                                        //Non-background pixel found
        		                               if( getPixel8( pX, pY ) != BLACK_PIXEL )
                         	                       {
                                	                    //Set sprite's x offset
                                        	                nextClip.x = pX;

                                                	        //Break the loops
                                                        	pCol = cellW;
	                                                        pRow = cellH;
	                                                }
	                                        }
	                                }



	                                //Right side
	                                for( int pCol_w = cellW - 1; pCol_w >= 0; pCol_w-- )
	                                {
	                                        for( int pRow_w = 0; pRow_w < cellH; pRow_w++ )
	                                        {
	                                            //Set pixel offset
	                                                pX = bX + pCol_w;
	                                                pY = bY + pRow_w;
	
	                                                //Non-background pixel found
	                                                if( getPixel8( pX, pY ) != BLACK_PIXEL )
        	                                        {
                	                                    //Set sprite's width
                        	                                nextClip.w = ( pX - nextClip.x ) + 1;

                                	                        //Break the loops
                                        	                pCol_w = -1;
                                                	        pRow_w = cellH;
	                                                }
	                                        }
	                                }


	                                //Find Top
	                                for( int pRow = 0; pRow < cellH; ++pRow )
	                                {
	                                        for( int pCol = 0; pCol < cellW; ++pCol )
	                                        {
	                                            //Set pixel offset
	                                                pX = bX + pCol;
	                                                pY = bY + pRow;
	
	                                                //Non-background pixel found
	                                                if( getPixel8( pX, pY ) != BLACK_PIXEL )
	                                                {
	                                                        //New Top Found
	                                                        if( pRow < top )
	                                                        {
	                                                                top = pRow;
	                                                        }
	
	                                                        //Break the loops
	                                                        pCol = cellW;
	                                                        pRow = cellH;
	                                                }
        	                                }
	                                }



					//Find Bottom
	                                for( int pRow_b = cellH - 1; pRow_b >= 0; --pRow_b )
        	                        {
                	                        for( int pCol_b = 0; pCol_b < cellW; ++pCol_b )
                        	                {
                                	            //Set pixel offset
                                        	        pX = bX + pCol_b;
                                                	pY = bY + pRow_b;
	
        	                                        //Non-background pixel found
                	                                if( getPixel8( pX, pY ) != BLACK_PIXEL )
                        	                        {
                                	                        //Set BaseLine
                                        	                if( currentChar == 'A' )
                                                	        {
                                                        	        aBottom = pRow_b;
        	                                                }	
	
                	                                        //New bottom Found
                        	                                if( pRow_b > bottom )
                                	                        {
                                        	                        bottom = pRow_b;
                                                	        }

                                                        	//Break the loops
	                                                        pCol_b = cellW;
	                                                        pRow_b = -1;
	                                                }
	                                        }
	                                }

		                       mClips.push_back( nextClip );
        	                       ++currentChar;


				}

			}

			// set top
			for ( int t = 0; t < 256; ++t )
			{
				mClips[ t ].y += top;
				mClips[ t ].h -= top;
			}

			loadTextureFromPixels8( );

			Sprite::generateDataBuffer( eSpriteOrigin_TopLeft );
			
			glBindTexture( GL_TEXTURE_2D, getTextureID( ) );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

			mSpace 	    = cellW / 2;
			mNewLine    = aBottom - top;
			mLineHeight = bottom - top;
		}

		void loadFreeType( std::string const & xPath, GLuint const xPixelSize )
		{
			FT_Error error = 0;

			GLuint cellW = 0
			     , cellH = 0;

			int maxBearing = 0
			  , minHang    = 0;

		
			static uint32_t const sBitmaps = 256;
			Texture bitmaps[ sBitmaps ];

			FT_Glyph_Metrics metrics[ sBitmaps ];
	
			FT_Face face = NULL;

			std::cout << "Loading font " << xPath.c_str( ) << std::endl;
			if ( ( error = FT_New_Face( sFT2.mLibrary, xPath.c_str( ), 0, &face ) ) )
			{
				std::cerr << "FT_New_Face( .. ) failed: " << error << std::endl;
				exit( -1 );
			}

			if ( ( error = FT_Set_Pixel_Sizes( face, 0, xPixelSize ) ) )
			{
				std::cerr << "FT_Set_Pixel_Sizes( .. ) failed: " << error << std::endl;
				exit( -1 );
			}

			for ( uint32_t ii = 0; ii < sBitmaps; ++ii )
			{
				if ( ( error = FT_Load_Char( face, ii, FT_LOAD_RENDER ) ) )
				{
					std::cerr << "FT_Load_Char( .. ) failed: " << error << std::endl;
					exit( -1 );
				}

				metrics[ ii ] = face->glyph->metrics;

				bitmaps[ ii ].copyPixels8( face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows );

				if ( metrics[ ii ].horiBearingY / 64 > maxBearing )
				{
					maxBearing = metrics[ ii ].horiBearingY / 64;
				}

				if ( metrics[ ii ].width / 64 > cellW )
				{
					cellW = metrics[ ii ].width / 64;
				}

				int glyphHang = ( metrics[ ii ].horiBearingY - metrics[ ii ].height ) / 64;
				
				if ( glyphHang < minHang )
				{
					minHang = glyphHang;
				}
			}
		
			cellH = maxBearing - minHang;
			createPixels8( cellW * 16, cellH * 16 );

			GLuint currentChar = 0;
			Rect nextClip ( 0.0f, 0.0f, (GLfloat) cellW, (GLfloat) cellH );
			
			int  bx = 0
			   , by = 0;

			
			for ( unsigned int rows = 0; rows < 16; ++rows )
			{
				for ( unsigned int cols = 0; cols < 16; ++cols )
				{
					bx = cellW * cols;
					by = cellH * rows;

					nextClip.x = bx;
					nextClip.y = by;
					nextClip.w = metrics[ currentChar ].width / 64;
					nextClip.h = cellH;

					bitmaps[ currentChar ].blitPixels8( bx, by + maxBearing - metrics[ currentChar ].horiBearingY / 64, *this );
				
					mClips.push_back( nextClip );

					++currentChar;
				}
			}	
		
			Texture::padPixels8( );

			Texture::loadTextureFromPixels8( );

			generateDataBuffer( );

			glBindTexture( GL_TEXTURE_2D, getTextureID() );
		        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
		        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

			mSpace      = cellW / 2;
			mNewLine    = maxBearing;
			mLineHeight = cellH;
			
			FT_Done_Face( face );

		}
	
		GLfloat getLineHeight( ) const
		{
			std::cout << __PRETTY_FUNCTION__ << " not implemented yet." << std::endl;
			exit( -1 );
		}

		Rect getStringArea( std::string const & xText )
		{
			std::cout << __PRETTY_FUNCTION__ << " not implemented yet." << std::endl;
			exit( -1 );
		}
};


} // namespace sisu


#endif // FONT_EAF969A3C426403DA705D9614D79046D_HPP_
