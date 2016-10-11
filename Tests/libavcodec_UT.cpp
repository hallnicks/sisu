#include "test.hpp"

#include "memblock.hpp"
#include "ttyc.hpp"
#include "ioassist.hpp"

#include "SDLShaderTest.hpp"
#include "threadgears.hpp"

#include <functional>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
}

using namespace sisu;

namespace
{
	class libavcodec_UT : public context
	{

		static constexpr const char * sInputMp4 = "trip.mp4";

		public:
			libavcodec_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};

	static class _libavcodecInitializer
	{
		public:
			_libavcodecInitializer( ) { avcodec_register_all( ); };


	} _slibavcodecInitializer;


	typedef std::function<void(uint8_t*, uint64_t, uint64_t)> OnDecodeFrame;

	template< uint64_t XInBufSize = 4096 >
	class AvCodecDecoder
	{
		AVCodec * mCodec;

		AVCodecContext * mCtx;

		uint64_t mFrameCount;

		uint8_t mInBuff[ XInBufSize + AV_INPUT_BUFFER_PADDING_SIZE ];

		AVPacket mAVPacket;

		AVFrame * mFrame;

		void decode_write_frame( OnDecodeFrame xOnDecodeFrame )
		{
			int32_t length, gotFrame;
			char buff[1024];

			if ((length = avcodec_decode_video2( mCtx, mFrame, &gotFrame, &mAVPacket )) < 0 )
			{
				std::cerr << "avcodec_decode_video2( .. ) failed." << std::endl;
				exit( -1 );
			}
			if ( gotFrame > 0 )
			{
				xOnDecodeFrame(( uint8_t* )mFrame->data[ 0 ]
					     , ( uint64_t )mFrame->width
					     , ( uint64_t )mFrame->height);

				++mFrameCount;
			}

			if ( mAVPacket.data )
			{
				mAVPacket.size -= length;
				mAVPacket.data += length;
			}
		}

		public:
			AvCodecDecoder( )
				: mCodec( NULL )
				, mCtx( NULL )
				, mFrameCount( 0 )
				, mInBuff( )
				, mAVPacket( )
				, mFrame( NULL )
			{
				av_init_packet( &mAVPacket );
				memset( mInBuff + XInBufSize, 0, AV_INPUT_BUFFER_PADDING_SIZE );
			}


			void decode( const char * xFilename, OnDecodeFrame xOnDecodeFrame )
			{
				FILE * fp = NULL;

				if ( ( mCodec = avcodec_find_decoder( AV_CODEC_ID_MPEG1VIDEO ) ) == NULL )
				{
					std::cerr << "avcodec_find_decoer( .. ) failed." <<std::endl;
					exit( -1 );
				}

				if ( ( mCtx = avcodec_alloc_context3( mCodec ) ) == NULL )
				{
					std::cerr << "avcodec_alloc_context3( .. ) failed." << std::endl;
					exit( -1 );
				}

				if ( mCodec->capabilities & AV_CODEC_CAP_TRUNCATED )
				{
					mCtx->flags |= AV_CODEC_FLAG_TRUNCATED;
				}

				// Open the codec
				if ( avcodec_open2( mCtx, mCodec, NULL ) < 0 )
				{
					std::cerr << "avcodec_open2( .. ) failed." << std::endl;
					exit( -1 );
				}

				if ( ( fp = fopen( xFilename, "rb" ) ) == NULL )
				{
					std::cerr << "Could not open " << xFilename << " for reading." << std::endl;
					exit( -1 );
				}

				if ( ( mFrame = av_frame_alloc( ) ) == NULL )
				{
					std::cerr << "av_frame_alloc( ) failed." << std::endl;
					exit( -1 );
				}

				mFrameCount = 0;

				while ( 1 )
				{
					if ( ( mAVPacket.size = fread( mInBuff, 1, XInBufSize, fp ) ) == 0 )
					{
						break;
					}

					mAVPacket.data = mInBuff;

					while ( mAVPacket.size > 0 )
					{
						decode_write_frame( xOnDecodeFrame );
					}
				}

				mAVPacket.data = NULL;
				mAVPacket.size = 0;

				decode_write_frame( xOnDecodeFrame );

				fclose( fp );

				avcodec_close( mCtx );

				av_free( mCtx );

				av_frame_free( &mFrame );
			}
	};


typedef gear<uint8_t, uint8_t> Thread;

} // namespace

#if 0
// Ensures writes are stateless ( at least for one iteration ) ..
TEST(libavcodec_UT, writeAVCODECFile)
{
	{
		SpriteShader window( true );

		Thread decodeThread([&](uint8_t)->uint8_t
		{
			AvCodecDecoder<4096> decoder;

			auto onDecodeFrame = [&]( uint8_t * xBuffer
						, uint64_t xWidth
						, uint64_t xHeight )
			{
				window.enqueue( (GLubyte*)xBuffer, xWidth, xHeight );
			};

			decoder.decode( "resources/test.mpg", onDecodeFrame );
		});

		window.initialize( { 4, 4, 0, false, false } );

		decodeThread( 0 );

		window.run( );

		decodeThread.join( );
	}
}
#endif
