#ifndef OSCILLATOR_CFED845F9FD04B3CB12E223B4199A07B_HPP_
#define OSCILLATOR_CFED845F9FD04B3CB12E223B4199A07B_HPP_

namespace sisu {

template< typename XType >
class Oscillator
{
	XType mValue
	    , mMin
	    , mMax;

	XType const mStep;

	bool mReverse;

	public:
		Oscillator( XType const xInitial
			  , XType const xMin
			  , XType const xMax
			  , XType const xStep )
			: mValue( xInitial )
			, mMin( xMin )
			, mMax( xMax )
			, mStep( xStep )
			, mReverse( false )
		{
			;
		}

		void setMinMax( XType const xNewMin, XType const xNewMax )
		{
			mMin = xNewMin;
			mMax = xNewMax;
		}

		XType operator++( )
		{
			if ( !mReverse )
			{
				mValue -= mStep;
			}
			else
			{
			 	mValue += mStep;
			}

			if ( mValue <= mMin )
			{
				mReverse = true;
				mValue = mMin;
			}
			else if ( mValue >= mMax )
			{
				mReverse = false;
			}

			return mValue;
		}
};

} // namespace sisu

#endif // OSCILLATOR_CFED845F9FD04B3CB12E223B4199A07B_HPP_
