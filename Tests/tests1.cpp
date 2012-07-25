#include "tdd.hpp"
#include <iostream>

namespace
{

class TinyTDD_TestSet1 : public ::TinyTDD::TDDBase
{

	public:
		TinyTDD_TestSet1( ) { }
		virtual ~TinyTDD_TestSet1( ) { }
		virtual void Setup( ) { }
		virtual void TearDown( ) { }
};

} // namespace

DECL_TDD(TinyTDD_TestSet1, HelloWorld,
{
	std::cout << "Hello, World!" << __FUNCTION__ << std::endl;
});

