#include "LinearContext.h"

namespace DDG
{
   // global context for linear solvers
   LinearContext context;

   LinearContext :: LinearContext( void )
   // constructor
   {
      cholmod_l_start( &context );
   }

   LinearContext :: ~LinearContext( void )
   // destructor
   {
      cholmod_l_finish( &context );
   }

   LinearContext :: operator cholmod_common*( void )
   // allows LinearContext to be treated as a cholmod_common*
   {
      return &context;
   }
}
