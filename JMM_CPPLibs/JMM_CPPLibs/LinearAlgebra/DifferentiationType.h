// Copyright 2017 Jean-Marie Mirebeau, University Paris-Sud, CNRS, University Paris-Saclay
// Distributed WITHOUT ANY WARRANTY. Licensed under the Apache License, Version 2.0, see http://www.apache.org/licenses/LICENSE-2.0

#ifndef OptimalTransport_DifferentiationType_h
#define OptimalTransport_DifferentiationType_h

// A class for automatic differentiation

#include "FriendOperators.h"
#include "math.h"
#include <cassert>

namespace LinearAlgebra {

template<typename TDifferentiation, typename TScalar> struct DifferentiationTypeOperators;

template<typename TScalar, typename TVector>
struct DifferentiationType :
DifferentiationTypeOperators<DifferentiationType<TScalar,TVector>, TScalar >
{
    typedef TScalar ScalarType;
    typedef TVector VectorType;
    typedef DifferentiationType DT;
    
    ScalarType s;
    VectorType v;
    
    DifferentiationType(){};
    DifferentiationType(ScalarType t, VectorType w):s(t),v(w){};
//    explicit DifferentiationType(ScalarType t):s(t){v.fill(ScalarType(0.));}
    template<typename TBaseScalar> explicit DifferentiationType(TBaseScalar t):
    s(t){v.fill(ScalarType(0.));}
    explicit DifferentiationType(ScalarType t, size_t i):s(t){v.fill(ScalarType(0.)); assert(i<v.size()); v[i]=ScalarType(1.);}
    
    const DT & operator *= (ScalarType t) {s*=t; v*=t; return *this;}
    const DT &  operator /= (ScalarType t) {s/=t; v/=t; return *this;}
    const DT &  operator += (ScalarType t) {s+=t; return *this;}
    const DT &  operator -= (ScalarType t) {s-=t; return *this;}
    bool operator < (ScalarType t) const {return s<t;}
	bool operator > (ScalarType t) const {return s>t;}

    const DT &  operator *= (const DT & y) {v= v*y.s + y.v*s; s*=y.s; return *this;}
    const DT &  operator /= (const DT & y) {v= v/y.s - y.v*s/(y.s*y.s); s/=y.s; return *this;}
    const DT &  operator += (const DT & y) {s+=y.s; v+=y.v; return *this;}
    const DT &  operator -= (const DT & y) {s-=y.s; v-=y.v; return *this;}
    
    friend DT operator / (ScalarType s, const DT & y){return DT(s)/y;}
    DT operator -() const {return DT(-s,-v);}

    // Note: only base scalars are compared.
    bool operator < (const DT & y) const  {return s<y.s;}
    bool operator == (const DT & y) const {return s==y.s;}
    
    friend DT min(const DT & x, ScalarType s){return x.s < s ? x : DT(s);}
    friend DT sqrt(const DT & x){const ScalarType ss = sqrt(x.s); return ss>ScalarType(0) ? DT(ss,x.v/(2.*ss)) : DT(0.);};
    friend DT fabs(const DT & x){return x.s>=ScalarType(0) ? x : DT(-x.s,-x.v);}
    friend DT pow(const DT &x, ScalarType p){const ScalarType xp = pow(x.s,p-1); return DT(xp*x.s, p*xp*x.v);}
    friend DT cos(const DT & x){return DT(cos(x.s),-sin(x.s)*x.v);}
    friend DT sin(const DT & x){return DT(sin(x.s), cos(x.s)*x.v);}
    
    //friend std::ostream & operator << (std::ostream & os, const ScalarType & y){return os << "{" << y.s << "," << y.v << "}";}
};

template<typename TS,typename TV>
std::ostream & operator << (std::ostream & os, const DifferentiationType<TS,TV> & a){
	std::cout << "{" << a.s << "," << a.v << "}";
	return os;
}
	

/*
A natural implementation would inherit from the following classes.
However, this results in some compilers (e.g. VS2017) allocating more space than needed
in order to disambiguate between the empty classes.

vector_space< DifferentiationType<TScalar,TVector>, TScalar>,
offsettable<DifferentiationType<TScalar,TVector>, TScalar>,
totally_ordered<DifferentiationType<TScalar, TVector> >,
totally_ordered2<DifferentiationType<TScalar, TVector>, TScalar>,
equality_comparable<DifferentiationType<TScalar, TVector> >,
multiplicative<DifferentiationType<TScalar, TVector> >

We thus use the more verbose implementation below
*/

template<typename TDifferentiation, typename TScalar> struct DifferentiationTypeOperators {
	using AD = TDifferentiation;
	using K = TScalar;
	friend AD operator + (AD a, AD const & b) { a += b; return a; }
	friend AD operator + (AD a, K const & b) { a += b; return a; }
	friend AD operator + (K const & a, AD b) { b += a; return b; }

	friend AD operator - (AD a, AD const & b) { a -= b; return a; }
	friend AD operator - (AD a, K const & b) { a -= b; return a; }
	friend AD operator - (K const & a, AD b) { b -= a; return -b; }

	friend AD operator * (AD a, AD const & b) { a *= b; return a; }
	friend AD operator * (AD a, K const & b) { a *= b; return a; }
	friend AD operator * (K const & a, AD b) { b *= a; return b; }

	// Totally ordered
	friend bool operator > (AD const & a, AD const & b) { return b < a; }
	friend bool operator >= (AD const & a, AD const & b) { return !(a < b); }
	friend bool operator <= (AD const & a, AD const & b) { return !(a > b); }

	// Totally ordered 2
	friend bool operator <= (const AD & a, const K & b) { return !(a > b); }
	friend bool operator >= (const AD & a, const K & b) { return !(a < b); }

	friend bool operator <  (const K & b, const AD & a) { return a > b; }
	friend bool operator >  (const K & b, const AD & a) { return a < b; }
	friend bool operator <= (const K & b, const AD & a) { return a >= b; }
	friend bool operator >= (const K & b, const AD & a) { return a <= b; }

	friend bool operator != (const AD &a, const AD &b) { return !(a == b); }
};


} // namespace
#endif
