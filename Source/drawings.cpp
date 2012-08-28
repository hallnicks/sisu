#include "drawings.hpp"

namespace sisu
{

Drawings::Drawings( Screen * xScreen ) : mS( xScreen ) { }

Drawings::~Drawings( ) { }

void Drawings::clear( TTYC const xC )
{
	unsigned int const w = mS->getWidth( );

	for ( unsigned int y = 0; y < mS->getHeight( ); ++y )
	{
		hline( y, 0, w, xC );
	}
}

void Drawings::hline( unsigned int const xY, unsigned int const xStart, unsigned int const xWidth, TTYC const xC )
{
	unsigned int const width = mS->getWidth( );

	unsigned int const bound = std::min( xWidth, width );

	for ( unsigned int i = xStart; i < bound; ++i )
	{
		setPixel( i, xY, xC );
	}
}

#if 0
inline static double getSlope2D( Point * const xy0, Point * const xy1 )
{
	return static_cast<double>( xy1->Y  - xy0->Y ) / static_cast<double>( xy1->X - xy0->X );
}

inline static double getPointLine2D_X( double const xM, double const xY, double const xB = 0 )
{
	return ( xY - xB ) / xM;
}

void Drawings::hquad( Quad * const xQ, TTYC const xC )
{
	double const m0 = getSlope2D( &xQ->X, &xQ->Y );

	double const m1 = getSlope2D( &xQ->Z, &xQ->W );

}
#endif

void Drawings::setPixel( unsigned int const xX, unsigned int const xY, TTYC const xC )
{
	if ( xX < mS->getWidth( ) && xY < mS->getHeight( ) )
	{
		(*(*mS)[ xY ][ xX ]) = xC;
	}
}

// From wikipedia
void Drawings::midpointCircle( unsigned int const xX, unsigned int const xY, unsigned int const xRadius, TTYC const xColor )
{
	int f = 1 - xRadius;
	int ddF_x = 1;
	int ddF_y = -2 * xRadius;
	int x = 0;
	int y = xRadius;

	setPixel( xX, xY + xRadius, xColor );
	setPixel( xX, xY - xRadius, xColor );
	setPixel( xX + xRadius, xY, xColor );
	setPixel( xX - xRadius, xY, xColor );

	while ( x < y )
	{
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - radius*radius + 2*x - y + 1;

		if ( f >= 0 )
    		{
      			y--;
      			ddF_y += 2;
      			f += ddF_y;
    		}

		x++;
		ddF_x += 2;
		f += ddF_x;

		setPixel( xX + x, xY + y, xColor );
		setPixel( xX - x, xY + y, xColor );
		setPixel( xX + x, xY - y, xColor );
		setPixel( xX - x, xY - y, xColor );
		setPixel( xX + y, xY + x, xColor );
		setPixel( xX - y, xY + x, xColor );
		setPixel( xX + y, xY - x, xColor );
		setPixel( xX - y, xY - x, xColor );
	}
}

void Drawings::mandlebrot( TTYC xC, double xScale )
{
	unsigned int const h = static_cast<unsigned int>( mS->getHeight( ) * xScale )
			,  w = static_cast<unsigned int>( mS->getWidth( ) * xScale );


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
				double Z_re2 = Z_re * Z_re, Z_im2 = Z_im * Z_im;

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
