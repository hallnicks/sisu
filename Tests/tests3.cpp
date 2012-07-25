#include "tdd.hpp"
#include <iostream>

namespace
{

class TestSet3 : public TinyTDD::TDDBase
{
	public:
		TestSet3( ) { }
		virtual ~TestSet3( ) { }
		virtual void Setup( ) { }
		virtual void TearDown( ) { }
};

} // namespace TinyTDD

DECL_TDD(TestSet3, HelloWorld,
{
	std::cout << "Hello, World!" << __FUNCTION__ << std::endl;
});

DECL_TDD(TestSet3, TestThrowException,
{
	throw std::exception();
});
