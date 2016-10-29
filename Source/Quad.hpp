#ifndef QUAD_4AC90298F79D48DB9104607B24BEDC41_HPP_
#define QUAD_4AC90298F79D48DB9104607B24BEDC41_HPP_

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

namespace sisu {

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

} // namespace sisu

#endif // QUAD_4AC90298F79D48DB9104607B24BEDC41_HPP_
