#ifndef DRAWINGS_99BB3631_3CE9_4F0A_9A33_0A792E6750A7_HPP_
#define DRAWINGS_99BB3631_3CE9_4F0A_9A33_0A792E6750A7_HPP_

#include "screen.hpp"

namespace sisu
{
        class Drawings
        {
                Screen * mS;

                public:
                        Drawings( Screen * xScreen );
                        ~Drawings( );
                        void clear( TTYC xC );
                        void mandlebrot( TTYC xC, double scale = 1.0 );
        };

} // namespace sisu

#endif // DRAWINGS_99BB3631_3CE9_4F0A_9A33_0A792E6750A7_HPP_
