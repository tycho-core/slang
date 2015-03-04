//////////////////////////////////////////////////////
// This file gets compiled before any user code, it
// defines well known aliases and compatibility 
// workarounds for hlsl v4
//////////////////////////////////////////////////////


//////////////////////////////////////////////////////
// DX8 EFFECT COMPATABILITY
//////////////////////////////////////////////////////

typedef int DWORD;
typedef float FLOAT; 
typedef vector <float, 4> VECTOR;
typedef matrix <float, 4, 4> MATRIX;
typedef string STRING;
typedef texture TEXTURE;
typedef pixelshader PIXELSHADER;
typedef vertexshader VERTEXSHADER;

//////////////////////////////////////////////////////
// VECTOR VARIANTS
//////////////////////////////////////////////////////

typedef vector <bool, 1> bool1;
typedef vector <int, 1> int1;
typedef vector <uint, 1> uint1;
typedef vector <half, 1> half1;
typedef vector <float, 1> float1;
typedef vector <double, 1> double1;

typedef vector <bool, 2> bool2;
typedef vector <int, 2> int2;
typedef vector <uint, 2> uint2;
typedef vector <half, 2> half2;
typedef vector <float, 2> float2;
typedef vector <double, 2> double2;

typedef vector <bool, 3> bool3;
typedef vector <int, 3> int3;
typedef vector <uint, 3> uint3;
typedef vector <half, 3> half3;
typedef vector <float, 3> float3;
typedef vector <double, 3> double3;

typedef vector <bool, 4> bool4;
typedef vector <int, 4> int4;
typedef vector <uint, 4> uint4;
typedef vector <half, 4> half4;
typedef vector <float, 4> float4;
typedef vector <double, 4> double4;

//////////////////////////////////////////////////////
// MATRIX VARIANTS
//////////////////////////////////////////////////////

typedef matrix <bool, 1, 1> bool1x1;
typedef matrix <int, 1, 1> int1x1;
typedef matrix <uint, 1, 1> uint1x1;
typedef matrix <half, 1, 1> half1x1;
typedef matrix <float, 1, 1> float1x1;
typedef matrix <double, 1, 1> double1x1;
typedef matrix <bool, 1, 2> bool1x2;
typedef matrix <int, 1, 2> int1x2;
typedef matrix <uint, 1, 2> uint1x2;
typedef matrix <half, 1, 2> half1x2;
typedef matrix <float, 1, 2> float1x2;
typedef matrix <double, 1, 2> double1x2;
typedef matrix <bool, 1, 3> bool1x3;
typedef matrix <int, 1, 3> int1x3;
typedef matrix <uint, 1, 3> uint1x3;
typedef matrix <half, 1, 3> half1x3;
typedef matrix <float, 1, 3> float1x3;
typedef matrix <double, 1, 3> double1x3;
typedef matrix <bool, 1, 4> bool1x4;
typedef matrix <int, 1, 4> int1x4;
typedef matrix <uint, 1, 4> uint1x4;
typedef matrix <half, 1, 4> half1x4;
typedef matrix <float, 1, 4> float1x4;
typedef matrix <double, 1, 4> double1x4;
typedef matrix <bool, 2, 1> bool2x1;
typedef matrix <int, 2, 1> int2x1;
typedef matrix <uint, 2, 1> uint2x1;
typedef matrix <half, 2, 1> half2x1;
typedef matrix <float, 2, 1> float2x1;
typedef matrix <double, 2, 1> double2x1;
typedef matrix <bool, 2, 2> bool2x2;
typedef matrix <int, 2, 2> int2x2;
typedef matrix <uint, 2, 2> uint2x2;
typedef matrix <half, 2, 2> half2x2;
typedef matrix <float, 2, 2> float2x2;
typedef matrix <double, 2, 2> double2x2;
typedef matrix <bool, 2, 3> bool2x3;
typedef matrix <int, 2, 3> int2x3;
typedef matrix <uint, 2, 3> uint2x3;
typedef matrix <half, 2, 3> half2x3;
typedef matrix <float, 2, 3> float2x3;
typedef matrix <double, 2, 3> double2x3;
typedef matrix <bool, 2, 4> bool2x4;
typedef matrix <int, 2, 4> int2x4;
typedef matrix <uint, 2, 4> uint2x4;
typedef matrix <half, 2, 4> half2x4;
typedef matrix <float, 2, 4> float2x4;
typedef matrix <double, 2, 4> double2x4;
typedef matrix <bool, 3, 1> bool3x1;
typedef matrix <int, 3, 1> int3x1;
typedef matrix <uint, 3, 1> uint3x1;
typedef matrix <half, 3, 1> half3x1;
typedef matrix <float, 3, 1> float3x1;
typedef matrix <double, 3, 1> double3x1;
typedef matrix <bool, 3, 2> bool3x2;
typedef matrix <int, 3, 2> int3x2;
typedef matrix <uint, 3, 2> uint3x2;
typedef matrix <half, 3, 2> half3x2;
typedef matrix <float, 3, 2> float3x2;
typedef matrix <double, 3, 2> double3x2;
typedef matrix <bool, 3, 3> bool3x3;
typedef matrix <int, 3, 3> int3x3;
typedef matrix <uint, 3, 3> uint3x3;
typedef matrix <half, 3, 3> half3x3;
typedef matrix <float, 3, 3> float3x3;
typedef matrix <double, 3, 3> double3x3;
typedef matrix <bool, 3, 4> bool3x4;
typedef matrix <int, 3, 4> int3x4;
typedef matrix <uint, 3, 4> uint3x4;
typedef matrix <half, 3, 4> half3x4;
typedef matrix <float, 3, 4> float3x4;
typedef matrix <double, 3, 4> double3x4;
typedef matrix <bool, 4, 1> bool4x1;
typedef matrix <int, 4, 1> int4x1;
typedef matrix <uint, 4, 1> uint4x1;
typedef matrix <half, 4, 1> half4x1;
typedef matrix <float, 4, 1> float4x1;
typedef matrix <double, 4, 1> double4x1;
typedef matrix <bool, 4, 2> bool4x2;
typedef matrix <int, 4, 2> int4x2;
typedef matrix <uint, 4, 2> uint4x2;
typedef matrix <half, 4, 2> half4x2;
typedef matrix <float, 4, 2> float4x2;
typedef matrix <double, 4, 2> double4x2;
typedef matrix <bool, 4, 3> bool4x3;
typedef matrix <int, 4, 3> int4x3;
typedef matrix <uint, 4, 3> uint4x3;
typedef matrix <half, 4, 3> half4x3;
typedef matrix <float, 4, 3> float4x3;
typedef matrix <double, 4, 3> double4x3;
typedef matrix <bool, 4, 4> bool4x4;
typedef matrix <int, 4, 4> int4x4;
typedef matrix <uint, 4, 4> uint4x4;
typedef matrix <half, 4, 4> half4x4;
typedef matrix <float, 4, 4> float4x4;
typedef matrix <double, 4, 4> double4x4;
