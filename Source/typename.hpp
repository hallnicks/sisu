#ifndef TYPENAME_HPP_
#define TYPENAME_HPP_

#include <cxxabi.h>

#include <cstdlib>
#include <iostream>

namespace sisu
{

class typename_string
{
        int s;

        char * name;

        typename_string( const typename_string & );

        typename_string & operator=( const typename_string & );

        public:
                template< typename XType >
                inline typename_string( XType & xInstance )
                        : s( 0 ), name( abi::__cxa_demangle( typeid( xInstance ).name(), 0, 0, &s ) )
                {
                        if ( s != 0 )
                        {
                                std::cerr << "Demangle failed" << std::endl;

                                exit( -1 );
                        }
                }

                inline ~typename_string( ) { if ( name ) { free( name ); } }

                inline char * const getName() const { return name; }

                typedef std::basic_ostream< char, std::char_traits< char > > cout_t;

                typedef cout_t & ( * endl_t )( cout_t & );

                inline typename_string & operator << ( endl_t xStream ) { std::cout << xStream; return (*this); }
};

inline std::ostream & operator << ( std::ostream & xStream, typename_string const & xTypeString )
{
        return xStream << xTypeString.getName( );
}

} // namespace sisu

#endif // TYPENAME_HPP_
