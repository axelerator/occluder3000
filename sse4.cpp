//
// C++ Implementation: sse4
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

// SSE4::SSE4(float x0, float x1, float x2, float x3) {}


const __m128 SSE4::EPSILON4 = _mm_set_ps1(EPSILON);
const __m128 SSE4::EPSILON4_NEG = _mm_set_ps1(-EPSILON);
const __m128 SSE4::ONE = _mm_set_ps1(1.0f);

__m128 one;
const __m128 SSE4::BINONE = *((__m128*)memset(&one, 0xff, sizeof(__m128)));
