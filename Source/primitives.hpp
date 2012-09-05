#ifndef PRIMITIVES_7F88BF4D_1085_42EE_A26B_50A78925093A_HPP_
#define PRIMITIVES_7F88BF4D_1085_42EE_A26B_50A78925093A_HPP_

#include <stdint.h>

namespace sisu
{
	template< typename XType, typename XPrecision, XPrecision XDimensions >
	class Point
	{
                XType mData [ XDimensions ];

		public:
			Point( ) : mData( )
			{
				for ( XPrecision i = 0; i < XDimensions; ++i )
				{
					mData [ i ] = 0;
				}
			}

        	        Point( XType const ( & xSourceMemory ) [ XDimensions ] ) : mData( )
			{
				SetPoints( xSourceMemory );
			}

			Point( XType * const xBuffer ) : mData( )
			{
				SetPoints( xBuffer );
			}

        	        Point( Point const & xRhs ) : mData( )
			{
				SetPoints( xRhs );
			}

                	void SetPoints( XType const ( & xSourceMemory ) [ XDimensions ] )
			{
				for ( XPrecision i = 0; i < XDimensions; ++i )
				{
					mData [ i ] = xSourceMemory [ i ];
				}
			}

			void SetPoints( XType * const xBuffer )
			{
				for ( XPrecision i = 0; i < XDimensions; ++i )
				{
					mData [ i ] = xBuffer [ i ];
				}
			}

        	        void SetPoints( Point const & xRhs )
			{
				for ( XPrecision i = 0; i < XDimensions; ++i )
				{
					mData [ i ] = xRhs [ i ];
				}
			}

			XType operator [ ] ( XPrecision const xIdx ) const
			{
				return mData [ xIdx ];
			}

			// This point is less than (anticlockwise to) xLhs
			bool operator < ( Point const & xLhs ) const
			{
				// stub
			}

			// This point is greater than (clockwise to) xLhs
			bool operator > ( Point const & xLhs ) const
			{
				// stub
			}

			bool operator == ( Point const & xLhs ) const
			{
				bool are_equal = true;

				Point const & rhs = *this;

				for ( XPrecision i = 0; i < XDimensions; ++i )
				{
					if ( rhs [ i ] != xLhs [ i ] )
					{
						are_equal = false;

						break;
					}
				}

				return are_equal;
			}

			bool operator != ( Point const & xLhs ) const { return ! ( *this == xLhs ); }

			bool operator >= ( Point const & xLhs ) const { return ( *this > xLhs ) || ( *this == xLhs ); }

			bool operator <= ( Point const & xLhs ) const { return ( *this < xLhs ) || ( *this == xLhs ); }
        };

	typedef Point<uint8_t, uint8_t, 2> Point2DOU;

} // namespace sisu

#endif //  PRIMITIVES_7F88BF4D_1085_42EE_A26B_50A78925093A_HPP_
