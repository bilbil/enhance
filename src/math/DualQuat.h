///@author Bill Liu
///Dual Quaternion port from Java (Antonio Sanchez)

#ifndef DUALQUAT_H
#define DUALQUAT_H

#include <stdexcept>

#include "Quat.h"
#include "Vec.h"
#include "DualScalar.h"

class DualQuat{
 public:

  //store quaternions: A (real = rotation) + eB (dual = translation)
  Quat              _A;		
  Quat              _B;

                    DualQuat(); 
                    DualQuat(Quat & a, Quat & b);
                    DualQuat(Vec A, float a, Vec B, float b);
                    DualQuat(DualQuat & a);

  void     	    SetIdentity(); // identity dual quaternion 1 + e0
  void              SetZero(); // dual quaternion 0 + e0
  void   	    SetQuats(Quat & a, Quat & b); // a + eb
  void  	    SetQuatsVecs(Vec A, float a, Vec B, float b); //A+a + e(B+b)
  void              SetArray(float a []);
  void              GetArray(float a []) const;

  DualQuat &        operator = ( const DualQuat & q ); //copy
  DualQuat          operator + ( const DualQuat & q ) const;
  DualQuat &        operator += ( const DualQuat & q );
  DualQuat          operator - ( const DualQuat & q ) const;
  DualQuat &        operator -= ( const DualQuat & q );
  DualQuat          operator * ( const DualQuat & q ) const; // non-transitive multiplication q1 * q2 = (q1._A*q2._A, q1._A*q2._B + q1._B*q2._A)
  DualQuat inline   Conjugate() const{ Quat a (_A.Conjugate()); Quat b (_B.Conjugate()); DualQuat q ( a, b); return q; }
  void inline       SetConjugate(){ _A = _A.Conjugate(); _B = _B.Conjugate(); }

  Quat              GetReal() const;
  Quat              GetDual() const;
  void              SetReal( const Quat & q );
  void              SetDual( const Quat & q );
  float             GetVal( int index ) const;

  DualScalar        NormSquared() const;
  DualScalar        Norm() const;

  DualQuat          Normalize() const; // normalize to ||mag|| = 1+ e*0

  DualQuat          Invert() const; //gets inverse where q^-1 * q = q * q^-1 = 1 + e0

  /* void              ScLerp( DualQuat & q1, DualQuat & q2, float t); */
  DualQuat          PowFloat(double e) const; ///Raises the supplied quaternion to the power e according to euler's formula, this applies to unit quaternions
  float             GetScrewParameters( Vec & screwaxix, Vec & moment, Vec & angles ); // returns norm of _A._quat's x,y,z 
  void              SetScrewParameters( Vec & screwaxis, Vec & moment, float theta, float alpha );

  class Exception : public std::runtime_error {
  public:
    Exception(const std::string &msg): std::runtime_error(msg) { }
  };
};

DualQuat          ScaleAddDualScalar( const DualScalar & s, const DualQuat & q1, const DualQuat & q2 ); // returns ( s.a*q1.a + q2.a, s.a*q1.b + q2.b + s.b * q1.a )
DualQuat          ScaleAddFloat( float s, const DualQuat & q1, const DualQuat & q2 ); // returns s*q1 + q2
DualQuat          ScaleDualScalar( const DualScalar & s, const DualQuat & q ); //( s.a*q1.a, s.a*q.b + s.b * q.a )
DualQuat          ScaleFloat( float s, const DualQuat & q ); //s*q
DualQuat          ScrewLinearInterpolate( const DualQuat & q1, const DualQuat & q2, float t ); ///q = q1*(q1^-1 q2)^t. For the shortest distance, q1 and q2 should have the same orientation.

#endif
