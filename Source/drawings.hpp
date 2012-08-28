#ifndef DRAWINGS_99BB3631_3CE9_4F0A_9A33_0A792E6750A7_HPP_
#define DRAWINGS_99BB3631_3CE9_4F0A_9A33_0A792E6750A7_HPP_

#include "screen.hpp"
#include "primitives.hpp"

namespace sisu
{
        class Drawings
        {
                Screen * mS;


                public:
                        Drawings( Screen * xScreen );

                        ~Drawings( );

			void hline( unsigned int const xY, unsigned int const xStart, unsigned int const xWidth, TTYC const xC );

			void setPixel( unsigned int const xX, unsigned int const xY, TTYC const xC );

			//void hquad( Quad * const xQuad, TTYC const xC );

                        void mandlebrot( TTYC const xC, double const scale = 1.0 );

			void midpointCircle( unsigned int const xX, unsigned int const xY, unsigned int const xRadius, TTYC const xC );

                        void clear( TTYC const xC );
        };

} // namespace sisu

#endif // DRAWINGS_99BB3631_3CE9_4F0A_9A33_0A792E6750A7_HPP_
