#include <map>
using namespace std;

#include <SuiteSparseQR.hpp>

#include "LinearSystem.h"
#include "LinearContext.h"
#include "Types.h"

namespace DDG
{
   extern LinearContext context;

   void LinearSystem::clear( void )
   // removes all equations from the system
   {
      equations.clear();
   }

   void LinearSystem::push_back( const LinearEquation& e )
   // appends the equation e to the sytem
   {
      equations.push_back( e );
   }

   void LinearSystem::solve( void )
   // solves the system and automatically stores the result in the variables
   // for an overdetermined system, computes a least-squares solution
   {
      convertEquations();
      indexVariables();
      buildSparseMatrix();
      buildRightHandSide();
      computeSolution();
   }

   void LinearSystem::convertEquations( void )
   // converts each equation to its polynomial representation
   {
      currentEquations.clear();

      for( EqnIter eqn  = equations.begin();
                   eqn != equations.end();
                   eqn ++ )
      {
         // move right-hand side to left-hand side
         LinearPolynomial p = eqn->lhs - eqn->rhs;

         // convert fixed variables to constants
         LinearPolynomial q( p.constantTerm );
         
         for( TermIter t  = p.linearTerms.begin();
                       t != p.linearTerms.end();
                       t ++ )
         {
            const double& coefficient( t->second );
            Variable& variable( *(t->first) );

            // skip zeros
            if( coefficient == 0. ) continue;

            if( t->first->fixed )
            {
               q += coefficient * variable.value;
            }
            else
            {
               q += coefficient * variable;
            }
         }

         if( q.linearTerms.size() > 0 )
         {
            currentEquations.push_back( q );
         }
      }

      nEquations = currentEquations.size();
   }

   void LinearSystem::indexVariables( void )
   // assign a unique index to each variable remaining in one of the polynomials
   {
      index.clear();
      nVariables = 0;

      for( PolyCIter p  = currentEquations.begin();
                     p != currentEquations.end();
                     p ++ )
      {
         for( TermCIter t  = p->linearTerms.begin();
                        t != p->linearTerms.end();
                        t ++ )
         {
            IndexIter j = index.find( t->first );

            // if we haven't seen this variable
            // before, assign it a unique index
            if( j == index.end() )
            {
               index[ t->first ] = nVariables;
               nVariables++;
            }
         }
      }
   }

   void LinearSystem::buildSparseMatrix( void )
   // build the sparse matrix representation of our current system
   {
      A = SparseMatrix<Real>( nEquations, nVariables );

      for( int i = 0; i < nEquations; i++ )
      {
         for( TermCIter t  = currentEquations[i].linearTerms.begin();
                        t != currentEquations[i].linearTerms.end();
                        t ++ )
         {
            int j = index[ t->first ];

            A(i,j) = t->second;
         }
      }
   }

   void LinearSystem::buildRightHandSide( void )
   // build the data vector for our current system
   {
      b = DenseMatrix<Real>( nEquations, 1 );

      for( int i = 0; i < nEquations; i++ )
      {
         b(i) = -currentEquations[i].constantTerm;
      }
   }

   void LinearSystem::computeSolution( void )
   {
      // solve linear system Ax=b
      x = SuiteSparseQR<double>( A.to_cholmod(), b.to_cholmod(), context );
      
      // put solution values in variables
      for( IndexIter i  = index.begin();
                     i != index.end();
                     i ++ )
      {
         i->first->value = x( i->second );
      }
   }
}

