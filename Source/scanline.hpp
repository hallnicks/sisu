#ifndef SCANLINE_ACF4BD97_E311_4658_BA43_5D17CBEFF16B_HPP_
#define SCANLINE_ACF4BD97_E311_4658_BA43_5D17CBEFF16B_HPP_

#include "ttycolor.hpp"

namespace sisu
{

class Scanline
{
        TTYC * mBuffer;

        uint8_t mWidth;

        public:
                Scanline( TTYC * const xBuffer, uint8_t const xWidth );

                TTYC * const operator [ ] ( uint8_t const xIdx ) const;
};

} // namespace sisu

#endif // SCANLINE_ACF4BD97_E311_4658_BA43_5D17CBEFF16B_HPP_



