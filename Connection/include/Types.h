// -----------------------------------------------------------------------------
// libDDG -- Types.h
// -----------------------------------------------------------------------------
//
// This file contains forward declarations of common types and definitions of
// convenience types for standard iterators.
//

#ifndef DDG_TYPES_H
#define DDG_TYPES_H

#include <cholmod.h>
#include <map>
#include <vector>

namespace DDG
{
   // forward declarations
   class Camera;
   class Complex;
   class Edge;
   class Face;
   class HalfEdge;
   class Image;
   class LinearContext;
   class LinearEquation;
   class LinearPolynomial;
   class LinearSystem;
   class Mesh;
   class MeshIO;
   class Quaternion;
   class Real;
   class Shader;
   class Variable;
   class Vector;
   class Vertex;
   class Viewer;

   template <class T>
   class DenseMatrix;

   template <class T>
   class SparseMatrix;
   
   // convenience types for iterators
   typedef std::map<Variable*,double>::iterator            TermIter;
   typedef std::map<Variable*,double>::const_iterator     TermCIter;
   typedef std::vector<LinearPolynomial>::iterator         PolyIter;
   typedef std::vector<LinearPolynomial>::const_iterator  PolyCIter;
   typedef std::vector<LinearEquation>::iterator            EqnIter;
   typedef std::vector<LinearEquation>::const_iterator     EqnCIter;
   typedef std::map<Variable*,int>::iterator              IndexIter;
   typedef std::map<Variable*,int>::const_iterator       IndexCIter;
   typedef std::vector<HalfEdge>::iterator             HalfEdgeIter;
   typedef std::vector<HalfEdge>::const_iterator      HalfEdgeCIter;
   typedef std::vector<Vertex>::iterator                 VertexIter;
   typedef std::vector<Vertex>::const_iterator          VertexCIter;
   typedef std::vector<Edge>::iterator                     EdgeIter;
   typedef std::vector<Edge>::const_iterator              EdgeCIter;
   typedef std::vector<Face>::iterator                     FaceIter;
   typedef std::vector<Face>::const_iterator              FaceCIter;
}

#endif
