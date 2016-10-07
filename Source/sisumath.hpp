#ifndef SISUMATH_8AD50D39A6704E1587D5CAD907B48B35_HPP_
#define SISUMATH_8AD50D39A6704E1587D5CAD907B48B35_HPP_

namespace sisu {

#include <SDL2/SDL.h>

inline static double dRand( double const xMin, double const xMax )
{
	double const roll = ( double )rand( ) / RAND_MAX;
	return xMin + roll * ( xMax - xMin );
}

inline static float generateSignal( float const xHerz )
{
	static uint32_t const sFreq = 8000;

	const float generatorConstant1   = cosf( 2 * M_PI * ( xHerz / sFreq ) );
	const float generatorConstant2   = sinf( 2 * M_PI * ( xHerz / sFreq ) );

	static float Register[2]={1,0};
	static float FeedBack;

	FeedBack=2*generatorConstant1*Register[0]-Register[1];
	Register[1]=Register[0];
	Register[0]=FeedBack;

	return (generatorConstant2*Register[1]);
}

inline static uint8_t floatToByte( float const xFloat )
{
	if ( xFloat > 1.0 )
	{
		return 255;
	}

	if ( xFloat < 0.0 )
	{
		return 0;
	}

	return (uint8_t)( xFloat * 255.0f );
}

} // namespace sisuu
#endif // SISUMATH_8AD50D39A6704E1587D5CAD907B48B35_HPP_

