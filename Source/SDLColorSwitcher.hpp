#ifndef SDL_COLOR_SWITCHER_6B4D92E204094A3AB8A7275087A55B87_HPP_
#define SDL_COLOR_SWITCHER_6B4D92E204094A3AB8A7275087A55B87_HPP_

#include "SDLTest.hpp"

namespace sisu {
        class SDLColorSwitcher : public SDLTestWindow
        {
                protected:
                        virtual void render( );

                public:
                        SDLColorSwitcher( );

                        virtual void run( );
        }; // class
} // namespace sisu
#endif // SDL_COLOR_SWITCHER_6B4D92E204094A3AB8A7275087A55B87_HPP_
