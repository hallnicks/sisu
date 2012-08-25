#include "drawings.hpp"

namespace sisu
{

Drawings::Drawings( Screen * xScreen ) : mS( xScreen ) { }

Drawings::~Drawings( ) { }

void Drawings::clear( TTYC xC )
{
	for ( unsigned int y = 0; y < mS->getHeight( ); ++y )
	{
		for ( unsigned int x = 0; x < mS->getWidth( ); ++x )
		{
			(*(*mS)[ y ][ x ]) = xC;
		}
	}
}

void Drawings::mandlebrot( TTYC xC, double xScale )
{
	unsigned int const 	h = static_cast<unsigned int>( mS->getHeight( ) * xScale )
			, 	w = static_cast<unsigned int>( mS->getWidth( ) * xScale );


	double MinRe = -2.0
		, MaxRe = 1.0
		, MinIm = -1.2
		, MaxIm = MinIm + ( MaxRe - MinRe ) * h / w
		, Re_factor = ( MaxRe - MinRe ) / ( w - 1 )
		, Im_factor = ( MaxIm - MinIm ) / ( h - 1 );

	unsigned MaxIterations = 30;

	for ( unsigned y = 0; y < h; ++y )
	{
		double c_im = MaxIm - y * Im_factor;

		for ( unsigned x = 0; x < w; ++x )
		{
			double c_re = MinRe + x * Re_factor
					, Z_re = c_re, Z_im = c_im;

			bool isInside = true;

			for ( unsigned n = 0; n < MaxIterations; ++n )
			{
				double Z_re2 = Z_re*Z_re, Z_im2 = Z_im * Z_im;

				if ( Z_re2 + Z_im2 > 4 )
				{
					isInside = false;
					break;
				}

				Z_im = 2*Z_re*Z_im + c_im;

				Z_re = Z_re2 - Z_im2 + c_re;
			}

			if ( isInside )
			{
				int32_t realY = y, realX = x;
				if (realX < w && realY < h && realX >= 0 && realY >= 0)
				{
					(*(*mS)[ realY ][ realX ]) = xC;
				}
			}
		}
	}
}

} // namespace sisu
