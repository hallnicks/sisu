#ifdef LIBAVCODEC_UT
#include "test.hpp"

#include "memblock.hpp"
#include "ttyc.hpp"
#include "ioassist.hpp"
#include "memblock.hpp"

#include "SDLShaderTest.hpp"
#include "threadgears.hpp"

#include <functional>

extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}

using namespace sisu;

namespace
{
	static inline const char * _sisuFFMPEGErr(int const xErrNum)
	{
		static char temp[AV_ERROR_MAX_STRING_SIZE];

		av_make_error_string(temp, AV_ERROR_MAX_STRING_SIZE, xErrNum);

		return temp;
	}

	static inline const char * _sisuFFMPEGTimeStr( int64_t const xTs, AVRational * xTb )
	{
		static char temp[AV_TS_MAX_STRING_SIZE];

		av_ts_make_time_string(temp, xTs, xTb );

		return temp;
	}

	static void _outputAudioData( uint8_t * xData, uint64_t const xSize )
	{
		std::cout << ccolor( eTTYCRed, eTTYCMagenta, eModNone )
			  << memblock( xData, xSize )
			  << std::endl;
	}

	static void _outputVideoData( uint8_t * xData, uint64_t const xSize )
	{
		std::cout << ccolor( eTTYCYellow, eTTYCBlack, eModNone )
			  << memblock( xData, xSize )
			  << std::endl;
	}

	class libavcodec_UT : public context
	{

		static constexpr const char * sInputMp4 = "trip.mp4";

		public:
			libavcodec_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};

	class AVCodecDecoder
	{

		typedef std::function<void(uint8_t *, uint64_t)> OnReceiveData;

		typedef std::vector<OnReceiveData> ReceiveDataCallbacks;

		ReceiveDataCallbacks mAudioCallbacks, mVideoCallbacks;

		AVFormatContext *fmt_ctx;
		AVCodecContext *video_dec_ctx
			     , *audio_dec_ctx;
		int width, height;
		enum AVPixelFormat pix_fmt;
		AVStream  *video_stream
			, *audio_stream;
		uint8_t *video_dst_data[4];

		int      video_dst_linesize[4];
		int video_dst_bufsize;
		int video_stream_idx, audio_stream_idx;
		AVFrame *frame;
		AVPacket pkt;
		int video_frame_count;
		int audio_frame_count;

		/* The different ways of decoding and managing data memory. You are not
		 * supposed to support all the modes in your application but pick the one most
		 * appropriate to your needs. Look for the use of api_mode in this example to
		 * see what are the differences of API usage between them */
		enum {
		    API_MODE_NEW_API_REF_COUNT    = 0, /* new method, using the frame reference counting */
		    API_MODE_NEW_API_NO_REF_COUNT = 1, /* new method, without reference counting */
		} api_mode;

		void _pushAudioData( uint8_t * xData, uint64_t const xSize )
		{
			for ( auto && ii : mAudioCallbacks )
			{
				ii( xData, xSize );
			}
		}

		void _pushVideoData( uint8_t * xData, uint64_t const xSize )
		{
			for ( auto && ii : mVideoCallbacks )
			{
				ii( xData, xSize );
			}
		}

		int decode_packet(int *got_frame, int cached)
		{
		    int ret = 0;
		    int decoded = pkt.size;
		    *got_frame = 0;
		    if (pkt.stream_index == video_stream_idx) {
		        /* decode video frame */
		        ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
		        if (ret < 0) {
		            fprintf(stderr, "Error decoding video frame (%s)\n",  _sisuFFMPEGErr(ret));
		            return ret;
		        }
		        if (video_dec_ctx->width != width || video_dec_ctx->height != height ||
		            video_dec_ctx->pix_fmt != pix_fmt) {
		            /* To handle this change, one could call av_image_alloc again and
		             * decode the following frames into another rawvideo file. */
		            fprintf(stderr, "Error: Width, height and pixel format have to be "
		                    "constant in a rawvideo file, but the width, height or "
		                    "pixel format of the input video changed:\n"
		                    "old: width = %d, height = %d, format = %s\n"
		                    "new: width = %d, height = %d, format = %s\n",
		                    width, height, av_get_pix_fmt_name(pix_fmt),
		                    video_dec_ctx->width, video_dec_ctx->height,
		                    av_get_pix_fmt_name(video_dec_ctx->pix_fmt));
		            return -1;
		        }
		        if (*got_frame) {
		            printf("video_frame%s n:%d coded_n:%d pts:%s\n",
		                   cached ? "(cached)" : "",
		                   video_frame_count++, frame->coded_picture_number,
					 _sisuFFMPEGTimeStr(frame->pts, &video_dec_ctx->time_base));
		            /* copy decoded frame to destination buffer:
		             * this is required since rawvideo expects non aligned data */
		            av_image_copy(video_dst_data, video_dst_linesize,
                		          (const uint8_t **)(frame->data), frame->linesize,
		                          pix_fmt, width, height);
		            /* write to rawvideo file */
			    _pushVideoData( video_dst_data[0], video_dst_bufsize ); 
		        }
		    } else if (pkt.stream_index == audio_stream_idx) {
		        /* decode audio frame */
		        ret = avcodec_decode_audio4(audio_dec_ctx, frame, got_frame, &pkt);
		        if (ret < 0) {
		            fprintf(stderr, "Error decoding audio frame (%s)\n",  _sisuFFMPEGErr(ret) );
		            return ret;
		        }
		        /* Some audio decoders decode only part of the packet, and have to be
		         * called again with the remainder of the packet data.
		         * Sample: fate-suite/lossless-audio/luckynight-partial.shn
		         * Also, some decoders might over-read the packet. */
		        decoded = FFMIN(ret, pkt.size);
		        if (*got_frame) {
		            size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
		            printf("audio_frame%s n:%d nb_samples:%d pts:%s\n",
		                   cached ? "(cached)" : "",
		                   audio_frame_count++, frame->nb_samples,
		                    _sisuFFMPEGTimeStr(frame->pts, &audio_dec_ctx->time_base));
		            /* Write the raw audio data samples of the first plane. This works
		             * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
		             * most audio decoders output planar audio, which uses a separate
		             * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
		             * In other words, this code will write only the first audio channel
		             * in these cases.
		             * You should use libswresample or libavfilter to convert the frame
		             * to packed data. */
			    _pushAudioData( frame->extended_data[0], unpadded_linesize );
		        }
		    }
		    /* If we use the new API with reference counting, we own the data and need
		     * to de-reference it when we don't use it anymore */
		    if (*got_frame && api_mode == API_MODE_NEW_API_REF_COUNT)
		        av_frame_unref(frame);
		    return decoded;
		}

		int open_codec_context(int *stream_idx,
		                              AVFormatContext *fmt_ctx, enum AVMediaType type)
		{
		    int ret, stream_index;
		    AVStream *st;
		    AVCodecContext *dec_ctx = NULL;
		    AVCodec *dec = NULL;
		    AVDictionary *opts = NULL;
		    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
		    if (ret < 0) {
		        fprintf(stderr, "Could not find %s stream in input file \n",
		                av_get_media_type_string(type) );
		        return ret;
		    } else {
		        stream_index = ret;
		        st = fmt_ctx->streams[stream_index];
		        /* find decoder for the stream */
		        dec_ctx = st->codec;
		        dec = avcodec_find_decoder(dec_ctx->codec_id);
		        if (!dec) {
		            fprintf(stderr, "Failed to find %s codec\n",
			                    av_get_media_type_string(type));
		            return AVERROR(EINVAL);
		        }
		        /* Init the decoders, with or without reference counting */
		        if (api_mode == API_MODE_NEW_API_REF_COUNT)
		            av_dict_set(&opts, "refcounted_frames", "1", 0);
		        if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0) {
		            fprintf(stderr, "Failed to open %s codec\n",
		                    av_get_media_type_string(type));
		            return ret;
		        }
		        *stream_idx = stream_index;
		    }
		    return 0;
		}

		int get_format_from_sample_fmt(const char **fmt,
		                                      enum AVSampleFormat sample_fmt)
		{
		    int i;
		    struct sample_fmt_entry {
		        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
		    } sample_fmt_entries[] = {
		        { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
		        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
		        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
		        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
		        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
		    };
		    *fmt = NULL;
		    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
		        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
		        if (sample_fmt == entry->sample_fmt) {
		            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
		            return 0;
		        }
		    }
		    fprintf(stderr,
		            "sample format %s is not supported as output format\n",
		            av_get_sample_fmt_name(sample_fmt));
		    return -1;
		}

		public:
			AVCodecDecoder( )
				: mAudioCallbacks( )
				, mVideoCallbacks( )
				, fmt_ctx( NULL )
				, video_dec_ctx( NULL )
				, audio_dec_ctx( NULL )
				, width( 0 )
				, height( 0 )
				, pix_fmt( AV_PIX_FMT_NONE )
				, video_stream( NULL )
				, audio_stream( NULL )
				, video_dst_data( )
				, video_dst_linesize( )
				, video_dst_bufsize( )
				, video_stream_idx( -1 )
				, audio_stream_idx( -1 )
				, frame( NULL )
				, pkt( )
				, video_frame_count( 0 )
				, audio_frame_count( 0 )
				, api_mode( API_MODE_NEW_API_REF_COUNT )
			{
				;
			}

			static void initialize( )
			{
				static bool sInitialized = false;

				if ( !sInitialized )
				{
					av_register_all( );

					sInitialized = true;
				}
			}

			void decode( const char * src_filename )
			{
			    int ret = 0, got_frame;

			    /* open input file, and allocate format context */
			    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
			        fprintf(stderr, "Could not open source file %s\n", src_filename);
			        exit(1);
			    }
			    /* retrieve stream information */
			    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
			        fprintf(stderr, "Could not find stream information\n");
			        exit(1);
			    }
			    if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
			        video_stream = fmt_ctx->streams[video_stream_idx];
			        video_dec_ctx = video_stream->codec;
			        /* allocate image where the decoded image will be put */
			        width = video_dec_ctx->width;
			        height = video_dec_ctx->height;
			        pix_fmt = video_dec_ctx->pix_fmt;
			        ret = av_image_alloc(video_dst_data, video_dst_linesize,
				                             width, height, pix_fmt, 1);
			        if (ret < 0) {
			            fprintf(stderr, "Could not allocate raw video buffer\n");
			            goto end;
			        }
			        video_dst_bufsize = ret;
			    }
			    if (open_codec_context(&audio_stream_idx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
			        audio_stream = fmt_ctx->streams[audio_stream_idx];
			        audio_dec_ctx = audio_stream->codec;
			    }
			    /* dump input information to stderr */
			    av_dump_format(fmt_ctx, 0, src_filename, 0);
			    if (!audio_stream && !video_stream) {
			        fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
			        ret = 1;
			        goto end;
			    }
			    /* When using the new API, you need to use the libavutil/frame.h API, while
			     * the classic frame management is available in libavcodec */
			    frame = av_frame_alloc();
			    if (!frame) {
			        fprintf(stderr, "Could not allocate frame\n");
			        ret = AVERROR(ENOMEM);
			        goto end;
			    }
			    /* initialize packet, set data to NULL, let the demuxer fill it */
			    av_init_packet(&pkt);
			    pkt.data = NULL;
			    pkt.size = 0;
			    /* read frames from the file */
			    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
			        AVPacket orig_pkt = pkt;
			        do {
			            ret = decode_packet(&got_frame, 0);
			            if (ret < 0)
			                break;
			            pkt.data += ret;
			            pkt.size -= ret;
			        } while (pkt.size > 0);
			        av_free_packet(&orig_pkt);
			    }
			    /* flush cached frames */
			    pkt.data = NULL;
			    pkt.size = 0;
			    do {
			        decode_packet(&got_frame, 1);
			    } while (got_frame);
			    printf("Demuxing succeeded.\n");
			    if (audio_stream) {
			        enum AVSampleFormat sfmt = audio_dec_ctx->sample_fmt;
			        int n_channels = audio_dec_ctx->channels;
			        const char *fmt;
			        if (av_sample_fmt_is_planar(sfmt)) {
			            const char *packed = av_get_sample_fmt_name(sfmt);
			            printf("Warning: the sample format the decoder produced is planar "
			                   "(%s). This example will output the first channel only.\n",
			                   packed ? packed : "?");
			            sfmt = av_get_packed_sample_fmt(sfmt);
			            n_channels = 1;
			        }
			        if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0)
			            goto end;
			    }
			end:
			    avcodec_close(video_dec_ctx);
			    avcodec_close(audio_dec_ctx);
			    avformat_close_input(&fmt_ctx);

			    av_frame_free(&frame);
			    av_free(video_dst_data[0]);
				if ( ret < 0 )
				{
					std::cerr << __PRETTY_FUNCTION__ << " test failed." << std::endl;
					exit( -1 );
				}

			}

			void registerAudioCallback( OnReceiveData xOnReceiveAudioData )
			{
				mAudioCallbacks.push_back( xOnReceiveAudioData );
			}
			void registerVideoCallback( OnReceiveData xOnReceiveVideoData )
			{
				mVideoCallbacks.push_back( xOnReceiveVideoData );
			}

	};
} // namespace


TEST(libavcodec_UT, avcodecDemuxDecodeExample)
{
	AVCodecDecoder::initialize( );

	AVCodecDecoder decoder;

	decoder.registerAudioCallback( _outputAudioData );
	decoder.registerVideoCallback( _outputVideoData );

	decoder.decode( "resources/trip.mp4" );
}
#endif
