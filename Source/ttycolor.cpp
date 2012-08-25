#include "ttycolor.hpp"

namespace sisu
{

eTTYColor const TTYCMap::colors[10] = { eTTYCNone
				    , eTTYCBlack
				    , eTTYCRed
				    , eTTYCGreen
				    , eTTYCYellow
				    , eTTYCBlue
				    , eTTYCMagenta
				    , eTTYCCyan
				    , eTTYCWhite
				    , eTTYCMax };

eTTYModifier const TTYCMap::modifiers[7] = { eModNone
			    	      	   , eModBold
			    	      	   , eModDim
			  	      	   , eModUnderline
			    	           , eModBlink
			    	      	   , eModReverse
			              	   , eModMax };


} // namespace sisu
