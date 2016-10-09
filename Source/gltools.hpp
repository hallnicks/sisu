#ifndef GL_TOOLS_HPP_
#define GL_TOOLS_HPP_

#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

namespace sisu {
// thanks to stack overflow/ nehe
void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;

    //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
    fH = tan( fovY / 360 * pi ) * zNear;
    fW = fH * aspect;

    glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

void lookAtGL(glm::vec3 const & pos, glm::vec3 const & dir, glm::vec3 const & up )
{
	glm::vec3 dirN;
	glm::vec3 upN;
	glm::vec3 rightN;

	dirN = dir;
	glm::normalize(dirN);

	upN = up;
	glm::normalize(upN);

	rightN = glm::cross( dirN, upN );
	glm::normalize(rightN);

	upN = glm::cross( rightN, dirN );
	glm::normalize(upN);

        float mat[16];
	mat[ 0] = rightN.x;
	mat[ 1] = upN.x;
	mat[ 2] = -dirN.x;
	mat[ 3] = 0.0;

	mat[ 4] = rightN.y;
	mat[ 5] = upN.y;
	mat[ 6] = -dirN.y;
	mat[ 7] = 0.0;

	mat[ 8] = rightN.z;
	mat[ 9] = upN.z;
	mat[10] = -dirN.z;
	mat[11] = 0.0;

	mat[12] = -glm::dot(rightN, pos);
	mat[13] = -glm::dot(upN   , pos);
	mat[14] =  glm::dot(dirN  , pos);
	mat[15] = 1.0;

        glMultMatrixf(&mat[0]);
}


} // namespace

#endif // GL_TOOLS_HPP_
