#include "Variable.h"

namespace DDG
{
   Variable :: Variable( double value_,
                         bool fixed_ )
   // initialize a variable which has value zero and is not fixed by default
   : value( value_ ),
     fixed( fixed_ )
   {}

   Variable :: Variable( std::string name_,
                         double value_,
                         bool fixed_ )
   // initialize a named variable which has value zero and is not fixed by default
   : name( name_ ),
     value( value_ ),
     fixed( fixed_ )
   {}

   double& Variable :: operator*( void )
   // returns a reference to the numerical value
   {
      return value;
   }

   const double& Variable :: operator*( void ) const
   // returns a const reference to the numerical value
   {
      return value;
   }
}

