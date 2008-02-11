//
// C++ Implementation: Float4
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sse4.h"

// Float4::Float4(float x0, float x1, float x2, float x3) {}

const Float4 Float4::EPSILON4(_mm_set_ps1(EPSILON));
const Float4 Float4::EPSILON4_NEG( _mm_set_ps1(-EPSILON) );
const Float4 Float4::ONE = ( 1.0f );

Float4 one;
const Float4 Float4::BINONE(*((__m128*)memset(&one, 0xff, sizeof(__m128))));
