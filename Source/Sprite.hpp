#ifndef SPRITE_B614392BB8B6462F9AB24933880D1329_HPP_
#define SPRITE_B614392BB8B6462F9AB24933880D1329_HPP_

#include "Texture2D.hpp"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include <iostream>

namespace sisu {

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
			if ( texData != NULL )
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

} // namespace sisu

#endif // SPRITE_B614392BB8B6462F9AB24933880D1329_HPP_
