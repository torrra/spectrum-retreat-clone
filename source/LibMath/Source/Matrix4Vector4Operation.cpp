#include "Matrix4Vector4Operation.h"

LibMath::Vector4 LibMath::operator*(Matrix4 const& matrix4, Vector4 const& vector)
{
	LibMath::Vector4 product;

	for (int i = 0; i < 4; ++i)
	{
		product.m_x += matrix4.m_matrix[0][i] * vector[i];
	}

	for (int j = 0; j < 4; ++j)
	{
		product.m_y += matrix4.m_matrix[1][j] * vector[j];
	}

	for (int k = 0; k < 4; ++k)
	{
		product.m_z += matrix4.m_matrix[2][k] * vector[k];
	}

	return product;
}
