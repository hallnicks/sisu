#ifndef VBM_D29B90D706BA486AB6119E79471EA462_HPP_
#define VBM_D29B90D706BA486AB6119E79471EA462_HPP_

#include <fstream>

namespace sisu {

typedef struct _VBM_HEADER
{
	uint32_t magic
	       , size;

	char name[64];

	uint32_t numberOfAttributes
	       , numberOfFrames
 	       , numberOfVertices
	       , numberOfIndices
	       , indexType;

} VBM_HEADER;

typedef struct _VBM_ATTRIBUTE_HEADER
{
	char name[64];

	uint32_t type, components, flags;

} VBM_ATTRIBUTE_HEADER;

typedef struct _VBM_FRAME_HEADER
{
	uint32_t first, count, flags;

} VBM_FRAME_HEADER;

typedef struct _VBM_VEC4F { float x, y, z, w; } VBM_VEC4F;

typedef struct _VBMObjectFileDescriptor
{
	_VBMObjectFileDescriptor( )
		: filename( NULL )
		, vertexIndex( -1 )
		, normalIndex( -1 )
		, texCoord0Index( -1 )
	{
		;
	}

	_VBMObjectFileDescriptor( const char * xFilename
				, int32_t const xVertexIndex
				, int32_t const xNormalIndex
				, int32_t const xTexCoord0Index )
		: filename( xFilename )
		, vertexIndex( xVertexIndex )
		, normalIndex( xNormalIndex )
		, texCoord0Index( xTexCoord0Index )
	{
		;
	}

	const char * filename;

	int32_t const vertexIndex
		    , normalIndex
		    , texCoord0Index;

} VBMObjectFileDescriptor;

class VBMObject
{
	std::vector<char> mBuffer;

	void _free( )
	{
		if ( mIndexBuffer != 0 )
		{

			glDeleteBuffers( 1, &mIndexBuffer );

			mIndexBuffer = 0;

		}


		if ( mAttributeBuffer != 0 )
		{
			glDeleteBuffers( 1, &mAttributeBuffer );

			mAttributeBuffer = 0;

		}

		if ( mVAO != 0 )
		{
			glDeleteVertexArrays( 1, &mVAO );

			mVAO = 0;
		}

		if ( mAttributes != NULL )
		{
			delete[] mAttributes;

			mAttributes = NULL;

		}

		if ( mFrames != NULL )
		{
			delete[] mFrames;

			mFrames = NULL;
		}
	}

	protected:
		GLuint mVAO
		     , mAttributeBuffer
		     , mIndexBuffer;

		VBM_ATTRIBUTE_HEADER * mAttributes;
		VBM_FRAME_HEADER     * mFrames;
		VBM_HEADER 	       mHeader;

		VBMObjectFileDescriptor mFileDescriptor;

	public:
		VBMObject( )
			: mBuffer( )
			, mVAO( 0 )
			, mAttributeBuffer( 0 )
			, mIndexBuffer( 0 )
			, mAttributes( NULL )
			, mFrames( NULL )
			, mHeader( )
			, mFileDescriptor( )
		{
			;
		}

		VBMObject( VBMObjectFileDescriptor const & xFileDescriptor )
			: mBuffer( )
			, mVAO( 0 )
			, mAttributeBuffer( 0 )
			, mIndexBuffer( 0 )
			, mAttributes( NULL )
			, mFrames( NULL )
			, mHeader( )
			, mFileDescriptor( xFileDescriptor )
		{
			;
		}

		virtual ~VBMObject( )
		{
			_free( );
		}

		void initialize( )
		{
			if ( mFileDescriptor.filename != NULL )
			{
				uint8_t * rawData = NULL;

				uint32_t totalDatasize = 0;

				std::ifstream ifs( mFileDescriptor.filename, std::ios::binary | std::ios::ate );

				std::streamsize const size = ifs.tellg( );

				ifs.seekg( 0, std::ios::beg );

				mBuffer = std::vector<char>( size );

				ifs.read( mBuffer.data( ), size );

				VBM_HEADER * header = (VBM_HEADER *) mBuffer.data( );

				rawData = (uint8_t*)mBuffer.data( ) + sizeof( VBM_HEADER )
					  		 	    + header->numberOfAttributes * sizeof( VBM_ATTRIBUTE_HEADER )
							 	    + header->numberOfFrames     * sizeof( VBM_FRAME_HEADER );

				VBM_ATTRIBUTE_HEADER * attributeHeader = ( VBM_ATTRIBUTE_HEADER* ) ( mBuffer.data( ) + sizeof( VBM_HEADER ) );

				VBM_FRAME_HEADER *     frameHeader     = ( VBM_FRAME_HEADER *    ) ( mBuffer.data( ) + sizeof( VBM_HEADER )  )
								       + ( header->numberOfAttributes * sizeof( VBM_ATTRIBUTE_HEADER ) );
				unsigned int totalDataSize = 0;

				// TODO: eliminate memcpy and just use buffer.data( ) ?
				memcpy( &mHeader, header, sizeof( VBM_HEADER ) );

				mAttributes = new VBM_ATTRIBUTE_HEADER[ header->numberOfAttributes ];

				memcpy( &mAttributes, attributeHeader, header->numberOfAttributes * sizeof(VBM_ATTRIBUTE_HEADER ) );

				mFrames = new VBM_FRAME_HEADER[ header->numberOfFrames ];

				memcpy( &mFrames, frameHeader, header->numberOfFrames * sizeof( VBM_FRAME_HEADER ) );

				glGenVertexArrays( 1, &mVAO );

				(*this)([&]()
				{
					glGenBuffers( 1, &mAttributeBuffer );

					glBindBuffer( GL_ARRAY_BUFFER, mAttributeBuffer );

					for ( uint32_t ii = 0; ii < header->numberOfAttributes; ++ii )
					{
						int32_t attributeIndex = ii;

						if ( attributeIndex == 0 )
						{
							attributeIndex = mFileDescriptor.vertexIndex;
						}
						else if ( attributeIndex == 1 )
						{
							attributeIndex = mFileDescriptor.normalIndex;
						}
						else if ( attributeIndex == 2 )
						{
							attributeIndex = mFileDescriptor.texCoord0Index;
						}

						glVertexAttribPointer( attributeIndex
								     , mAttributes[ ii ].components
								     , mAttributes[ ii ].type
								     , GL_FALSE
								     , 0
								     , reinterpret_cast<GLvoid *>( totalDataSize )); // sketchy cast, originally was (GLVoid*) gave compiler err

						glEnableVertexAttribArray( attributeIndex );

						totalDataSize += mAttributes[ ii ].components
							       * sizeof( GLfloat )
							       * header->numberOfVertices;
					}

					glBufferData( GL_ARRAY_BUFFER, totalDataSize, rawData, GL_STATIC_DRAW );

					if ( header->numberOfIndices > 0 )
					{
						glGenBuffers( 1, &mIndexBuffer );

						glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer );

						uint32_t elementSize;

						switch( header->indexType )
						{
							case GL_UNSIGNED_SHORT:
								elementSize = sizeof( GLushort );
								break;

							default:
								elementSize = sizeof( GLuint );
								break;
						}

						glBufferData( GL_ELEMENT_ARRAY_BUFFER
							    , header->numberOfIndices * elementSize
						 	    , mBuffer.data( ) + totalDataSize
							    , GL_STATIC_DRAW );
					}

					glBindVertexArray( 0 );
				});
			}
		}

		uint32_t getVertexCount( uint32_t const xFrame ) const
		{
			if ( xFrame >= mHeader.numberOfFrames )
			{
				std::cerr << " getVertexCount ( " << xFrame << " ) is out of bounds." << std::endl;
				exit( -1 );
			}

			return mFrames[ xFrame ].count;
		}

		uint32_t getAttributeCount( ) const
		{
			return mHeader.numberOfAttributes;
		}

		const char * getAttributeName( uint32_t const xIndex ) const
		{
			if ( xIndex >= mHeader.numberOfAttributes )
			{
				std::cerr << " getAttributeName( " << xIndex << " ) is out of bounds." << std::endl;
				exit( -1 );
			}

			return mAttributes[ xIndex ].name;
		}

		VBMObject & operator( )( std::function<void(void)> xLambda )
		{
			glBindVertexArray( mVAO );
			xLambda( );
			glBindVertexArray( 0 );
		}

		void render( uint32_t const xFrameIndex, uint32_t const xInstances )
		{
			if ( xFrameIndex >= mHeader.numberOfFrames )
			{
				std::cerr << " render( " << xFrameIndex << ", " << xInstances << ") is out of bounds." << std::endl;
				exit( -1 );
			}

			(*this)( [ & ] ( )
			{
				if ( xInstances > 0 )
				{
					if ( mHeader.numberOfIndices > 0 )
					{
						glDrawElementsInstanced( GL_TRIANGLES
								       , mFrames[ xFrameIndex ].count
								       , GL_UNSIGNED_INT
								       , ( GLvoid* )( mFrames[ xFrameIndex ].first * sizeof( GLuint ) )
								       , xInstances );
					}
					else
					{
						glDrawArraysInstanced( GL_TRIANGLES
								     , mFrames[ xFrameIndex ].first
								     , mFrames[ xFrameIndex ].count
								     , xInstances );
					}
				}
				else
				{
					if ( mHeader.numberOfIndices > 0 )
					{
						glDrawElements( GL_TRIANGLES
							      , mFrames[ xFrameIndex ].count
							      , GL_UNSIGNED_INT
							      , ( GLvoid * )( mFrames[ xFrameIndex ] .first * sizeof(GLuint ) ) );
					}
					else
					{
						glDrawArrays( GL_TRIANGLES
							    , mFrames[ xFrameIndex ].first
							    , mFrames[ xFrameIndex ].count );
					}
				}
			});
		}
};

} // namespace sisu

#endif // VBM_D29B90D706BA486AB6119E79471EA462_HPP_
