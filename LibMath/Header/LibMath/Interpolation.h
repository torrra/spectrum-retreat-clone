#pragma once

#include "Matrix.h"
#include "Vector.h"

namespace LibMath
{
	float blerp(const LibMath::Matrix2& qGrid, const LibMath::Matrix2& gridCoords, const LibMath::Vector2& point);

	float lerp(const float low, const float high, const float ratio);

	unsigned char lerp(unsigned char low, unsigned char high, unsigned char ratio);


	float getLerpRatio(const float val, const float low, const float high);

	Vector3 getBarycentricWeights(const Vector2& v1, const Vector2& v2, const Vector2& v3, float x, float y);

	float barycentricInterpolate(const Vector3& weights, const float val1, const float val2, const float val3);

	Vector3 tripleBarycentric(const Vector3& weights, const Vector3& val1, const Vector3& val2, const Vector3& val3);

}

namespace lm = LibMath;