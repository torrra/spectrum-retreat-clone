#include "Interpolation.h"


namespace LibMath
{
	float LibMath::blerp(const LibMath::Matrix2& qGrid, const LibMath::Matrix2& gridCoords, const LibMath::Vector2& point)
	{
		LibMath::Vector2       gridLength(gridCoords.m_matrix[0][1] - gridCoords.m_matrix[0][0], gridCoords.m_matrix[1][1] - gridCoords.m_matrix[1][0]);
		LibMath::Vector2       pointToEnd(gridCoords.m_matrix[0][1] - point[0], gridCoords.m_matrix[1][1] - point[1]);
		LibMath::Vector2       begToPoint(point[0] - gridCoords.m_matrix[0][0], point[1] - gridCoords.m_matrix[1][0]);

		float           areaDivider = 1.f / (gridLength[0] * gridLength[1]);

		float           matrixProduct = qGrid.m_matrix[0][0] * pointToEnd[0] * pointToEnd[1];

		matrixProduct += qGrid.m_matrix[1][0] * begToPoint[0] * pointToEnd[1];
		matrixProduct += qGrid.m_matrix[0][1] * pointToEnd[0] * begToPoint[1];
		matrixProduct += qGrid.m_matrix[1][1] * begToPoint[0] * begToPoint[1];

		return areaDivider * matrixProduct;
	}

	float lerp(const float low, const float high, const float ratio)
	{
		if (ratio == 0.f)
			return low;
		else if (ratio == 1.f)
			return high;

		else if (ratio < 0.f || ratio > 1.f)
		{
			if (low == high)
				return low;

			// Return 0 as result may not be defined
			else
				return 0.f;
		}

		return low + ratio * (high - low);
	}

	unsigned char LibMath::lerp(unsigned char low, unsigned char high, unsigned char ratio)
	{
		if (ratio == 0)
			return low;
		else if (ratio == 1)
			return high;

		else if (ratio < 0 || ratio > 1)
		{
			if (low == high)
				return low;

			// Return 0 as result may not be defined
			else
				return 0;
		}

		return low + ratio * (high - low);
	}

	float LibMath::getLerpRatio(const float val, const float low, const float high)
	{
		return (val - low) / (high - high);
	}

	Vector3 LibMath::getBarycentricWeights(const Vector2& v1, const Vector2& v2, const Vector2& v3, float x, float y)
	{
		float    detDivider = (v2.m_y - v3.m_y) * (v1.m_x - v3.m_x);
		detDivider += (v3.m_x - v2.m_x) * (v1.m_y - v3.m_y);
		detDivider = 1.f / detDivider;

		float s = (v2.m_y - v3.m_y) * (x - v3.m_x) + (v3.m_x - v2.m_x) * (y - v3.m_y);
		s *= detDivider;

		float t = (v3.m_y - v1.m_y) * (x - v3.m_x) + (v1.m_x - v3.m_x) * (y - v3.m_y);
		t *= detDivider;

		float k = 1.f - s - t;

		return Vector3(s, t, k);

	}

	float LibMath::barycentricInterpolate(const Vector3& weights, const float val1, const float val2, const float val3)
	{
		return weights.m_x * val1 + weights.m_y * val2 + weights.m_z * val3;
	}

	Vector3 LibMath::tripleBarycentric(const Vector3& weights, const Vector3& val1, const Vector3& val2, const Vector3& val3)
	{
		return weights.m_x * val1 + weights.m_y * val2 + weights.m_z * val3;
	}
}