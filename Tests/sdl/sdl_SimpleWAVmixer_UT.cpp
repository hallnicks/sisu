#ifdef SDL_SIMPLE_WAVE_MIXER_UT
#include "test.hpp"
#include "threadgears.hpp"
#include <string>
#include <iostream>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <Shlwapi.h>

using namespace sisu;

namespace
{

class sdl_SimpleWAVMixer_UT : public context
{
	public:
		sdl_SimpleWAVMixer_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

static Uint8 *audio_pos;
static Uint32 audio_len;


void my_audio_callback(void *userdata, Uint8 *stream, int len) {

	if (audio_len ==0)
		return;

	len = ( len > audio_len ? audio_len : len );
	SDL_memcpy (stream, audio_pos, len); 	// simply copy from one buffer into the other
	//SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another -- Does not work!!

	audio_pos += len;
	audio_len -= len;
}


} // namespace

TEST(sdl_SimpleWAVMixer_UT, PlaySDLAudioWithoutExceptions)
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		std::cout << "SDL_Init failed." << std::endl;
		exit(-1); 
	}

	static Uint32 wav_length;
	static Uint8 *wav_buffer;
	static SDL_AudioSpec wav_spec;

	if( SDL_LoadWAV( "resources/sound01.wav", &wav_spec, &wav_buffer, &wav_length) == NULL ){
		std::cout << "SDL_LoadWAV failed." << std::endl;
		exit( -1 );
	}

	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;

	audio_pos = wav_buffer;
	audio_len = wav_length;

	if ( SDL_OpenAudio(&wav_spec, NULL) < 0 )
	{
		std::cout << "SDL_OpenAudio failed." << std::endl;
		exit(-1);
	}

	SDL_PauseAudio( 0 );

	sleep::ms( 10000 );

	SDL_CloseAudio( );

	SDL_FreeWAV( wav_buffer );
}
#endif // SDL_SIMPLE_WAVE_MIXER_UT
