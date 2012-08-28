#if 0 // Work In Progress

#ifndef PRIMITIVES_7F88BF4D_1085_42EE_A26B_50A78925093A_HPP_
#define PRIMITIVES_7F88BF4D_1085_42EE_A26B_50A78925093A_HPP_

namespace sisu
{
	enum eIdx { ePX = 0
		, epY = 1
		, epZ = 2
		, epW = 3 };

	struct Point
	{
                Point( double const xX
			, double const xY
			, double const xZ
			, double const xW );

                Point( Point const & xRhs );

                double data [ 4 ];

		double operator[ ]( eIdx xIdx ) { return data[ xIdx ]; }

		friend inline bool operator < ( Point const & xLhs, Point const & xRhs );
		friend inline bool operator > ( Point const & xLhs, Point const & xRhs );
		friend inline bool operator != ( Point const & xLhs, Point const & xRhs );
		friend inline bool operator == ( Point const & xLhs, Point const & xRhs );
		friend inline bool operator >= ( Point const & xLhs, Point const & xRhs );
		friend inline bool operator <= ( Point const & xLhs, Point const & xRhs );
        };

	inline bool operator == ( Point const & xLhs, Point const & xRhs )
	{
		return xLhs[ epX ] == xRhs[ epX ]
			&& xLhs[ epY ] == xRhs[ epY ]
			&& xLhs[ epZ ] == xRhs[ epZ ]
			&& xLhs[ epW ] == xRhs[ epW ];
	}

	inline bool operator != ( Point const & xLhs, Point const & xRhs )
	{
		return !( xRhs == xRhs );
	}

	inline bool operator < ( Point const & xLhs, Point const & xRhs )
	{
		return xLhs[ epX ] < xRhs[ epX ]
			&& xLhs[ epY ] < xRhs[ epY ]
			&& xLhs[ epZ ] < xRhs[ epZ ]
			&& xLhs[ epW ] < xRhs[ epW ];
	}

	inline bool operator > ( Point const & xLhs, Point const & xRhs )
	{
		return xRhs < xLhs;
	}

	inline bool operator >= ( Point const & xLhs, Point const & xRhs )
	{
		return xLhs == xRhs || xLhs > xRhs;
	}

	inline bool operator <= ( Point const & xLhs, Point const & xRhs )
	{
		return xLhs == xRhs || xLhs < xRhs;
	}

        struct Triangle
        {
                Triangle( Point const & xX
                        , Point const & xY
                        , Point const & xZ );

                Triangle( Triangle const & xRhs );

                Point X;
                Point Y;
                Point Z;
        };

   	struct Quad
        {
                Quad( Point const & xX
                        , Point const & xY
                        , Point const & xZ
                        , Point const & xW );

		Quad( Quad const & xQ );

                Point X;
                Point Y;
                Point Z;
                Point W;
        };

        struct TriQuad
        {
                TriQuad( Triangle const & xA, Triangle const & xO );

                Triangle A;
                Triangle O;
        };

} // namespace sisu

#endif //  PRIMITIVES_7F88BF4D_1085_42EE_A26B_50A78925093A_HPP_

#endif // 0 - Work In Progress
