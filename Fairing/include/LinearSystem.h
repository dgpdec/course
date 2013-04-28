// -----------------------------------------------------------------------------
// libDDG -- LinearSystem.h
// -----------------------------------------------------------------------------
//
// LinearSystem represents a system of linear equations expressed in terms of
// instances of the Variable class.  The main idea is to make it easy to
// construct and solve linear systems without explicitly think about variable
// indices, matrix layout, etc.  (This kind of abstraction is particularly
// useful for debugging and rapid prototyping.)  See the documentation for
// examples of building linear and solving systems.
//
// Importantly, any variable used by a LinearSystem should not be deallocated
// while the system is still in use, because the method LinearSystem::solve()
// automatically copies the solution back into the variables used to define the
// equations.  (In the future variables may become reference-counted in order
// to avoid this issue.)
//
// Note that LinearSystem::solve() uses a general-purpose linear solver (namely
// the sparse QR factorization found in SuiteSparse) that is quite fast but may
// not always be your best option.  To improve performance you may want to
// build the system explicitly using an instance of SparseMatrix and call a
// more specialized solver.  (In the future there may be options for specifying
// that a LinearSystem is, e.g., symmetric and positive-definite.)
//

#ifndef DDG_LINEARSYSTEM_H
#define DDG_LINEARSYSTEM_H

#include <vector>
#include "LinearEquation.h"
#include "SparseMatrix.h"
#include "DenseMatrix.h"
#include "Real.h"

namespace DDG
{
   class LinearSystem
   {
      public:
         void clear( void );
         // removes all equations from the system

         void push_back( const LinearEquation& e );
         // appends the equation e to the sytem

         void solve( void );
         // solves the system and automatically stores the result in the variables
         // for an overdetermined system, computes a least-squares solution

         std::vector<LinearEquation> equations;
         // the collection of equations defining the system

      protected:
         void convertEquations( void );
         void indexVariables( void );
         void buildSparseMatrix( void );
         void buildRightHandSide( void );
         void computeSolution( void );

         int nEquations;
         int nVariables;
         std::vector<LinearPolynomial> currentEquations;
         std::map<Variable*,int> index;
         SparseMatrix<Real> A;
          DenseMatrix<Real> x;
          DenseMatrix<Real> b;
   };
}

#endif
