// -----------------------------------------------------------------------------
// libDDG -- DiscreteExteriorCalculus.h
// -----------------------------------------------------------------------------
//
// Static methods for building the fundamental discrete operators (exterior
// derivative, Hodge star) for 0-, 1-, and 2-forms on a surface mesh.  Methods
// are templated on entry type, i.e., one can build either real- or complex-
// matrices using the types DDG::Real and DDG::Complex, respectively.  For
// instance, to build the usual Laplacian on functions, one could write
//
//    Mesh mesh;
//    SparseMatrix d0, star0, star1, Delta;
//
//    ExteriorDerivative0Form::build( mesh, d0 );
//    HodgeStar0Form::build( mesh, star0 );
//    HodgeStar1Form::build( mesh, star1 );
//    Delta = star0.inverse() * d0.transpose() * star1 * d0;
// 

#ifndef DDG_DISCRETEEXTERIORCALCULUS_H
#define DDG_DISCRETEEXTERIORCALCULUS_H

#include "Mesh.h"
#include "SparseMatrix.h"

namespace DDG
{
   template< class T > struct HodgeStar0Form { static void build( const Mesh& mesh, SparseMatrix<T>& star0 ); };
   template< class T > struct HodgeStar1Form { static void build( const Mesh& mesh, SparseMatrix<T>& star1 ); };
   template< class T > struct HodgeStar2Form { static void build( const Mesh& mesh, SparseMatrix<T>& star2 ); };
   template< class T > struct ExteriorDerivative0Form { static void build( const Mesh& mesh, SparseMatrix<T>& d0 ); };
   template< class T > struct ExteriorDerivative1Form { static void build( const Mesh& mesh, SparseMatrix<T>& d1 ); };
}

#include "DiscreteExteriorCalculus.inl"

#endif
