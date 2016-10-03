
#ifndef SDL_SHADER_TEST_DBB12D8670054D2E97E6A436C1F0EC51_HPP_
#define SDL_SHADER_TEST_DBB12D8670054D2E97E6A436C1F0EC51_HPP_

#include "SDLTest.hpp"
#include "ioassist.hpp"
#include "Font.hpp"
#include "VBM.hpp"
#include "threadgears.hpp"

#include <string>
#include <iostream>
#include <functional>
#include <limits>
#include <map>
#include <vector>

#include <concurrentqueue.h>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.H>
#include <GL/glu.h>

#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>


#include "Texture2D.hpp"
#include "GLCharacterMap.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "memblock.hpp"
#include "word.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace sisu {

struct ShaderSourcePair
{
	ShaderSourcePair( const char * xV, const char* xF )
		: VertexSource( xV )
		, FragmentSource( xF )
	{
		;
	}

	char const * const VertexSource;
	char const * const FragmentSource;
};

struct ShaderPathPair
{
	ShaderPathPair( const char * xV, const char * xF )
		: VertexPath( xV )
		, FragmentPath( xF )
	{
		;
	}

	char const * const VertexPath;
	char const * const FragmentPath;
};

class SDLShader
{
	std::string mVertexShaderSource
		  , mFragmentShaderSource;

	protected:
		GLuint mProgramID;

	public:
		SDLShader( ShaderSourcePair const & xShaderSources );
		SDLShader( ShaderPathPair const & xShaderPaths );
		~SDLShader( );

		class Uniforms
		{
			GLuint const mProgramID;

			Uniforms( GLuint const xProgramID )
				: mProgramID( xProgramID )
			{
				;
			}

			friend class SDLShader;

			public:
				GLint operator [ ] ( const char * xName )
				{
					return glGetUniformLocation( mProgramID, xName );
				}

				void setUniform1i( const char * xName, GLint const xValue )
				{
					glUniform1i( (*this)[ xName ], xValue );
				}

				void setUniform1f( const char * xName, GLfloat const xValue )
				{
					glUniform1f( (*this)[ xName ], xValue );
				}

				void setUniformMatrix4fv( const char * xName, glm::mat4 const & xMatrix )
				{
					glUniformMatrix4fv( (*this)[ xName ], 1, GL_FALSE, glm::value_ptr( xMatrix ) );
				}

				void setUniformVector3f( const char * xName, glm::vec3 const & xValue )
				{
					glUniform3f( (*this)[ xName ], xValue.x, xValue.y, xValue.z );
				}
		};

		virtual void initialize( );

		virtual SDLShader & operator( )( std::function<void(void)> xLambda );

		Uniforms getUniforms() { return Uniforms( mProgramID ); }

		GLuint getProgramID( ) const { return mProgramID; }
};

class SDLTestShaderWindow : public SDLTestWindow
{
	protected:
		SDLShader mShader;

	public:
		SDLTestShaderWindow( ShaderSourcePair const & xShaderSources );
		SDLTestShaderWindow( ShaderPathPair const & xShaderPaths );
		~SDLTestShaderWindow( );

		virtual void render( );

		virtual void run( );

		virtual SDLTestShaderWindow & operator( )( std::function<void(void)> xLambda );

		virtual void initialize( OpenGLAttributes const & xOpenGLAttributes );
};

class SDLQuadShader : public SDLTestShaderWindow
{
	GLint mVertexPos2DLocation;

	GLuint mVBO, mIBO;

	protected:
		virtual void render( );
	public:
		SDLQuadShader( );

		virtual void initialize( OpenGLAttributes const & xAttributes );
};


class SpriteShader : public SDLTestWindow
{
	// TODO: unify these constants into a config.
	static constexpr int32_t const sFuzzPixels   = 64;
	static constexpr int32_t const sBorderWidth  = 4;
	static constexpr int32_t const sCaptionWidth = 64;

	SDLShader mSpriteShader;

	Texture2D mBackgroundTexture;

	GLubyte * mRandomData;

	class  _TextureInstance
	{
		public:
			_TextureInstance( )
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

			virtual ~_TextureInstance( )
			{
				if (texData != NULL )
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
				tex.initialize( w, h, texData );
				initialized = true;
			}
	};

	class _SubWindow : public _TextureInstance
	{
		public:
			_SubWindow( _TextureInstance * xCloseButton )
				: stdIn( )
				, isSelected( false )
				, hasUpdates( false )
				, originX( 0 )
				, originY( 0 )
				, resizeX( false )
				, resizeY( false )
				, hasFocus( false )
				, closeButton( xCloseButton )
			{
				;
			}

			moodycamel::ConcurrentQueue< char > stdIn;

			bool isSelected
			   , hasUpdates
			   , resizeX
			   , resizeY
			   , hasFocus;

			uint32_t originX, originY;

			_TextureInstance * closeButton;
	};

	_TextureInstance * mShown;

	typedef std::map<char, _TextureInstance * > CharacterMap;

	CharacterMap mCharacters, mHighlightedCharacters, mTitleCharacters;

	_TextureInstance * mCursor;

	GLuint mVBO, mQuadVAO;

	event mQuit;

	bool mPBOEnabled;

	GLuint mPBO[2];

	uint64_t mSize;

	GLCharacterMap mCharacterMap, mHighlightedCharacterMap, mTitleCharacterMap;

	Mouse mMouse;
	Keyboard mKB;

	mutex mMouseMutex;

	MouseEventInfo mCursorPosition;

	moodycamel::ConcurrentQueue< char > mStdIn;

	moodycamel::ConcurrentQueue< _TextureInstance * > mTextureQueue;

	std::vector<char> mLineBuffer;
	typedef std::map< uint32_t, _SubWindow * > _InnerWindowMap;
	typedef _InnerWindowMap::iterator 	   _SubWindowIterator;

	std::map< uint32_t, _InnerWindowMap > mSubWindows;

	_SubWindow * mSelectedWindow, * mFocusedWindow;

	void _drawSprite( Texture2D & xTexture
		        , glm::vec2 const xPosition
			, glm::vec2 const xSize
			, GLfloat xRotate
			, glm::vec3 const xColor )
	{
		mSpriteShader( [ & ] ( )
		{
			glm::mat4 model;

			model = glm::translate( model, glm::vec3( xPosition, 0.0f ) );

			model = glm::translate( model, glm::vec3( 0.5f * xSize.x, 0.5f * xSize.y, 0.0f ) );

			model = glm::rotate(    model, xRotate, glm::vec3( 0.0f, 0.0f, 1.0f ) );

			model = glm::translate( model, glm::vec3( -0.5f * xSize.x, -0.5f * xSize.y, 0.0f ) );

			model = glm::scale(     model, glm::vec3( xSize, 1.0f ) );

			mSpriteShader.getUniforms( ).setUniformMatrix4fv( "model", model );

			mSpriteShader.getUniforms( ).setUniformVector3f( "spriteColor", xColor );

			glActiveTexture( GL_TEXTURE0 );

			xTexture( [ & ]( )
			{
				glBindVertexArray( mQuadVAO );

				glDrawArrays( GL_TRIANGLES, 0, 6 );

				glBindVertexArray( 0 );
			} );
		} );
	}

	union _RGBA
	{
	        _RGBA( )
	                  : integer( 0 )
	        {
	               ;
	        }
	        _RGBA2( uint8_t const xR
	                    , uint8_t const xG
	                    , uint8_t const xB
	                    , uint8_t xA = 255 )
	        {
	                r = xR;
	                g = xG;
        	        b = xB;
	                a = xA;
	        }

	        uint32_t integer;
#ifdef SISU_BIG_ENDIAN_
		struct { uint8_t a, b, g, r; };
#else
	        struct { uint8_t r, g, b, a; };
#endif
	};

	void _printMap( )
	{
		for ( auto && ymap : mSubWindows )
		{
			for ( auto && xmapWindowPair : ymap.second )
			{
				_SubWindow * w = xmapWindowPair.second;

				if ( w == NULL )
				{
					std::cout << "[null window pointer]" << std::endl;
					continue;
				}

				std::cout << "[" << ymap.first << "]"
					  << "[" << xmapWindowPair.first << "]"
					  << " = SubWindow( "
					  << "x=" 	<< w->x       << ", y="      << w->y
					  << "w=" 	<< w->h       << ", h="      << w->h
					  << "originX=" << w->originX << ", originY" << w->originY
					  << ")" << std::endl;
			}
		}

	}

	void _loadCursor( )
	{
		_loadTitleCharacter( mCharacterMap[ 'X' ] );

		mCursor = mTitleCharacters[ 'X' ];
	}


	static _TextureInstance * _loadGLCharacterFromMap( CharacterMap & xMap, GLCharacter * xGLChar )
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

		_TextureInstance * pT = new _TextureInstance( );

		pT->texData = xGLChar->allocGLBuffer( );

		pT->tex.initialize( ( pT->w = xGLChar->getWidth( )  )
				  , ( pT->h = xGLChar->getHeight( ) )
				  , ( uint8_t* )pT->texData );

		xMap[ xGLChar->getCharacter( ) ] = pT;

		return pT;
	}

	_TextureInstance * _loadTitleCharacter( GLCharacter * xGLChar )
	{
		return _loadGLCharacterFromMap( mTitleCharacters, xGLChar );
	}

	_TextureInstance * _loadHighlightedCharacter( GLCharacter * xGLChar )
	{
		return _loadGLCharacterFromMap( mHighlightedCharacters, xGLChar );

	}
	_TextureInstance * _loadGLCharacter( GLCharacter * xGLChar )
	{
		return _loadGLCharacterFromMap( mCharacters, xGLChar );
	}

	void _dequeueOne( )
	{
		_TextureInstance * instance = NULL;

		if ( mTextureQueue.try_dequeue( instance ) && instance != NULL )
		{
			if ( !mPBOEnabled )
			{
				instance->initialize( );
			}

			if ( mShown != NULL )
			{
				delete mShown;
			}

			mShown = instance;
		}
	}


	_SubWindow * _createSubWindow( uint32_t const xWidth
				     , uint32_t const xHeight
 				     , uint32_t const xX
				     , uint32_t const xY )
	{
		_SubWindow * subWindow = new _SubWindow( mTitleCharacters['X'] );

		subWindow->texData    		   = ( GLubyte* )malloc( xWidth * xHeight * 4  );
		subWindow->w 	      		   = xWidth;
		subWindow->h 	      		   = xHeight;
		subWindow->originX = subWindow->x  = xX;
		subWindow->originY = subWindow->y  = xY;
		subWindow->isSelected 		   = false;

		_RGBA * windowPixels = reinterpret_cast<_RGBA*>( subWindow->texData );

		for ( uint32_t ii = 0; ii < xWidth * xHeight; ++ii )
		{
			if ( ( ii % xWidth < sBorderWidth ) 		   	   ||
		             ( ii > ( xWidth * xHeight - xWidth * sBorderWidth ) ) ||
			     ( ii < ( xWidth * sBorderWidth ) ) 		   ||
			     ( ii % xWidth > xWidth - sBorderWidth ) )
			{
				windowPixels[ ii ].r = 255;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 127;
			}
			else if ( ( ii < ( xWidth * sCaptionWidth ) ) )
			{
				windowPixels[ ii ].r = 255;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 127;
			}
			else
			{
				windowPixels[ ii ].r = 0;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 127;
			}
		}

		return subWindow;
	}

	void _fillData( GLubyte * xRandomData )
	{
		if ( mShown != NULL )
		{
			size_t const bounds = std::min( (uint32_t)mW * mH, (uint32_t)mShown->w * mShown->h );

			_RGBA * pixelData = reinterpret_cast<_RGBA*>( xRandomData );
			_RGBA * setData = reinterpret_cast<_RGBA*>( mShown->texData );

			for ( uint32_t ii = 0; ii < bounds; ++ii )
			{
				pixelData[ ii ].r = setData[ ii ].r;
				pixelData[ ii ].g = setData[ ii ].g;
				pixelData[ ii ].b = setData[ ii ].b;
				pixelData[ ii ].a = setData[ ii ].a;
			}

			return;
		}

		_RGBA * pixelData = reinterpret_cast<_RGBA*>( xRandomData );
		for ( uint64_t ii = 0; ii < mW * mH; ii += 4 )
		{
			pixelData[ ii ].r = rand( ) % 255;
			pixelData[ ii ].g = rand( ) % 255;
			pixelData[ ii ].b = rand( ) % 255;
			pixelData[ ii ].a = 255;
		}
	}

	void _deselectWindow( )
	{
		if ( mSelectedWindow == NULL )
		{
			return;
		}

		_RGBA * windowPixels = reinterpret_cast<_RGBA*>( mSelectedWindow->texData );

		for ( uint32_t ii = 0; ii < mSelectedWindow->w* mSelectedWindow->h; ++ii )
		{
			if ( ( ii % mSelectedWindow->w < sBorderWidth ) 		                  	      ||
		             ( ii > ( mSelectedWindow->w * mSelectedWindow->h - mSelectedWindow->w * sBorderWidth ) ) ||
			     ( ii < ( mSelectedWindow->w* sBorderWidth ) ) 		  	     		      ||
			     ( ii % mSelectedWindow->w > mSelectedWindow->w - sBorderWidth ) )
			{
				windowPixels[ ii ].r = 255;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 127;
			}
			else if ( ( ii < ( mSelectedWindow->w * sCaptionWidth ) ) )
			{
				windowPixels[ ii ].r = 255;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 127;
			}
			else
			{
				windowPixels[ ii ].r = 0;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 127;
			}
		}

		mSelectedWindow->isSelected = false;
		mSelectedWindow->hasUpdates = true;
		mSelectedWindow->resizeX    = false;
		mSelectedWindow->resizeY    = false;

		_SubWindowIterator const it = mSubWindows[ mSelectedWindow->originY ].find( mSelectedWindow->originX );

		if ( it != mSubWindows[ mSelectedWindow->originY ].end( ) )
		{
			std::swap( mSubWindows[ mSelectedWindow->y ][ mSelectedWindow->x ]
				 , it->second );
			mSubWindows[ mSelectedWindow->originY ].erase( it );
		}

		// Update new origin xy
		mSelectedWindow->originX = mSelectedWindow->x;
		mSelectedWindow->originY = mSelectedWindow->y;


		mSelectedWindow = NULL;
	}

	void _unfocusWindow( )
	{
		if ( mFocusedWindow == NULL )
		{
			return;
		}

		_RGBA * windowPixels = reinterpret_cast<_RGBA*>( mFocusedWindow->texData );

		for ( uint32_t ii = 0; ii < mFocusedWindow->w* mFocusedWindow->h; ++ii )
		{
			if ( ( ii % mFocusedWindow->w < sBorderWidth ) 		                  	      ||
		             ( ii > ( mFocusedWindow->w * mFocusedWindow->h - mFocusedWindow->w * sBorderWidth ) ) ||
			     ( ii < ( mFocusedWindow->w* sBorderWidth ) ) 		  	     		      ||
			     ( ii % mFocusedWindow->w > mFocusedWindow->w - sBorderWidth ) )
			{
				windowPixels[ ii ].r = 255;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 127;
			}
			else if ( ( ii < ( mFocusedWindow->w * sCaptionWidth ) ) )
			{
				windowPixels[ ii ].r = 255;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 127;
			}
			else
			{
				windowPixels[ ii ].r = 0;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 127;
			}
		}

		mFocusedWindow->hasFocus   = false;
		mFocusedWindow->hasUpdates = true;

		mFocusedWindow  = NULL;
	}

	void _focusWindow( _SubWindow * xWindow )
	{
		std::cout << "fuck us window. " << std::endl;
		TRACE;
		if ( xWindow == NULL )
		{
			std::cerr << "Attempt to set null window as selected window." << std::endl;
			exit( -1 );
		}

		if ( xWindow == mFocusedWindow )
		{
			return;
		}

		std::cout << "Bisect1 " << std::endl;

		_RGBA * windowPixels = reinterpret_cast<_RGBA*>( xWindow->texData );

		for ( uint32_t ii = 0; ii < xWindow->w* xWindow->h; ++ii )
		{
			if ( ( ii % xWindow->w < sBorderWidth ) 		   	     ||
		             ( ii > ( xWindow->w* xWindow->h - xWindow->w * sBorderWidth ) ) ||
			     ( ii < ( xWindow->w* sBorderWidth ) ) 		   	     ||
			     ( ii % xWindow->w > xWindow->w - sBorderWidth ) )
			{
				windowPixels[ ii ].r = 0;
				windowPixels[ ii ].g = 255;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 255;
			}
			else if ( ( ii < ( xWindow->w * sCaptionWidth ) ) )
			{
				windowPixels[ ii ].r = 0;
				windowPixels[ ii ].g = 255;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 255;
			}
			else
			{
				windowPixels[ ii ].r = 0;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 255;
			}
		}

		std::cout << "Bisect2 " << std::endl;
		if ( mSelectedWindow != NULL && xWindow != mSelectedWindow )
		{
			_deselectWindow( );
		}

		std::cout << "Bisect3 " << std::endl;

		if ( mFocusedWindow != NULL )
		{
			_unfocusWindow( );
		}

		mFocusedWindow = xWindow;

		xWindow->hasFocus  = true;
		xWindow->hasUpdates = true;
		std::cout << "Bisect4 " << std::endl;

	}

	void _selectWindow( _SubWindow * xWindow )
	{
		if ( xWindow == NULL )
		{
			std::cerr << "Attempt to set null window as selected window." << std::endl;
			exit( -1 );
		}

		if ( xWindow == mSelectedWindow )
		{
			return;
		}

		_RGBA * windowPixels = reinterpret_cast<_RGBA*>( xWindow->texData );

		for ( uint32_t ii = 0; ii < xWindow->w* xWindow->h; ++ii )
		{
			if ( ( ii % xWindow->w < sBorderWidth ) 		   	     ||
		             ( ii > ( xWindow->w* xWindow->h - xWindow->w * sBorderWidth ) ) ||
			     ( ii < ( xWindow->w* sBorderWidth ) ) 		   	     ||
			     ( ii % xWindow->w > xWindow->w - sBorderWidth ) )
			{
				windowPixels[ ii ].r = 255;
				windowPixels[ ii ].g = 255;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 255;
			}
			else if ( ( ii < ( xWindow->w * sCaptionWidth ) ) )
			{
				windowPixels[ ii ].r = 255;
				windowPixels[ ii ].g = 255;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 255;
			}
			else
			{
				windowPixels[ ii ].r = 0;
				windowPixels[ ii ].g = 0;
				windowPixels[ ii ].b = 0;
				windowPixels[ ii ].a = 255;
			}
		}

		if ( mSelectedWindow != NULL )
		{
			_deselectWindow( );
		}

		mSelectedWindow = xWindow;

		xWindow->isSelected = true;
		xWindow->hasUpdates = true;
	}

	void _initializeMouse( )
	{
		mMouse.registerCallback( [ & ] ( MouseEventInfo xEvent )
		{
			mMouseMutex.run( [ & ] ( )
			{
				mCursorPosition = xEvent;

				bool windowCollission = false;
				if ( xEvent.leftState == eClickState_Down )
				{
					_SubWindow * windowToDelete = NULL;

					for ( auto && ymap : mSubWindows )
					{
						for ( auto && xmapWindowPair : ymap.second )
						{
							_SubWindow * window = xmapWindowPair.second;

							if ( window == NULL )
							{
								continue;
							}

							if ( xEvent.x < ( window->x + window->w ) 			       &&
							     xEvent.x > ( window->x + ( window->w - window->closeButton->w ) ) &&
							     xEvent.y > window->y  			       		       &&
							     xEvent.y < ( window->y + window->closeButton->h ) )
							{
								windowToDelete = window ;

								if ( window == mSelectedWindow )
								{
									_deselectWindow( );
								}

								windowCollission = true;
							}
						        else if ( xEvent.x > window->x 	      	   &&
							          xEvent.x < window->x + window->w &&
							     	  xEvent.y > window->y 	      	   &&
							     	  xEvent.y < window->y + sFuzzPixels ) // Click top panel
							{
								_selectWindow( window );
								windowCollission = true;
							}
							else if ( xEvent.x > window->x 		   		     &&
								  xEvent.x < window->x + window->w 		     &&
								  xEvent.y > ( window->y + window->h - sFuzzPixels ) &&
								  xEvent.y < ( window->y + window->h ) ) // Click Bottom line
							{
								window->resizeY = true;
								windowCollission = true;
							}
							/*
							else if ( ) // click left or right border
							{
								window->resizeY = true;
								_selectWindow( window );
							}
							else if (  ) // click corner
							{
								window->resizeX = true;
								window->resizeY = true;
								_selectWindow( window );
							}
							*/

							if ( windowCollission )
							{
								break;
							}
						}

						if ( windowCollission )
						{
							break;
						}
					}

					// delete window
					if ( windowToDelete != NULL )
					{
						//_printMap( );

						_SubWindowIterator const it = mSubWindows[ windowToDelete->originY ].find( windowToDelete->originX );

						if ( it == mSubWindows[ windowToDelete->originY ].end( ) )
						{
							std::cerr << "Window not found in map!" << std::endl;
							exit( -1 );
						}

						delete it->second;

						it->second = NULL;

						mSubWindows[ windowToDelete->originY ].erase( it );
					}

				} // if  click down
				else if ( xEvent.leftState == eClickState_Continue )
				{
					if ( mSelectedWindow != NULL && !mSelectedWindow->resizeY && !mSelectedWindow->resizeX )
					{
						mSelectedWindow->x = xEvent.x;
						mSelectedWindow->y = xEvent.y;
					}

					if ( mSelectedWindow != NULL  &&
					     mSelectedWindow->resizeY &&
					     xEvent.y > mSelectedWindow->y )
					{
						uint32_t const newHeight = xEvent.y - mSelectedWindow->y;
						mSelectedWindow->h = newHeight < mSelectedWindow->h + sFuzzPixels ? sFuzzPixels : newHeight;
						mSelectedWindow->h = mSelectedWindow->y + newHeight > mH ? mH - mSelectedWindow->y : newHeight;
					}

					if ( mSelectedWindow != NULL && mSelectedWindow->resizeX )
					{
						mSelectedWindow->w = mSelectedWindow->x - xEvent.x;
					}


					mCursor = mTitleCharacters[ 'O' ];

				}
				else if ( xEvent.leftState == eClickState_Up )
				{
					if ( mSelectedWindow != NULL )
					{
						std::cout << "focus" << std::endl;
						_focusWindow( mSelectedWindow );
						std::cout << "Deselect" << std::endl;
						_deselectWindow( );
					}

					mCursor = mTitleCharacters[ 'X' ];
				}
			} ); // mutex run
		}); // register cb

		mMouse.listen( );

	}

	void _updateTexture( Texture2D & xTexture, GLubyte * xData, std::function<uint8_t(uint8_t*)> xUpdateFunction = [](uint8_t*){ return 0; })
	{
		xTexture( [ & ]( )
		{
			if ( !mPBOEnabled )
			{
				xUpdateFunction( xData );
				glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mW, mH, GL_RGBA, GL_UNSIGNED_BYTE, xData );
			}
			else
			{
				static int index = 0;
				int nextIndex = 0;

				index = (index + 1) % 2;
			        nextIndex = (index + 1) % 2;

				glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, mPBO[ index ] );

				GLsizei w = mW, h = mH;

				if ( mShown != NULL )
				{
					w = mShown->w;
					h = mShown->h;
				}

				glTexSubImage2D( GL_TEXTURE_2D
					       , 0
					       , 0
					       , 0
					       , w
					       , h
					       , GL_RGBA
					       , GL_UNSIGNED_BYTE
					       , 0 );

				glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, mPBO[ nextIndex ] );

				glBufferDataARB( GL_PIXEL_UNPACK_BUFFER_ARB, mSize, 0, GL_STREAM_DRAW_ARB );
				GLubyte* ptr = (GLubyte*)glMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );

				if ( ptr )
				{
					xUpdateFunction( ptr );
					glUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );
				}

				glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
			}
		});

	}

	void _initializeKeyboard( )
	{
		mKB.registerCallback( [&]( KeyboardEvent xEvent )
		{
			std::cout << "Event get" << std::endl;
			if ( xEvent.getScanCode( ) == SDL_SCANCODE_Q )
			{
				mQuit.set( );
			}

			bool const shiftPressed = xEvent[ SDL_SCANCODE_LSHIFT  ] || xEvent[ SDL_SCANCODE_RSHIFT  ];
			bool const altPressed   = xEvent[ SDL_SCANCODE_LALT    ] || xEvent[ SDL_SCANCODE_RALT    ];
			bool const ctrlPressed  = xEvent[ SDL_SCANCODE_LCTRL   ] || xEvent[ SDL_SCANCODE_RCTRL    ];
			bool const enterPressed = xEvent[ SDL_SCANCODE_RETURN  ] || xEvent[ SDL_SCANCODE_RETURN2 ];

			if ( ctrlPressed && altPressed && enterPressed )
			{

			        uint32_t const x = ( rand( ) % mW  ) - 512;
		                uint32_t const y = ( rand( ) % mH  ) - 512;
				uint32_t const w = ( rand( ) % 100 ) + 412;
				uint32_t const h = ( rand( ) % 100 ) + 412;

				std::cout << "create sub window. " << std::endl;
				mSubWindows[ y ][ x ] = _createSubWindow( w, h, x, y );

				return;
			}

			char const c = sSDLKeyboardScancodeMap.resolveScanCode( xEvent.getScanCode( ), shiftPressed );

			if ( c != 0x00 && xEvent.getKeyDown( ) || xEvent.getKeyContinue( ) )
			{
				mStdIn.enqueue( c );
			}
		} );

		mKB.listen( );
	}

	void _drawWindow( _SubWindow * xWindow )
	{
		if ( xWindow == NULL )
		{
			return;
		}

		if ( !xWindow->initialized )
		{
			xWindow->initialize( );
		}

		_drawSprite( xWindow->tex
			   , glm::vec2( xWindow->x, xWindow->y )
			   , glm::vec2( xWindow->w, xWindow->h )
			   , 0.0f
			   , glm::vec3( 1.0f, 1.0f, 1.0f ) );

		_drawSprite ( xWindow->closeButton->tex
	  		   , glm::vec2( xWindow->x + (xWindow->w - xWindow->closeButton->w)
				      , xWindow->y )
			   , glm::vec2( xWindow->closeButton->w
				      , xWindow->closeButton->h )
			   , 0.0f
			   , glm::vec3( 1.0f, 1.0f, 1.0f ) );


		if ( xWindow->hasUpdates )
		{
			xWindow->tex([&]()
			{
				glTexParameteri( GL_TEXTURE_2D
						       , GL_GENERATE_MIPMAP
						       , GL_FALSE );

				glTexSubImage2D( GL_TEXTURE_2D
					        , 0
						, 0
						, 0
						, xWindow->w
						, xWindow->h
						, GL_RGBA
						, GL_UNSIGNED_BYTE
						, xWindow->texData );
			});

			xWindow->hasUpdates = false;
		}

	}

	protected:
		virtual void render( )
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );


			_drawSprite( mBackgroundTexture
				   , glm::vec2( 0, 0 )
				   , glm::vec2( mW, mH )
				   , 0.0f
				   , glm::vec3( 1.0f, 1.0f, 1.0f ) );

			glEnable( GL_BLEND );

			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			//glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE );

			glBlendEquation( GL_FUNC_ADD );

			_dequeueOne( );

			if ( mShown != NULL && !mPBOEnabled )
			{
				_drawSprite( mShown->tex
					   , glm::vec2( 0, 0 )
					   , glm::vec2( mShown->w, mShown->h )
					   , 0.0f
					   , glm::vec3( 1.0f, 1.0f, 1.0f ) );
			}


			MouseEventInfo currentCursorPosition;

			mMouseMutex.run( [ & ]( )
			{
				currentCursorPosition = mCursorPosition;

				for ( auto && ymap : mSubWindows )
				{
					for ( auto && instance : ymap.second )
					{
						_drawWindow( instance.second );
					}
				}

				// Draw the cursor
				_drawSprite( mCursor->tex
					   , glm::vec2( currentCursorPosition.x, currentCursorPosition.y )
					   , glm::vec2( mCursor->w, mCursor->h )
					   , 0.0f
			        	   , glm::vec3( 1.0f, 1.0f, 1.0f ) );

			});

			static GLuint const sOverscanX = 64;
			static GLuint const sOverscanY = 64;

			GLuint offsetx = sOverscanX;
			GLuint offsety = sOverscanY;

			char c;

			if ( mStdIn.try_dequeue( c ) )
			{
				mLineBuffer.push_back( c );
			}

			for ( auto && c : mLineBuffer )
			{
				_TextureInstance * pT = _loadGLCharacter( mCharacterMap[ c ] );

				if ( c == '\n' )
				{
					offsety += pT->h;
					offsetx = sOverscanX;
					continue;
				}

				if ( c == '\t' )
				{
					offsetx += pT->w * 8;
					continue;
				}

				_drawSprite( pT->tex
					   , glm::vec2( offsetx, offsety )
					   , glm::vec2( pT->w, pT->h )
					   , 0.0f
					   , glm::vec3( 1.0f, 1.0f, 1.0f ) );

				if ( ( offsetx += pT->w ) >= mW )
				{
					offsetx = sOverscanX;
					offsety += pT->h;
				}
			}

			// Animate the background if we had no incoming frames
			_updateTexture( mBackgroundTexture, mRandomData, [&](uint8_t* xData) { _fillData( xData ); return 0; } );
		}

	public:
		SpriteShader( bool const xEnablePBO = false )
			: SDLTestWindow( )
				, mSpriteShader( ShaderPathPair( "resources/sprite.vs.txt", "resources/sprite.fs.txt" ) )
			, mBackgroundTexture( )
			, mRandomData( NULL )
			, mVBO( 0 )
			, mQuadVAO( 0 )
			, mQuit( )
			, mPBOEnabled( xEnablePBO )
			, mPBO( )
			, mSize( 0 )
			, mCharacterMap( "resources/terminus.ttf"
				       , 32
				       , "" )
			, mHighlightedCharacterMap( "resources/terminus.ttf"
				       , 32
				       , ""
				       , _FontPixel32( 255, 255, 0 )
				       , _FontPixel32( 255, 255, 0 ) )
			, mTitleCharacterMap( "resources/terminus.ttf"
				       , 32
				       , ""
				       , _FontPixel32( 0, 0, 0 )
				       , _FontPixel32( 0, 0, 0 ) )
			, mHighlightedCharacters( )
			, mCharacters( )
			, mTitleCharacters( )
			, mMouse( )
			, mKB( )
			, mMouseMutex( )
			, mCursor( NULL )
			, mShown( NULL )
			, mSubWindows( )
			, mSelectedWindow( NULL )
			, mFocusedWindow( NULL )
		{
			;
		}

		~SpriteShader( )
		{
			for ( auto && ymap : mSubWindows )
			{
				for ( auto && window : ymap.second )
				{
					delete window.second;
				}
			}

			if ( mShown != NULL )
			{
				delete mShown;
			}

			mMouse.stopListening( );

			mKB.stopListening( );

			if ( mRandomData != NULL )
			{
				delete[] mRandomData;
			}

			for ( auto && ii : mCharacters )
			{
				delete ii.second;
			}

			if ( mPBOEnabled )
			{
				glDeleteBuffersARB( 2, mPBO );
			}

			if ( mVBO != 0 )
			{
				glDeleteBuffers( 1, &mVBO );
			}

			if ( mQuadVAO != 0 )
			{
				glDeleteVertexArrays( 1, &mQuadVAO );
			}
		}


		virtual void enqueue( GLubyte * xData, uint64_t const xWidth, uint64_t const xHeight )
		{
			_TextureInstance * instance = new _TextureInstance( );

			instance->texData = xData;
			instance->w = xWidth;
			instance->h = xHeight;

			mTextureQueue.enqueue( instance );
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			_initializeMouse( );

			_initializeKeyboard( );

			// Load ASCII
		        for ( char c = '!'; c < '~'; c++ )
			{
				_loadGLCharacter( mCharacterMap[ c ] );
				_loadHighlightedCharacter( mHighlightedCharacterMap[ c ] );
				_loadTitleCharacter( mTitleCharacterMap[ c ] );
			}

			_loadCursor( );

			glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

			if ( mRandomData != NULL )
			{
				std::cerr << "RGBA data was already initialized. Calling initialze( .. ) twice is undefined." << std::endl;
				exit( -1 );
			}

			mSize = mW * mH * 4; // Assuming 32 bit color. TODO: Fix or not care

			mRandomData = new GLubyte[ mSize ];

			_fillData( mRandomData );

			mBackgroundTexture.initialize( mW, mH, (uint8_t*)mRandomData );

			if ( mPBOEnabled )
			{
				mBackgroundTexture( [ & ] ( )
				{
					glGenBuffersARB( 2, mPBO );

					glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, mPBO[ 0 ] );

					glBufferDataARB( GL_PIXEL_PACK_BUFFER_ARB, mSize, 0, GL_STREAM_READ_ARB );

					glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, mPBO[ 1 ] );

					glBufferDataARB( GL_PIXEL_PACK_BUFFER_ARB, mSize, 0, GL_STREAM_READ_ARB );

					glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, 0 );

					_checkForGLError( "Create PBOs" );
				} );
			}

			mSpriteShader.initialize( );

			glm::mat4 projection = glm::ortho(  0.0f
							 , static_cast<GLfloat>( mW )
							 , static_cast<GLfloat>( mH )
							 ,  0.0f
							 , -1.0f
							 ,  1.0f );

			mSpriteShader( [ & ]( )
			{
				mSpriteShader.getUniforms( ).setUniform1i( "image", 0 );
				mSpriteShader.getUniforms( ).setUniformMatrix4fv( "projection", projection );
			});


			// TODO: Support more than one sprite!
			static GLfloat const vertices[] = {
						   	    // Pos      // Tex
	  						    0.0f, 1.0f, 0.0f, 1.0f,
	  						    1.0f, 0.0f, 1.0f, 0.0f,
						            0.0f, 0.0f, 0.0f, 0.0f,

						            0.0f, 1.0f, 0.0f, 1.0f,
						            1.0f, 1.0f, 1.0f, 1.0f,
						            1.0f, 0.0f, 1.0f, 0.0f
					    		  };

			glGenVertexArrays( 1, &mQuadVAO );

			glGenBuffers( 1, &mVBO );

			glBindBuffer( GL_ARRAY_BUFFER, mVBO );

			glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

			glBindVertexArray( mQuadVAO );

			glEnableVertexAttribArray( 0 );

			glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), (GLvoid*)0 );

			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindVertexArray( 0 );
		}

		virtual void run( )
		{
			while ( !mQuit.isSet( ) )
			{
				ShowCursor( FALSE );
				SetCursor( NULL );

				render( );
				SDL_GL_SwapWindow( mMainWindow );
				SDL_PumpEvents( );
			}

			_hide( );
		}

		void stop ( ) { mQuit.set( ); }
};

#define DEPTH_TEXTURE_SIZE 512
#define _M_PI 3.14159265358979323846

#define INSTANCE_COUNT 100

#define FRUSTUM_DEPTH       800.0f
#define DEPTH_TEXTURE_SIZE  512


class RedbookCh04Shader : public SDLTestWindow
{
	SDLShader mShadowShader, mSceneShader;

	float mAspect;

	struct _RenderLightUniforms
	{
		_RenderLightUniforms( )
			: modelViewProjectionMatrix( -1 )
		{
			;
		}

		GLint modelViewProjectionMatrix;

	} mRenderLightUniforms;

	struct _RenderSceneUniforms
	{

		_RenderSceneUniforms( )
			: viewMatrix( -1 )
			, projectionMatrix( -1 )
			, shadowMatrix( -1 )
			, lightPosition( -1 )
			, materialAmbient( -1 )
			, materialDiffuse( -1 )
			, materialSpecular( -1 )
			, materialSpecularPower( -1 )
		{
			;
		}

		GLint modelMatrix
		    , viewMatrix
		    , projectionMatrix
		    , shadowMatrix
		    , lightPosition
		    , materialAmbient
		    , materialDiffuse
		    , materialSpecular
		    , materialSpecularPower;

	} mRenderSceneUniforms;

	GLuint mDepthFBO
	     , mDepthTexture
	     , mGroundVBO
	     , mGroundVAO;

	VBMObject mObject;

	void _drawScene( bool const xDepthOnly )
	{
		if ( !xDepthOnly )
    		{
        		glUniform3fv( mRenderSceneUniforms.materialAmbient,  1, glm::value_ptr(glm::vec3(0.1f, 0.0f, 0.2f)));
        		glUniform3fv( mRenderSceneUniforms.materialDiffuse,  1, glm::value_ptr(glm::vec3(0.3f, 0.2f, 0.8f)));
        		glUniform3fv( mRenderSceneUniforms.materialSpecular, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

        		glUniform1f( mRenderSceneUniforms.materialSpecularPower, 25.0f);
 		}

    		mObject.render( 0, 0 );

    		if ( !xDepthOnly )
    		{
        		glUniform3fv(mRenderSceneUniforms.materialAmbient,  1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));
        		glUniform3fv(mRenderSceneUniforms.materialDiffuse,  1, glm::value_ptr(glm::vec3(0.1f, 0.5f, 0.1f)));
        		glUniform3fv(mRenderSceneUniforms.materialSpecular, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));

        		glUniform1f( mRenderSceneUniforms.materialSpecularPower, 3.0f );
		}

    		glBindVertexArray( mGroundVAO );

    		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    		glBindVertexArray(0);
	}

	protected:
		virtual void render( )
		{
			float t = float(SDL_GetTicks() & 0xFFFF) / float(0xFFFF);

			static float q = 0.0f;
			static const glm::vec3 X( 1.0f, 0.0f, 0.0f );
			static const glm::vec3 Y( 0.0f, 1.0f, 0.0f );
			static const glm::vec3 Z( 0.0f, 0.0f, 1.0f );

			glm::vec3 light_position = glm::vec3( sinf( t * 6.0f * 3.141592f ) * 300.0f
							    , 200.0f
							    , cosf(t * 4.0f * 3.141592f) * 100.0f + 250.0f );

			glEnable( GL_CULL_FACE );
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_LEQUAL );

			glm::mat4 scene_model_matrix 	  = glm::rotate(glm::mat4(1.0f),t * float(_M_PI*4), Y);
			glm::mat4 scene_view_matrix 	  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -300.0f));
			glm::mat4 scene_projection_matrix = glm::frustum(-1.0f, 1.0f, -mAspect, mAspect, 1.0f, FRUSTUM_DEPTH);

			const glm::mat4 scale_bias_matrix = glm::mat4(glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
                        			                      glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
                                                  		      glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
   			                                              glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    			// Matrices used when rendering from the light's position
    			glm::mat4 light_view_matrix = glm::lookAt(light_position, glm::vec3(0.0f), Y);
    			glm::mat4 light_projection_matrix(glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, FRUSTUM_DEPTH));

    			// Now we render from the light's position into the depth buffer.
    			// Select the appropriate program
			mShadowShader([&](){

				glUniformMatrix4fv( mRenderLightUniforms.modelViewProjectionMatrix
						  , 1
						  , GL_FALSE
						  , glm::value_ptr(light_projection_matrix * light_view_matrix * scene_model_matrix));

    				// Bind the 'depth only' FBO and set the viewport to the size of the depth texture
    				glBindFramebuffer(GL_FRAMEBUFFER, mDepthFBO);
    				glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);

				// Clear
	    			glClearDepth(1.0f);
	    			glClear(GL_DEPTH_BUFFER_BIT);

	    			// Enable polygon offset to resolve depth-fighting isuses
	    			glEnable(GL_POLYGON_OFFSET_FILL);
	    			glPolygonOffset(2.0f, 4.0f);

				// Draw from the light's point of view
	   			_drawScene(true);
	    			glDisable(GL_POLYGON_OFFSET_FILL);

	   			// Restore the default framebuffer and field of view
	   			glBindFramebuffer(GL_FRAMEBUFFER, 0);

	    			glViewport(0, 0, mW, mH);
			});

			mSceneShader([&]() {
	    			// Now render from the viewer's position    			glUseProgram(render_scene_prog);
	    			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	   			 // Setup all the matrices
	    			glUniformMatrix4fv(mRenderSceneUniforms.modelMatrix, 	  1, GL_FALSE, glm::value_ptr(scene_model_matrix));
	   			glUniformMatrix4fv(mRenderSceneUniforms.viewMatrix, 	  1, GL_FALSE, glm::value_ptr(scene_view_matrix));
	    			glUniformMatrix4fv(mRenderSceneUniforms.projectionMatrix, 1, GL_FALSE, glm::value_ptr(scene_projection_matrix));
	    			glUniformMatrix4fv(mRenderSceneUniforms.shadowMatrix, 	  1, GL_FALSE, glm::value_ptr(scale_bias_matrix * light_projection_matrix * light_view_matrix));

	    			glUniform3fv(mRenderSceneUniforms.lightPosition, 1, glm::value_ptr(light_position));

	   			// Bind the depth texture
    				glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	    			glGenerateMipmap(GL_TEXTURE_2D);

	    			// Draw
	    			_drawScene(false);
			});
		}

	public:
		RedbookCh04Shader( )
			: SDLTestWindow( )
			, mShadowShader( ShaderPathPair("resources/shadowmap_shadow.vs.glsl", "resources/shadowmap_shadow.fs.glsl") )
			, mSceneShader( ShaderPathPair("resources/shadowmap_scene.vs.glsl", "resources/shadowmap_scene.fs.glsl") )
			, mAspect( 0 )
			, mRenderLightUniforms( )
			, mRenderSceneUniforms( )
			, mDepthFBO( 0 )
			, mDepthTexture( 0 )
			, mGroundVBO( 0 )
			, mGroundVAO( 0 )
			, mObject( VBMObjectFileDescriptor( "resources/armadillow_low.vbm", 0, 1, 2 ) )
		{
			;
		}

		~RedbookCh04Shader( )
		{
			if ( mGroundVBO != 0 )
			{
				glDeleteBuffers( 1, &mGroundVBO );
			}

			if ( mGroundVAO != 0 )
			{
				glDeleteVertexArrays( 1, &mGroundVAO );
			}
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );
			mAspect = mH / mW;

			mShadowShader.initialize( );
			mSceneShader.initialize( );

			mRenderSceneUniforms.modelMatrix 	   = mSceneShader.getUniforms( )[ "model_matrix" ];
			mRenderSceneUniforms.viewMatrix 	   = mSceneShader.getUniforms( )[ "view_matrix" ];
			mRenderSceneUniforms.projectionMatrix 	   = mSceneShader.getUniforms( )[ "projection_matrix" ];
			mRenderSceneUniforms.shadowMatrix 	   = mSceneShader.getUniforms( )[ "shadow_matrix" ];
			mRenderSceneUniforms.lightPosition 	   = mSceneShader.getUniforms( )[ "light_position" ];
			mRenderSceneUniforms.materialAmbient 	   = mSceneShader.getUniforms( )[ "material_ambient" ];
			mRenderSceneUniforms.materialDiffuse 	   = mSceneShader.getUniforms( )[ "material_diffuse" ];
			mRenderSceneUniforms.materialSpecular 	   = mSceneShader.getUniforms( )[ "material_specular" ];
			mRenderSceneUniforms.materialSpecularPower = mSceneShader.getUniforms( )[ "material_specular_power" ];

			mSceneShader([&](){

				glUniform1i( mSceneShader.getUniforms()[ "depth_texture" ], 0 );

				glGenTextures( 1, &mDepthTexture );
				glBindTexture( GL_TEXTURE_2D, mDepthTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glBindTexture( GL_TEXTURE_2D, 0 );

				glGenFramebuffers( 1, &mDepthFBO );
				glBindFramebuffer( GL_FRAMEBUFFER, mDepthFBO );
				glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,mDepthTexture, 0 );
				glDrawBuffer( GL_NONE );

				GLuint const status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

				if ( status != GL_FRAMEBUFFER_COMPLETE )
				{
					std::cerr << "Framebuffer error: " << std::hex << status << std::endl;
					exit( -1 );
				}

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				static const float ground_vertices[] =
				{
				        -500.0f, -50.0f, -500.0f, 1.0f,
				        -500.0f, -50.0f,  500.0f, 1.0f,
				         500.0f, -50.0f,  500.0f, 1.0f,
				         500.0f, -50.0f, -500.0f, 1.0f,
				};

	 	 	 	static const float ground_normals[] =
				{
				        0.0f, 1.0f, 0.0f,
				        0.0f, 1.0f, 0.0f,
				        0.0f, 1.0f, 0.0f,
				        0.0f, 1.0f, 0.0f
				};

    				glGenVertexArrays(1, &mGroundVAO);
    				glGenBuffers(1, &mGroundVBO);;
				glBindVertexArray(mGroundVAO);
 				glBindBuffer(GL_ARRAY_BUFFER, mGroundVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices) + sizeof(ground_normals), NULL, GL_STATIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ground_vertices), ground_vertices);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(ground_vertices), sizeof(ground_normals), ground_normals);

				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(ground_vertices));
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);

			});

			mObject.initialize( );
		}

		virtual void run( )
		{
			while ( 1 )
			{
				render( );
				SDL_GL_SwapWindow( mMainWindow );
			}

			_hide( );
		}

};

class RedbookCh03Shader : public SDLTestShaderWindow
{
	float mAspect;

	GLuint mVBO, mEBO, mVAO;

	GLint mRenderModelMatrixLoc
	    , mRenderProjectionMatrixLoc;

	protected:
		virtual void render( )
		{
			SDLTestShaderWindow::render( );

			glm::mat4 model_matrix;

			// Setup
			glEnable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Activate simple shading program
			mShader([&](){

				// Set up the model and projection matrix
				glm::mat4 projection_matrix(glm::frustum(-1.0f, 1.0f, -mAspect, mAspect, 1.0f, 500.0f));
				glUniformMatrix4fv(mRenderProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

				// Set up for a glDrawElements call
				glBindVertexArray(mVAO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

				// Draw Arrays...
				model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(-3.0f, 0.0f, -5.0f));
				glUniformMatrix4fv(mRenderModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
				glDrawArrays(GL_TRIANGLES, 0, 3);

				// DrawElements
				model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(-1.0f, 0.0f, -5.0f));
				glUniformMatrix4fv(mRenderModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

				// DrawElementsBaseVertex
				model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(1.0f, 0.0f, -5.0f));
				glUniformMatrix4fv(mRenderModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
				glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

				// DrawArraysInstanced
				model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(3.0f, 0.0f, -5.0f));
				glUniformMatrix4fv(mRenderModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
				glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);
			});
		}
	public:
		RedbookCh03Shader( )
			: SDLTestShaderWindow( ShaderPathPair("resources/primitive_restart.vs.glsl", "resources/primitive_restart.fs.glsl") )
			, mAspect( 0.0f )
			, mVBO( 0 )
			, mVAO( 0 )
			, mEBO( 0 )
			, mRenderModelMatrixLoc( -1 )
			, mRenderProjectionMatrixLoc( -1 )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestShaderWindow::initialize( xAttributes );


			mShader([&](){

				mRenderModelMatrixLoc = glGetUniformLocation( mShader.getProgramID( ), "model_matrix" );

				mRenderProjectionMatrixLoc = glGetUniformLocation( mShader.getProgramID( ), "projection_matrix" );

				// A single triangle
				static const GLfloat vertex_positions[] = {
									      -1.0f, -1.0f,  0.0f, 1.0f,
									      1.0f, -1.0f,  0.0f, 1.0f,
									      -1.0f,  1.0f,  0.0f, 1.0f,
									      -1.0f, -1.0f,  0.0f, 1.0f,
				  };

				// Color for each vertex
				static const GLfloat vertex_colors[] = {
									      1.0f, 1.0f, 1.0f, 1.0f,
									      1.0f, 1.0f, 0.0f, 1.0f,
									      1.0f, 0.0f, 1.0f, 1.0f,
									      0.0f, 1.0f, 1.0f, 1.0f
				  };

				// Indices for the triangle strips
				static const GLushort vertex_indices[] = { 0, 1, 2 };

				// Set up the element array buffer
				glGenBuffers(1, &mEBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

				// Set up the vertex attributes
				glGenVertexArrays(1, &mVAO);
				glBindVertexArray(mVAO);

				glGenBuffers(1, &mVBO);
				glBindBuffer(GL_ARRAY_BUFFER, mVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);

				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
				glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);

				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

				// TODO: Should only do this on resize
				glViewport(0, 0, mW, mH );
				mAspect = float(mH)/float(mW);
			});
		}

		virtual void run( )
		{
			do
			{
				render( );
				_checkForGLError( );
				SDL_PumpEvents( );
				SDL_GL_SwapWindow( mMainWindow );

			} while ( !mQuit.isSet( ) );
			_hide( );
		}

};

} // namespace sisu

#endif // SDL_SHADER_TEST_DBB12D8670054D2E97E6A436C1F0EC51_HPP_
