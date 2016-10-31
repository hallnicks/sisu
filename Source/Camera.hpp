#ifndef CAMERA_79B84E01EB5A44C9A94F8596C9682F2E_HPP_
#define CAMERA_79B84E01EB5A44C9A94F8596C9682F2E_HPP_

#include "SDLShader.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"
#include "DevILImage.hpp"
#include "sisumath.hpp"
#include "AndroidLogWrapper.hpp"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace sisu {

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
			SISULOG("In Camera Ctor");
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

} // namespace sisu

#endif // CAMERA_79B84E01EB5A44C9A94F8596C9682F2E_HPP_
