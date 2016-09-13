#include "scanline.hpp"


namespace sisu
{

Scanline::Scanline( TTYC * const xBuffer, uint8_t const xWidth )
        : mBuffer( xBuffer )
        , mWidth( xWidth)
{
        ;
}

TTYC * const Scanline::operator [ ] ( uint8_t const xIdx ) const { return & mBuffer [ xIdx ];  }

} // namespace sisu
