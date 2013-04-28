#include <iostream>
using namespace std;

#include "LinearPolynomial.h"
#include "Types.h"

namespace DDG
{
   LinearPolynomial :: LinearPolynomial( void )
   : constantTerm( 0. )
   {}

   LinearPolynomial :: LinearPolynomial( double c )
   {
      *this = c;
   }
   
   LinearPolynomial :: LinearPolynomial( Variable& v )
   {
      *this = v;
   }
   
   const LinearPolynomial& LinearPolynomial :: operator=( double c )
   {
      linearTerms.clear();
   
      constantTerm = c;
   
      return *this;
   }

   const LinearPolynomial& LinearPolynomial :: operator=( Variable& v )
   {
      linearTerms.clear();

      linearTerms[ &v ] = 1.;

      constantTerm = 0.;

      return *this;
   }

   void LinearPolynomial::operator+=( double c )
   {
      constantTerm += c;
   }
   
   void LinearPolynomial::operator-=( double c )
   {
      constantTerm -= c;
   }
   
   void LinearPolynomial::operator*=( double c )
   {
      for( TermIter i  = linearTerms.begin();
                    i != linearTerms.end();
                    i ++ )
      {
         i->second *= c;
      }
   
      constantTerm *= c;
   }
   
   void LinearPolynomial::operator/=( double c )
   {
      for( TermIter i  = linearTerms.begin();
                    i != linearTerms.end();
                    i ++ )
      {
         i->second /= c;
      }
   
      constantTerm /= c;
   }

   void LinearPolynomial::operator+=( Variable& v )
   {
      LinearPolynomial p( v );

      *this += p;
   }

   void LinearPolynomial::operator-=( Variable& v )
   {
      LinearPolynomial p( v );

      *this -= p;
   }

   void LinearPolynomial::operator+=( const LinearPolynomial& e )
   {
      for( TermCIter i  = e.linearTerms.begin();
                     i != e.linearTerms.end();
                     i++ )
      {
         TermIter j = linearTerms.find( i->first );
   
         if( j != linearTerms.end() )
         {
            j->second += i->second;
         }
         else
         {
            linearTerms[i->first] = i->second;
         }
      }
   
      constantTerm += e.constantTerm;
   }

   void LinearPolynomial::operator-=( const LinearPolynomial& e )
   {
      for( TermCIter i  = e.linearTerms.begin();
                     i != e.linearTerms.end();
                     i++ )
      {
         TermIter j = linearTerms.find( i->first );
   
         if( j != linearTerms.end() )
         {
            j->second -= i->second;
         }
         else
         {
            linearTerms[i->first] = -i->second;
         }
      }
   
      constantTerm -= e.constantTerm;
   }
   
   LinearPolynomial LinearPolynomial::operator-( void ) const
   {
      LinearPolynomial p = *this;

      for( TermIter i  = p.linearTerms.begin();
                    i != p.linearTerms.end();
                    i ++ )
      {
         i->second = -i->second;
      }

      p.constantTerm = -p.constantTerm;

      return p;
   }

   double LinearPolynomial::evaluate( void ) const
   {
      double value = constantTerm;

      for( TermCIter i  = linearTerms.begin();
                     i != linearTerms.end();
                     i ++ )
      {
         value += i->second * i->first->value;
      }

      return value;
   }

   LinearPolynomial operator+( double c,
                               Variable& v )
   {
      return c + LinearPolynomial(v);
   }

   LinearPolynomial operator+( Variable& v,
                               double c )
   {
      return LinearPolynomial(v) + c;
   }

   LinearPolynomial operator-( double c,
                               Variable& v )
   {
      return c - LinearPolynomial(v);
   }

   LinearPolynomial operator-( Variable& v,
                               double c )
   {
      return LinearPolynomial(v) - c;
   }

   LinearPolynomial operator*( double c,
                               Variable& v )
   {
      return LinearPolynomial(v) * c;
   }

   LinearPolynomial operator*( Variable& v,
                               double c )
   {
      return LinearPolynomial(v) * c;
   }

   LinearPolynomial operator/( Variable& v,
                               double c )
   {
      return LinearPolynomial(v) / c;
   }


   LinearPolynomial operator+( Variable& v1,
                               Variable& v2 )
   {
      return LinearPolynomial(v1) + LinearPolynomial(v2);
   }

   LinearPolynomial operator-( Variable& v1,
                               Variable& v2 )
   {
      return LinearPolynomial(v1) - LinearPolynomial(v2);
   }


   LinearPolynomial operator+( const LinearPolynomial& p,
                               double c )
   {
      LinearPolynomial sum = p;
   
      sum += c;
   
      return sum;
   }
   
   LinearPolynomial operator+( double c,
                               const LinearPolynomial& p )
   {
      LinearPolynomial sum = p;
   
      sum += c;
   
      return sum;
   }
   
   LinearPolynomial operator-( const LinearPolynomial& p,
                               double c )
   {
      LinearPolynomial difference = p;
   
      difference -= c;
   
      return difference;
   }

   LinearPolynomial operator-( double c,
                               const LinearPolynomial& p )
   {
      LinearPolynomial difference = -p;
   
      difference += c;
   
      return difference;
   }
   
   LinearPolynomial operator*( const LinearPolynomial& p,
                               double c )
   {
      LinearPolynomial product = p;
   
      product *= c;
   
      return product;
   }

   LinearPolynomial operator*( double c,
                               const LinearPolynomial& p )
   {
      LinearPolynomial product = p;
   
      product *= c;
   
      return product;
   }
   
   LinearPolynomial operator/( const LinearPolynomial& p,
                               double c )
   {
      LinearPolynomial quotient = p;

      quotient /= c;
   
      return quotient;
   }

   LinearPolynomial operator+( const LinearPolynomial& p,
                               Variable& v )
   {
      return p + LinearPolynomial(v);
   }

   LinearPolynomial operator+( Variable& v,
                               const LinearPolynomial& p )
   {
      return LinearPolynomial(v) + p;
   }

   LinearPolynomial operator-( const LinearPolynomial& p,
                               Variable& v )
   {
      return p - LinearPolynomial(v);
   }

   LinearPolynomial operator-( Variable& v,
                               const LinearPolynomial& p )
   {
      return LinearPolynomial(v) - p;
   }

   LinearPolynomial operator+( const LinearPolynomial& p,
                               const LinearPolynomial& q )
   {
      LinearPolynomial sum = p;
   
      sum += q;
   
      return sum;
   }
   
   LinearPolynomial operator-( const LinearPolynomial& p,
                               const LinearPolynomial& q )
   {
      LinearPolynomial difference = p;
   
      difference -= q;
   
      return difference;
   }

   ostream& operator<<( ostream& os, const LinearPolynomial& p )
   {
      for( TermCIter i  = p.linearTerms.begin();
                     i != p.linearTerms.end();
                     i ++ )
      {
         os << i->second << "*" << i->first->name << " + ";
      }

      os << p.constantTerm;

      return os;
   }
}

