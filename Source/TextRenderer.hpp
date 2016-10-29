#ifndef TEXT_RENDERER_FDEC6541F53A4DF3806C0E8177407ACA_HPP_
#define TEXT_RENDERER_FDEC6541F53A4DF3806C0E8177407ACA_HPP_

#include "GLCharacterMap.hpp"
#include "Overlay2D.hpp"
#include "Sprite.hpp"
#include "Quad.hpp"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <map>

namespace sisu {

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
                  	                             , glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

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

} // namespace sisu

#endif // TEXT_RENDERER_FDEC6541F53A4DF3806C0E8177407ACA_HPP_
