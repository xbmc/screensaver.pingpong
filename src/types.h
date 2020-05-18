/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2005 Joakim Eriksson <je@plane9.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <math.h>
#include <stdint.h>

/***************************** D E F I N E S *******************************/

typedef signed char      s8;
typedef unsigned char    u8;
typedef signed short    s16;
typedef unsigned short  u16;
typedef signed long     s32;
typedef unsigned long   u32;
typedef int64_t         s64;
typedef uint64_t        u64;
typedef float           f32;
typedef double          f64;
#define null 0

const f32 PI = 3.14159265358979323846f;
const f32 FLOATEPSILON = 0.00001f;

/****************************** M A C R O S ********************************/

#define SAFE_DELETE(_p)        { if(_p) { delete _p;    _p=NULL; } }
#define SAFE_DELETE_ARRAY(_p)  { if(_p) { delete [] _p;  _p=NULL; } }
#define SAFE_RELEASE(_p)       { if(_p) { _p->Release();  _p=NULL; } }

// Direct 3d verify
#define DVERIFY( _func ) \
  { \
    HRESULT _retCode = _func; \
    if( _retCode !=  D3D_OK) \
    { \
      char buf[2000]; \
      sprintf(buf, "\""#_func"\" returned 0x%lx in\n" __FILE__ "(%ld)\n", _retCode, __LINE__); \
      OutputDebugString(buf); \
      return false; \
    } \
  }

#define DEGTORAD(d)      ((d)*(PI / 180.0f))
#define RADTODEG(r)      ((r)*(180.0f / PI))
#define assert(_x)

/***************************** C L A S S E S *******************************/

////////////////////////////////////////////////////////////////////////////
//
class CRGBA
{
public:
  union
  {
    f32 col[4];
    struct
    {
      f32 r,g,b,a;
    };
  };

  CRGBA()
  {
    col[0] = col[1] = col[2] = 1.0f;
    col[3] = 1.0f;
  }
  CRGBA(f32 R, f32 G, f32 B, f32 A)
  {
    col[0]=R; col[1]=G; col[2]=B; col[3]=A;
  }

  void Set(f32 R, f32 G, f32 B, f32 A)
  {
    col[0]=R;
    col[1]=G;
    col[2]=B;
    col[3]=A;
  }

  CRGBA& operator = (const CRGBA& c)
  {
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
    return *this;
  }
};

////////////////////////////////////////////////////////////////////////////
//
class CVector
{
public:
  CVector() { }
  CVector(f32 _x, f32 _y, f32 _z)
  {
    x = _x;
    y = _y;
    z = _z;
  }

  void Zero()
  {
    x = y = z = 0.0f;
  }

  void Set(f32 _x, f32 _y, f32 _z)
  {
    x = _x;
    y = _y;
    z = _z;
  }

  f32 x;
  f32 y;
  f32 z;
};

/***************************** G L O B A L S *******************************/
/***************************** I N L I N E S *******************************/

inline f32 Clamp(f32 x, f32 min, f32 max) { return (x <= min ? min : (x >= max ? max : x)); }
inline f32 RandFloat(void) { return (1.0f / RAND_MAX) * ((f32)rand());  }
inline int ISEQUAL(f32 a,f32 b,f32 absprec) { return (fabs((a)-(b))<=absprec); }
inline int Rand(int max) { return rand() % max; }

////////////////////////////////////////////////////////////////////////////
//
inline f32 DotProduct(const CVector& v1, const CVector& v2)
{
  return v1.x*v2.x + v1.y * v2.y + v1.z*v2.z;
}
