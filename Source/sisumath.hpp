#ifndef SISUMATH_8AD50D39A6704E1587D5CAD907B48B35_HPP_
#define SISUMATH_8AD50D39A6704E1587D5CAD907B48B35_HPP_

namespace sisu {

inline static double dRand( double const xMin, double const xMax )
{
	double const roll = ( double )rand( ) / RAND_MAX;
	return xMin + roll * ( xMax - xMin );
}

} // namespace sisuu
#endif // SISUMATH_8AD50D39A6704E1587D5CAD907B48B35_HPP_

