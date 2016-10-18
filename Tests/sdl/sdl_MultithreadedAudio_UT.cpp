#ifdef SDL_MULTITHREADED_AUDIO_UT
#include "test.hpp"
#include "threadgears.hpp"
#include "memblock.hpp"
#include <string>
#include <iostream>
#include <map>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace sisu;

namespace
{

class sdl_MultiThreadedAudio_UT : public context
{
	protected:
		static const char * sSounds[];

		static const uint8_t sSoundCount;

		const char * _randomSound( ) { return sSounds[ rand( ) % ( sSoundCount - 1 ) ]; }

	public:
		sdl_MultiThreadedAudio_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

const char * sdl_MultiThreadedAudio_UT::sSounds[] = { "resources/sound01.wav"
						    , "resources/sound05.wav" };

const uint8_t sdl_MultiThreadedAudio_UT::sSoundCount = sizeof( sSounds ) / sizeof( const char * );


// Class should probably inherit ISingleton when it is done.
class AudioPlayer
{
	struct AudioSpecUserdata
	{
		const char * issued;
		const char * file;

		uint32_t eventCount
		       , bufferLength
		       , loadedLength;

		uint8_t * buffer;

		uint64_t threadID;

		SDL_AudioDeviceID device;

		void print( )
		{
			std::cout << "issued:       " << issued       << std::endl
				  << "eventCount:   " << eventCount   << std::endl
				  << "bufferLength: " << bufferLength << std::endl
				  << "loadedLength: " << loadedLength << std::endl
				  << "threadID:     " << threadID     << std::endl
				  << memblock( buffer, bufferLength ) << std::endl;
		}

	};

	static mutex sOpenAudioMapMutex, sLoadWavMapMutex;
	static std::map<uint64_t, AudioSpecUserdata* > sOpenAudioUserDataMap
					      	     , sLoadWavCallbackUserDataMap;

	tooth<AudioPlayer, uint64_t> mThreadIDs;

	static void _removeFromMap( uint64_t const xThreadID, std::map< uint64_t, AudioSpecUserdata * > & xMap )
	{
		std::map< uint64_t, AudioSpecUserdata* >::iterator it = xMap.find( xThreadID );

		if ( it == xMap.end( ) )
		{
			std::cerr << "Thread ID " << xThreadID << " was missing user data." << std::endl;
			exit( -1 );
		}


		SDL_CloseAudioDevice( it->second->device );
		delete it->second;

		xMap.erase( it );
	}

	static void _processUserData( AudioSpecUserdata * xData, uint8_t * xStream, int32_t xLength)
	{
		uint32_t newLength;

		if ( xData == NULL )
		{
			std::cerr << "User data was not found." << std::endl;
			exit( -1 );
		}

		xData->eventCount++;

		xData->print( );

		if ( xLength > 0 && xData->buffer )
		{
			if ( ( newLength = xData->loadedLength + xLength ) > xData->bufferLength )
			{
				xLength   = xData->bufferLength - xData->loadedLength;
				newLength = xData->bufferLength;
			}

			SDL_memcpy( xStream, xData->buffer + xData->loadedLength, xLength );

			xData->loadedLength = newLength;
		}

	}

	static void _openAudioCallback( void * xUserData, uint8_t * xStream, int32_t xLength )
	{
		AudioSpecUserdata * data = ( AudioSpecUserdata * )xUserData;

		_processUserData( data, xStream, xLength );

		if( data->loadedLength >= data->bufferLength )
		{
			sOpenAudioMapMutex.run([&]()
			{
				_removeFromMap( data->threadID, sOpenAudioUserDataMap );
			} );
		}
	}

	static void _loadWAVCallback( void * xUserData, uint8_t * xStream, int32_t xLength )
	{
		AudioSpecUserdata * data = ( AudioSpecUserdata * )xUserData;

		_processUserData( data, xStream, xLength );

		if( data->loadedLength >= data->bufferLength )
		{
			sLoadWavMapMutex.run([&]()
			{
				_removeFromMap( data->threadID, sOpenAudioUserDataMap );
			} );

		}
	}

	public:
		AudioPlayer( )
			: mThreadIDs( )
		{
		 	;
		}

		~AudioPlayer( )
		{
			// TODO: clean up maps..
			SDL_CloseAudio( );
		}

		void initialize( )
		{
			if ( SDL_Init( SDL_INIT_AUDIO ) < 0 )
			{
				std::cerr << " SDL_Init( SDL_INIT_AUDIO ) failed: " << SDL_GetError( ) << std::endl;
				exit( -1 );
			}

		}

		void play( const char * xPath )
		{
			std::cout << xPath << std::endl;

			uint64_t threadID = ++mThreadIDs; // should be a 'tooth'

			uint32_t  audioLength = 0;

			SDL_AudioSpec wantWAVSpec, haveWAVSpec;

			uint8_t * audioBuffer = NULL;

			AudioSpecUserdata * ud = NULL;

			sLoadWavMapMutex.run([&]()
			{
				ud = sLoadWavCallbackUserDataMap[ threadID ] = new AudioSpecUserdata( );
			});


			wantWAVSpec.callback = _loadWAVCallback;
			wantWAVSpec.userdata = (void*)ud;

			ud->file 	 = xPath;
			ud->loadedLength = 0;

			if ( !SDL_LoadWAV( xPath, &wantWAVSpec, &audioBuffer, &audioLength ) )
			{
				std::cerr << "SDL_LoadWav( .. ) failed: " << SDL_GetError( ) << std::endl;
				exit( -1 );
			}

			ud->issued 	 = "SDL_LoadWAV";
			ud->buffer 	 = audioBuffer;
			ud->bufferLength = audioLength;
			ud->eventCount   = 0;

			wantWAVSpec.callback = _openAudioCallback;

			sOpenAudioMapMutex.run([&]()
			{
				ud = sOpenAudioUserDataMap[ threadID ] = new AudioSpecUserdata( );
			});

			ud->issued 	 = "OpenAudio";
			ud->file   	 = xPath;
			ud->buffer 	 = audioBuffer;
			ud->bufferLength = audioLength;
			ud->eventCount   = 0;

			if ( ( ud->device = SDL_OpenAudioDevice( NULL, 0, &wantWAVSpec, &haveWAVSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE ) ) == 0 )
			{
				std::cerr << "SDL_OpenAudio( .. ) failed: " << SDL_GetError( ) << std::endl;
				exit( -1 );
			}

			// Otherwise, unpause audio and begin playback.
			SDL_PauseAudioDevice( ud->device, 0 );
		}

		void silence( ) { }
};

mutex AudioPlayer::sOpenAudioMapMutex;
mutex AudioPlayer::sLoadWavMapMutex;

std::map<uint64_t, AudioPlayer::AudioSpecUserdata* > AudioPlayer::sOpenAudioUserDataMap;
std::map<uint64_t, AudioPlayer::AudioSpecUserdata* > AudioPlayer::sLoadWavCallbackUserDataMap;

} // namespace


TEST(sdl_MultiThreadedAudio_UT, OrchestrateAudioFromBackgroundThread)
{
	AudioPlayer player;

	event quit;

	gear<int32_t,uint32_t> requesterThread( [ &]( uint32_t xSleep )->int32_t
	{
//		do
//		{
			sleep::ms( rand( ) % xSleep );

			player.play( _randomSound( ) );

//		} while ( !quit.isSet( ) );

		std::cout << "Goodbye!" << std::endl;

		return 0;
	});

	requesterThread( 1000 );
///	requesterThread( 4000 );
//	requesterThread( 500 );
//	requesterThread( 200 );

	sleep::ms( 30000 );

	quit.set( );

	BLOCK_EXECUTION;
}
#endif // SDL_MULTITHREADED_AUDIO_UT
