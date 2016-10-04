#include "test.hpp"

#include "threadgears.hpp"
#include "sisumath.hpp"

#include <concurrentqueue.h>
#include <SDL2/SDL.h>
#include <iostream>

#include <math.h>
#include <map>

#include "memblock.hpp"
#include "Packet.hpp"

using namespace sisu;

namespace {

class sdl_AudioMixer_UT : public context
{
	private:
               static const char * sSounds[];
               static const uint8_t sSoundCount;

	protected:
                const char * _randomSound( ) { return sSounds[ rand( ) % ( sSoundCount - 1 ) ]; }

	public:
		sdl_AudioMixer_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};
const char * sdl_AudioMixer_UT::sSounds[] = { "resources/sound01.wav"
                                            /*, "resources/sound02.wav"
                                            , "resources/sound03.wav"
                                            , "resources/sound04.wav" */};

const uint8_t sdl_AudioMixer_UT::sSoundCount = sizeof( sSounds ) / sizeof( const char * );

typedef gear<uint8_t, uint8_t> Thread;
typedef Packet<384000> 	       AudioPacket;
typedef moodycamel::ConcurrentQueue< AudioPacket *> AudioQ;

struct _SDLAudioMixerUserData
{
	const char * issued
	         , * file;

	uint32_t eventCount
	       , bufferLength
	       , loadedLength;

	uint8_t * buffer;

	uint64_t threadID;

	SDL_AudioDeviceID device;
	void * datastore;

	AudioQ * q;
;
};

typedef std::map< uint64_t, _SDLAudioMixerUserData * > UserDataStore;

class SDLAudioMixer
{
	tooth< Thread, uint64_t> mThreadIDFactory;

	AudioQ mQ;

	mutex mLoadWAVMapMutex
	    , mOpenAudioMapMutex;

	UserDataStore mLoadWAVUserData
		    , mOpenAudioUserData;

	SDL_AudioSpec mWant
	 	    , mHave;

	uint32_t mFrameRate
	       , mSampleRate
	       , mFloatStreamLength
	       , mSamplesPerFrame
	       , mmsPerFrame;

	private:
		static void _openAudioCallback( void * xUserData, uint8_t * xStream, int32_t xLength )
		{
			_SDLAudioMixerUserData * ud = (_SDLAudioMixerUserData*)xUserData;

			AudioPacket * ap = NULL;

			if ( ud == NULL || ud->q == NULL || !ud->q->try_dequeue( ap ) || ap == NULL || xStream == NULL )
			{
				return;
			}

			SDL_memcpy( xStream, ap->getBuffer( ), xLength );

			delete ap; // TODO: Reuse objects for a lower / more predictable memory profile
		}

		static void _loadWAVCallback( void * xUserData, uint8_t * xStream, int32_t xLength )
		{
			std::cout << __PRETTY_FUNCTION__ << std::endl;
		}

		void _openAudio( _SDLAudioMixerUserData * xUD )
		{
			mWant.freq 	= mSampleRate;
			mWant.format 	= AUDIO_F32;
			mWant.channels 	= 2;
			mWant.samples 	= mFloatStreamLength;
			mWant.callback 	= _openAudioCallback;
			mWant.userdata  = static_cast<void*>( xUD );

			if ( ( xUD->device = SDL_OpenAudioDevice( NULL, 0, &mWant, &mHave, SDL_AUDIO_ALLOW_FORMAT_CHANGE ) ) == 0 )
			{
				std::cerr << "SDL_OpenAudioDevice( .. ) failed:" << SDL_GetError( ) << std::endl;
				exit( -1 );
			}

			SDL_PauseAudioDevice( xUD->device, 0 );

			mSampleRate 	   = mHave.freq;
			mFloatStreamLength = mHave.size/4;
			mSamplesPerFrame   = mSampleRate/mFrameRate;
			mmsPerFrame	   = 1000/mFrameRate;
		}

		void _loadWAV( _SDLAudioMixerUserData * xUD )
		{
			SDL_AudioSpec want, have;

			want.callback = _loadWAVCallback;
			want.userdata = static_cast<void*>( xUD );

			if ( !SDL_LoadWAV( xUD->file, &want, &xUD->buffer, &xUD->bufferLength ) )
			{
				std::cerr << "SDL_LoadWav( .. ) failed: " << SDL_GetError( ) << std::endl;
				exit( -1 );
			}
		}


	static float _generateSignal( float const xHerz )
	{
		static uint32_t const sFreq      = 8000;

	        const float generatorConstant1   = cosf( 2 * M_PI * ( xHerz / sFreq ) );
	        const float generatorConstant2   = sinf( 2 * M_PI * ( xHerz / sFreq ) );

	        static float Register[2]={1,0};
	        static float FeedBack;

	        FeedBack=2*generatorConstant1*Register[0]-Register[1];
	        Register[1]=Register[0];
	        Register[0]=FeedBack;

	        return (generatorConstant2*Register[1]);
	}

	public:
		SDLAudioMixer( uint32_t const xFrameRate  	 = 60
			     , uint32_t const xSampleRate 	 = 48000
			     , uint32_t const xFloatStreamLength = 1024 // must be ^2
			      )
			: mThreadIDFactory( )
			, mFrameRate( xFrameRate )
			, mSampleRate( xSampleRate )
			, mFloatStreamLength( xFloatStreamLength )
			, mSamplesPerFrame( 0 )
			, mmsPerFrame( 0 )
			, mQ( )
			, mLoadWAVMapMutex( )
			, mOpenAudioMapMutex( )
			, mLoadWAVUserData( )
			, mOpenAudioUserData( )
			, mWant( )
			, mHave( )
		{
			;
		}

		~SDLAudioMixer( )
		{
			auto cleanUserData = [&]( mutex xM, UserDataStore & xStore )
			{
				xM.run([&]( )
				{
					for ( auto && threadIDUserDataPair : xStore )
					{
						delete threadIDUserDataPair.second;
					}
				});
			};

			cleanUserData( mLoadWAVMapMutex, mLoadWAVUserData );
			cleanUserData( mOpenAudioMapMutex, mOpenAudioUserData );
		}

		void initialize( )
		{
			if ( ( SDL_Init( SDL_INIT_AUDIO | SDL_INIT_TIMER ) ) < 0 )
			{
				std::cerr << "SDL_Init( .. ) failed: " << SDL_GetError( ) << std::endl;
				exit( -1 );
			}

			_SDLAudioMixerUserData * udInitialize = new _SDLAudioMixerUserData( );

			udInitialize->issued       = __PRETTY_FUNCTION__;
		        udInitialize->file   	   = NULL;
			udInitialize->eventCount   = 0;
		        udInitialize->bufferLength = 0;
		        udInitialize->loadedLength = 0;
			udInitialize->buffer       = NULL;
			udInitialize->threadID     = ++mThreadIDFactory;
			udInitialize->device       = 0;
			udInitialize->datastore    = static_cast<void *>( &mOpenAudioUserData );
			udInitialize->q		   = &mQ;

			mOpenAudioMapMutex.run([&]( ){ mOpenAudioUserData[ udInitialize->threadID ] = udInitialize; });

			_openAudio( udInitialize );
		}

		// all play( ) methods are asynchronous and should return immediately.
		void playWAVImmediately( const char * xPathToWav )
		{
			/*
			_SDLAudioMixerUserData * udLoadWAV = new _SDLAudioMixerUserData( );

			udLoadWAV->issued       = "_loadWAV";
		        udLoadWAV->file   	= xPathToWav;
			udLoadWAV->eventCount   = 0;
		        udLoadWAV->bufferLength = 0;
		        udLoadWAV->loadedLength = 0;
			udLoadWAV->buffer       = NULL;
			udLoadWAV->threadID     = ++mThreadIDFactory;
			udLoadWAV->device       = 0;
			udLoadWAV->datastore    = static_cast<void *>( &mLoadWAVUserData );

			mLoadWAVMapMutex.run([&]( ){ mLoadWAVUserData[ udLoadWAV->threadID ] = udLoadWAV; });

			_loadWAV( udLoadWAV );

			_SDLAudioMixerUserData * udOpenAudio = new _SDLAudioMixerUserData( );

			udOpenAudio->issued 	  = "_OpenAudio";
		        udOpenAudio->file   	  = xPathToWav;
			udOpenAudio->eventCount   = 0;
		        udOpenAudio->loadedLength = 0;
		        udOpenAudio->bufferLength = udLoadWAV->bufferLength;
			udOpenAudio->buffer       = udLoadWAV->buffer;
			udOpenAudio->threadID     = ++mThreadIDFactory;
			udOpenAudio->device       = 0;
			udOpenAudio->datastore    = static_cast<void*>( &mOpenAudioUserData );

			mOpenAudioMapMutex.run([&]( ){ mOpenAudioUserData[ udLoadWAV->threadID ] = udLoadWAV; });

			_openAudio( udOpenAudio );

			*/
			std::cerr << __PRETTY_FUNCTION__ << "(" << xPathToWav << ")" << std::endl;
		}

		void playSineWave( double const xDuration )
		{
			event quit;

			Thread enqueueSineWave( [ & ]( uint8_t )->uint8_t
			{
				static uint32_t const sMaxHerz = 888;
				static uint32_t const sMinHerz = 8;
			        static int32_t const step = 3000;
			        int32_t stepper = 0;

			        float herz = sMaxHerz;
			        bool reverse = false;


				while ( !quit.isSet( ) )
				{
					uint32_t audioBufferSize = mHave.freq;

					if ( audioBufferSize % mSamplesPerFrame )
					{
						audioBufferSize += mSamplesPerFrame
								 - ( audioBufferSize % mSamplesPerFrame );
					}

					float * buffer = new float[ audioBufferSize ];

					for ( uint32_t ii = 0; ii < audioBufferSize; ++ii )
					{
						buffer[ ii ] = _generateSignal( herz );

                                                if ( ++stepper % step == 0 )
                                                {
                                                        if ( !reverse )
                                                        {
                                                                herz += 3.33;

                                                                if ( herz >= sMaxHerz)
                                                                {
                                                                        reverse = true;
                                                                }
								continue;
							}

							herz -= 3.33;

                                                        if( herz <= sMinHerz )
                                                        {
                                                                  reverse = false;
                                                        }
                                                }
					}

					AudioPacket * pAP = new AudioPacket( );

					if (pAP->getSize( ) < audioBufferSize * sizeof( float ))
					{
						std::cerr << "Audio buffer overflow: " << pAP->getSize( )
							  << " < "		       << ( audioBufferSize * sizeof( float ) )
							  << std::endl;
						exit( -1 );
					}

					pAP->writeBuffer(  (uint8_t*)buffer, audioBufferSize*sizeof(float) );

					mQ.enqueue( pAP );
				}

				return 0;
			} );

			enqueueSineWave( 0 );

			sleep::ms( xDuration );

			quit.set( );

			enqueueSineWave.join( );
			std::cerr << __PRETTY_FUNCTION__ << "(" << xDuration << ")" << std::endl;
		}

		void playRandomNoise( double const xDuration )
		{
			event quit;

			Thread enqueueRandomNoise( [ & ]( uint8_t )->uint8_t
			{
				while ( !quit.isSet( ) )
				{
					uint32_t audioBufferSize = mHave.freq;

					if ( audioBufferSize % mSamplesPerFrame )
					{
						audioBufferSize += mSamplesPerFrame
								 - ( audioBufferSize % mSamplesPerFrame );
					}

					float * buffer = new float[ audioBufferSize ];

					for ( uint32_t ii = 0; ii < audioBufferSize; ++ii )
					{
						buffer[ ii ] = static_cast< float >( rand( ) ) / static_cast< float > ( RAND_MAX );
					}

					AudioPacket * pAP = new AudioPacket( );

					if (pAP->getSize( ) < audioBufferSize * sizeof( float ))
					{
						std::cerr << "Audio buffer overflow: " << pAP->getSize( )
							  << " < "		       << ( audioBufferSize * sizeof( float ) )
							  << std::endl;
						exit( -1 );
					}

					pAP->writeBuffer(  (uint8_t*)buffer, audioBufferSize*sizeof(float) );

					mQ.enqueue( pAP );
				}

				return 0;
			});

			enqueueRandomNoise( 0 );

			sleep::ms( xDuration );

			quit.set( );

			enqueueRandomNoise.join( );

			std::cerr << __PRETTY_FUNCTION__ << "(" << xDuration << ")" << std::endl;
		}

		// eventually..
		/* void playNote( double const xPitch, xPatch, xSample, etc. ) */
};

} // namespace

TEST(sdl_AudioMixer_UT, PlaySDLAudioWithoutExceptions)
{
	event quit;

	SDLAudioMixer mixer;

	mixer.initialize( );

	Thread requesterThread([&](uint8_t)->uint8_t
	{
		while (!quit.isSet( ) )
		{
			//mixer.playWAVImmediately( _randomSound( ) );

			mixer.playSineWave( 15000.0 );

			mixer.playRandomNoise( 3000.0 );

			sleep::ms( rand( ) % 3000 );
		}
	});

	requesterThread( 0 );

	while ( !quit.isSet( ) )
	{
		// do whatever
		sleep::ms( 3000 ); 
	};

	BLOCK_EXECUTION;
}
