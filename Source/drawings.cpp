#include "drawings.hpp"

#include <limits>
#include <cstdlib>
#include <iostream>

namespace sisu
{

ThoughtBox::ThoughtBox( char const * const xMessage
			, uint8_t const xX
                        , uint8_t const xY
                        , uint8_t const xW
                        , uint8_t const xH
                        , TTYC const xBG
                        , TTYC const xFG )
	: Message( xMessage )
	, X( xX )
	, Y( xY )
	, W( xW )
	, H( xH )
	, BG( xBG )
	, FG( xFG )
{
	;
}

Changed::Changed( Screen * const xS )
	: mMax( -1 )
	, mMagnitude( xS->getWidth( ) * xS->getHeight( ) )
	, mBound( mMagnitude - 1 )
	, mPoints( new Point2DOU [ mMagnitude ] )
{
	;
}

Changed::~Changed( )
{
	delete [] mPoints;
}

Point2DOU const & Changed::operator [ ] ( uint8_t const xIndex ) const
{
	return mPoints [ xIndex ];
}

void Changed::operator ( ) ( uint8_t const xX, uint8_t const xY )
{
	uint8_t const param[] = { xX, xY };

	mPoints [ mMax >= mBound ? mMax = 0 : ++mMax ] = Point2DOU( param );
}

void Changed::reset( )
{
	mMax = -1;
}

uint8_t Changed::max() const
{
	return mMax + 1;
}

Drawings::Drawings( Screen * const xScreen, eDrawingMode const xDrawMode )
	: mS( xScreen )
	, mMode( xDrawMode )
	, mCh( xDrawMode == eOnlyChanged ? new Changed( xScreen ) : NULL )
{
	if ( mS == NULL )
	{
		std::cerr << "Drawings object " << this <<  " received null screen." << std::endl;

		exit( -1 );
	}
}

Drawings::~Drawings( )
{
	;
}

void Drawings::clear( TTYC const xC )
{
	uint8_t const w = mS->getWidth( );

	for ( uint8_t y = 0; y < mS->getHeight( ); ++y )
	{
		hline( y, 0, w, xC );
	}
}

void Drawings::hline( uint8_t const xY, uint8_t const xStart, uint8_t const xWidth, TTYC const xC )
{
	uint8_t const width = mS->getWidth( ), bound = std::min( xWidth, width );

	for ( unsigned int i = xStart; i < bound; ++i )
	{
		setPixel( i, xY, xC );
	}
}

void Drawings::setPixel( uint8_t const xX, uint8_t const xY, TTYC const xC )
{
	if ( xX < mS->getWidth( ) && xY < mS->getHeight( ) )
	{
		class SetPixelAction
		{
			uint8_t mX, mY;

			TTYC mC;

			Screen * mS;

			Changed * mCH;

			public:
		                SetPixelAction( Screen * const xS
						, Changed * const xCH
						, TTYC const xC
						, uint8_t const xX
						, uint8_t const xY )
					: mX( xX )
					, mY( xY )
					, mC( xC )
					, mS( xS )
					, mCH( xCH )
				{
					;
				}

				void drawImmediately( ) { mS->setPixel( mX, mY, mC ); }

				void drawVsync( ) { ( * ( * mS )[ mY ][ mX ] ) = mC; }

				void drawChanged( )
				{
					drawVsync( );

					( * mCH )( mX, mY );
				}

		} actor( mS, mCh, xC, xX, xY );

		typedef void (SetPixelAction::*DrawingAction)(void);

		static DrawingAction const actions[] = { &SetPixelAction::drawImmediately
							, &SetPixelAction::drawVsync
							, &SetPixelAction::drawChanged };

		( actor.* ( actions [ mMode ] ) )( );
	}
}

// From wikipedia
void Drawings::midpointCircle( uint8_t const xX, uint8_t const xY, uint8_t const xRadius, TTYC const xColor )
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
	uint8_t const h = static_cast<uint8_t>( mS->getHeight( ) * xScale )
			,  w = static_cast<uint8_t>( mS->getWidth( ) * xScale );

	double MinRe = -2.0
		, MaxRe = 1.0
		, MinIm = -1.2
		, MaxIm = MinIm + ( MaxRe - MinRe ) * h / w
		, Re_factor = ( MaxRe - MinRe ) / ( w - 1 )
		, Im_factor = ( MaxIm - MinIm ) / ( h - 1 );

	static uint8_t const MaxIterations = std::numeric_limits<uint8_t>::max();

	for ( uint8_t y = 0; y < h; ++y )
	{
		double const c_im = MaxIm - y * Im_factor;

		for ( uint8_t x = 0; x < w; ++x )
		{
			double const c_re = MinRe + x * Re_factor;

			double Z_re = c_re, Z_im = c_im;

			bool isInside = true;

			for ( uint8_t n = 0; n < MaxIterations; ++n )
			{
				double const Z_re2 = Z_re * Z_re, Z_im2 = Z_im * Z_im;

				if ( Z_re2 + Z_im2 > 4 )
				{
					isInside = false;

					break;
				}

				Z_im = 2 * Z_re * Z_im + c_im;

				Z_re = Z_re2 - Z_im2 + c_re;
			}

			if ( isInside && y < w && y < h && x >= 0 && y >= 0 )
			{
				setPixel( x, y, xC );
			}
		}
	}
}

void Drawings::thoughtBox( ThoughtBox const & xTB )
{
	for ( uint8_t ii = 0; ii < xTB.H; ++ii )
	{
		hline( ii, xTB.X, xTB.W, xTB.BG );
	}
}

void Drawings::refresh( )
{
	class SetPixelAction
	{
		Screen * mS;
		Changed * mCh;

		public:
			SetPixelAction( Screen * const xS, Changed * const xCH )
				: mS( xS ), mCh( xCH )
		{
			;
		}

		void drawImmediately( ) { ; }
		void drawVsync( ) { mS->refresh( ); }
		void drawChanged( )
		{
			for ( uint8_t ii = 0; ii < mCh->max( ); ++ii )
			{
				Point2DOU const & p = ( * mCh )[ ii ];

				mS->refreshPosition( p[ 0 ], p[ 1 ] );
			}

			mCh->reset( );
		}

	} actor( mS, mCh );

	typedef void (SetPixelAction::*DrawingAction)(void);

	static DrawingAction const actions[] = { &SetPixelAction::drawImmediately
						, &SetPixelAction::drawVsync
						, &SetPixelAction::drawChanged };

	( actor.* ( actions [ mMode ] ) )( );
}

} // namespace sisu
