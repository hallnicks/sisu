#include <IL/il.h>

static class DevILInit {

	public:
		DevILInit( )
		{
			ilInit( );
		}

} sDevILInitializer;
