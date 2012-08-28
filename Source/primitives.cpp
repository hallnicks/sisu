#if 0 // Work In Progress
#include "primitives.hpp"

namespace sisu
{

Point::Point( double const xX
		, double const xY
		, double const xZ
		, double const xW )
			: X( xX )
			, Y( xY )
			, Z( xZ )
			, W( xW )
{
	;
}

Point::Point( Point const & xRhs )
	: X( xRhs.X )
	, Y( xRhs.Y )
	, Z( xRhs.Z )
	, W( xRhs.W )
{
	;
}

template< size_t XSize >
inline static void SortPoints( Point ( & xMemory[ XSize ] )
{

}

Quad::Quad( Point const & xX
	, Point const & xY
	, Point const & xZ
	, Point const & xW )
		: X( xX )
		, Y( xY )
		, Z( xZ )
		, W( xW )
{
	SortQuad( *this );
}

Quad::Quad( Quad const & xQ )
{
	SortQuad( *this );
}

Triangle::Triangle( Point const & xX
		, Point const & xY
		, Point const & xZ )
			: X( xX )
			, Y( xY )
			, Z( xZ )
{
	;
}

Triangle::Triangle( Triangle const & xRhs )
	: X( xRhs.X )
	, Y( xRhs.Y )
	, Z( xRhs.Z )
{
	;
}

TriQuad::TriQuad( Triangle const & xA, Triangle const & xO )
	: A( xA )
	, O( xO )
{
	;
}

} // namespace sisu

#endif // Work In Progress
