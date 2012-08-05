#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

namespace sisu
{

	// TODO - Add variadic class support.

	template< typename XReturnType, typename XEventHandler, typename XArgumentType >
	class signal
	{
		typedef XReturnType(XEventHandler::*XEventFunctionType)(XArgumentType);

		XEventFunctionType mMethod;

		XReturnType & mEventHandler;

		public:

			signal( XEventHandler & xEventHandler, XEventFunctionType xType )
				: mEventHandler( xEventHandler ), mMethod( xType )
			{
				;
			}

			virtual ~signal( )
			{
				;
			}
	};

} // namespace sisu

#endif // SIGNAL_HPP_
