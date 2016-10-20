#ifndef ES_SHAPES_04D4D747E0934224B063A0A6F7D7A131_HPP_
#define ES_SHAPES_04D4D747E0934224B063A0A6F7D7A131_HPP_
// Adapted from the following:

// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// ESShapes.c
//
//    Utility functions for generating shapes
//

///
//  Includes
//

#ifndef OPENGLES
#include <GL/glew.h>
#else
#include <GLES2/gl2.h>
#endif

#include <SDL2/SDL.h>

#ifndef OPENGLES
#include <SDL2/SDL_opengl.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace sisu {

class ESShapes
{
	public:
		static int esGenSphere ( int numSlices
					, float radius
					, GLfloat **vertices
					, GLfloat **normals
		                       , GLfloat **texCoords
					, GLuint **indices );

		static int esGenCube ( float scale
				     , GLfloat **vertices
				     , GLfloat **normals
				     , GLfloat **texCoords
			             , GLuint **indices );

		static int esGenSquareGrid ( int size
					   , GLfloat **vertices
					   , GLuint **indices );

}; // class ESShapes

} // namespace sisu
#endif // ES_SHAPES_04D4D747E0934224B063A0A6F7D7A131_HPP_
