#if 0
#include "test.hpp"
#include "threadgears.hpp"
#include "sisumath.hpp"
#include "memblock.hpp"
#include "Packet.hpp"
#include "Stopwatch.hpp"


#include "OpenAL/OpenAL.hpp"
#include "OpenAL/Framework.h"

#include <concurrentqueue.h>
#include <iostream>

#include <math.h>
#include <limits>

using namespace sisu;

namespace {

class OpenAL_UT : public context
{

	public:
		OpenAL_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

// From SDK
TEST(OpenAL_UT, PlayAudioWithoutExceptions)
{
#define	TEST_WAVE_FILE		"resources\\sound06.wav"
	ALuint      uiBuffer;
	ALuint      uiSource;
	ALint       iState;

	// Initialize Framework
	ALFWInit();

	ALFWprintf("PlayStatic Test Application\n");

	if (!ALFWInitOpenAL())
	{
		ALFWprintf("Failed to initialize OpenAL\n");
		ALFWShutdown();
	}

	// Generate an AL Buffer
	alGenBuffers( 1, &uiBuffer );

	// Load Wave file into OpenAL Buffer
	if (!ALFWLoadWaveToBuffer((char*)TEST_WAVE_FILE, uiBuffer))
	{
		ALFWprintf("Failed to load %s\n", TEST_WAVE_FILE);
		exit( -1 );
	}

	// Generate a Source to playback the Buffer
	alGenSources( 1, &uiSource );

	// Attach Source to Buffer
	alSourcei( uiSource, AL_BUFFER, uiBuffer );

	// Play Source
	TRACE;
	alSourcePlay( uiSource );
	ALFWprintf("Playing Source ");

	do
	{
		Sleep(100);
		ALFWprintf(".");
		// Get Source State
		alGetSourcei( uiSource, AL_SOURCE_STATE, &iState);
	} while (iState == AL_PLAYING);

	ALFWprintf("\n");

	// Clean up by deleting Source(s) and Buffer(s)
	alSourceStop(uiSource);
	alDeleteSources(1, &uiSource);
	alDeleteBuffers(1, &uiBuffer);

	ALFWShutdownOpenAL();
	ALFWShutdown();
}
#endif
