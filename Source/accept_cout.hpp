#ifndef ACCEPT_COUT_65B9F952_C195_4874_B1BE_799700402726_HPP_
#define ACCEPT_COUT_65B9F952_C195_4874_B1BE_799700402726_HPP_

namespace sisu
{

template < typename XReturnType, typename XCharType = char >
class accept_cout
{
	XReturnType & mThis;

	protected:
		accept_cout( )
			: mThis ( static_cast< XReturnType & > ( *this ) )
				{ ; }

	public:
		virtual ~accept_cout( ) { ; }

		virtual std::ostream & operator >> ( std::ostream & xS ) const = 0;

		typedef std::basic_ostream< XCharType, std::char_traits< XCharType > > cout_t;

                typedef cout_t & ( * endl_t )( cout_t & );

                XReturnType & operator << ( endl_t xS )
			{ return ( * this ) << xS; }

		template < typename T >
		XReturnType & operator << ( T const & xS )
		{
			std::cout << xS;
			return mThis;
		}
};

} // namespace sisu

#endif // COUTABLE_65B9F952_C195_4874_B1BE_799700402726_HPP_
