#include "tdd.hpp"
#include <iostream>

namespace
{

class MyTDD : public TinyTDD::TDDBase
{
	public:
		MyTDD( ) { }
		virtual ~MyTDD( ) { }
		virtual void Setup( ) { }
		virtual void TearDown( ) { }
};

} // namespace

DECL_TDD(MyTDD, HelloWorld,
{
	std::cout << "Hello, World!" << __FUNCTION__ << std::endl;
});
