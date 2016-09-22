#ifndef RECT_134CE725B3B1498585DC84AADB266CFC_HPP_
#define RECT_134CE725B3B1498585DC84AADB266CFC_HPP_

namespace sisu {
	struct Rect 
	{ 
		Rect( )
			: x(0)
			, y(0)
			, w(0)
			, h(0)
		{
			;
		}

		Rect( GLfloat const xX
		    , GLfloat const xY
		    , GLfloat const xW
		    , GLfloat const xH )
			: x(xX)
			, y(xY)
			, w(xW)
			, h(xH)
		{
			;
		}

		GLfloat x, y, w, h; 
	};
} // namespace sisu

#endif // RECT_134CE725B3B1498585DC84AADB266CFC_HPP_
