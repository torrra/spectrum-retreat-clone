#include <cmath>
#include <float.h>

#include "Matrix/Matrix2.h"
#include "Arithmetic.h"

LibMath::Matrix2::Matrix2(void)
{
	// Initialize with 2x2 identity matrix
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			i == j ? this->m_matrix[i][j] = 1.0f : this->m_matrix[i][j] = 0.0f;
		}
	}
}

LibMath::Matrix2::Matrix2(float scalar)
{
	// Initialize with 2x2 identity matrix
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			i == j ? this->m_matrix[i][j] = scalar : this->m_matrix[i][j] = 0.0f;
		}
	}
}

LibMath::Matrix2::Matrix2(float x, float y, float z, float w)
{
	this->m_matrix[0][0] = x;
	this->m_matrix[0][1] = y;
	this->m_matrix[1][0] = z;
	this->m_matrix[1][1] = w;
}

LibMath::Matrix2::~Matrix2()
{
}

float LibMath::Matrix2::Determinant(const Matrix2 matrix)
{
	float a = matrix.m_matrix[0][0] * matrix.m_matrix[1][1];
	float b = matrix.m_matrix[0][1] * matrix.m_matrix[1][0];

	return a - b;
}

LibMath::Matrix2 LibMath::Matrix2::Identity(float scalar)
{
	//LibMath::Matrix2 matrix2;
	//return matrix2 * scalar;

	this->m_matrix[0][0] = 1.0f;
	this->m_matrix[0][1] = 0.0f;
	this->m_matrix[1][0] = 0.0f;
	this->m_matrix[1][1] = 1.0f;

	return *this * scalar;
}

LibMath::Matrix2 LibMath::Matrix2::Transpose(const Matrix2& matrix2)
{
	LibMath::Matrix2 tmpMatrix = LibMath::Matrix2(
		matrix2.m_matrix[0][0],
		matrix2.m_matrix[0][1],
		matrix2.m_matrix[1][0],
		matrix2.m_matrix[1][1]
	);

	// Transpose matrix
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			this->m_matrix[j][i] = tmpMatrix.m_matrix[i][j];
		}
	}

	return *this;
}

LibMath::Matrix2 LibMath::Matrix2::Minor(const Matrix2& matrix2)
{
	*this = matrix2;

	this->m_matrix[0][0] = std::powf(-1.0f, 2) * this->m_matrix[0][0];
	this->m_matrix[0][1] = std::powf(-1.0f, 3) * this->m_matrix[0][1];
	this->m_matrix[1][0] = std::powf(-1.0f, 3) * this->m_matrix[1][0];
	this->m_matrix[1][1] = std::powf(-1.0f, 4) * this->m_matrix[1][1];

	return *this;
}

LibMath::Matrix2 LibMath::Matrix2::Cofactor(const Matrix2& matrix2)
{
	// TODO: finish cofactor function
	(void) matrix2;

	return Matrix2();
}

LibMath::Matrix2& LibMath::Matrix2::Adjugate(const Matrix2& matrix2)
{
	float tmp = matrix2.m_matrix[0][0];

	this->m_matrix[0][0] = matrix2.m_matrix[1][1];
	this->m_matrix[1][0] = -matrix2.m_matrix[1][0];
	this->m_matrix[0][1] = -matrix2.m_matrix[0][1];
	this->m_matrix[1][1] = tmp;

	return *this;
}

LibMath::Matrix2 LibMath::Matrix2::Inverse(const Matrix2& matrix2)
{
	LibMath::Matrix2 matrixCopy = matrix2;
	LibMath::Matrix2 resultMatrix;

	// Get determinant of matrix
	float det = Determinant(matrix2);

	// Check determinant is not zero or return an identity matrix
	if (det == 0.0f)
		return LibMath::Matrix2(); // TODO: check might cause potential issues

	resultMatrix = matrixCopy.Adjugate(matrix2) * (1 / det);

	return resultMatrix;
}

LibMath::Matrix2& LibMath::Matrix2::GetMatrix2(const Matrix3& matrix3, int row, int column)
{
	int iOffset = 0;
	int jOffset = 0;

	for (int i = 0; i < 3; ++i)
	{
		if (i == row)
		{
			jOffset = -1;
			continue;
		}

		iOffset = 0;
		for (int j = 0; j < 3; ++j)
		{
			if (j == column)
			{
				iOffset = -1;
				continue;
			}

			m_matrix[i + jOffset][j + iOffset] = matrix3.m_matrix[i][j];
		}
	}

	return *this;
}

LibMath::Matrix2 LibMath::Matrix2::operator=(const Matrix2 matB)
{

	this->m_matrix[0][0] = matB.m_matrix[0][0];
	this->m_matrix[0][1] = matB.m_matrix[0][1];
	this->m_matrix[1][0] = matB.m_matrix[1][0];
	this->m_matrix[1][1] = matB.m_matrix[1][1];

	return *this;
}

LibMath::Matrix2 LibMath::Matrix2::operator+(const Matrix2 matB) const
{
	LibMath::Matrix2 tmp = LibMath::Matrix2();

	tmp.m_matrix[0][0] = this->m_matrix[0][0] + matB.m_matrix[0][0];
	tmp.m_matrix[0][1] = this->m_matrix[0][1] + matB.m_matrix[0][1];
	tmp.m_matrix[1][0] = this->m_matrix[1][0] + matB.m_matrix[1][0];
	tmp.m_matrix[1][1] = this->m_matrix[1][1] + matB.m_matrix[1][1];

	return tmp;
}

LibMath::Matrix2 LibMath::Matrix2::operator-(const Matrix2 matB) const
{
	LibMath::Matrix2 tmp = LibMath::Matrix2();

	tmp.m_matrix[0][0] = this->m_matrix[0][0] - matB.m_matrix[0][0];
	tmp.m_matrix[0][1] = this->m_matrix[0][1] - matB.m_matrix[0][1];
	tmp.m_matrix[1][0] = this->m_matrix[1][0] - matB.m_matrix[1][0];
	tmp.m_matrix[1][1] = this->m_matrix[1][1] - matB.m_matrix[1][1];

	return tmp;
}

LibMath::Matrix2 LibMath::Matrix2::operator*(const Matrix2 matrixB) const
{
	LibMath::Matrix2 tmp = LibMath::Matrix2
	(
		(this->m_matrix[0][0] * matrixB.m_matrix[0][0]) + (this->m_matrix[0][1] * matrixB.m_matrix[1][0]),
		(this->m_matrix[0][0] * matrixB.m_matrix[0][1]) + (this->m_matrix[0][1] * matrixB.m_matrix[1][1]),
		(this->m_matrix[1][0] * matrixB.m_matrix[0][0]) + (this->m_matrix[1][1] * matrixB.m_matrix[1][0]),
		(this->m_matrix[1][0] * matrixB.m_matrix[0][1]) + (this->m_matrix[1][1] * matrixB.m_matrix[1][1])
	);

	return tmp;
}

LibMath::Matrix2& LibMath::Matrix2::operator+=(const Matrix2& matrixB)
{
	*this = *this + matrixB;

	return *this;
}

LibMath::Matrix2& LibMath::Matrix2::operator-=(const Matrix2& matrixB)
{
	*this = *this - matrixB;

	return *this;
}

LibMath::Matrix2& LibMath::Matrix2::operator*=(const Matrix2& matrixB)
{
	*this = *this * matrixB;

	return *this;
}

bool LibMath::Matrix2::operator==(const Matrix2 matrixB) const
{
	// Check equality via epsilon test
	float difference = 0.0f;

	for (int i = 0; i < 4; ++i)
	{
		float num1 = LibMath::absolute(m_matrix[i / 2][i % 2]);
		float num2 = LibMath::absolute(matrixB.m_matrix[i / 2][i % 2]);

		float scaledEpsilon = FLT_EPSILON * LibMath::max(num1, num2);

		if (difference > scaledEpsilon)
			return false;
	}

	return true;
}

bool LibMath::Matrix2::operator!=(const Matrix2 matrixB)
{
	// Check equality via epsilon test
	float difference = 0.0f;

	for (int i = 0; i < 4; ++i)
	{
		float num1 = LibMath::absolute(m_matrix[i / 2][i % 2]);
		float num2 = LibMath::absolute(matrixB.m_matrix[i / 2][i % 2]);

		float scaledEpsilon = FLT_EPSILON * LibMath::max(num1, num2);

		if (difference <= scaledEpsilon)
			return false;
	}

	return true;
}

//LibMath::Matrix2 LibMath::operator*(const Matrix2& matrixA, const Matrix2& matrixB)
//{
//	LibMath::Matrix2 newMatrix
//	(
//		(matrixA.m_matrix[0][0] * matrixB.m_matrix[0][0]) + (matrixA.m_matrix[0][1] * matrixB.m_matrix[1][0]),
//		(matrixA.m_matrix[0][0] * matrixB.m_matrix[0][1]) + (matrixA.m_matrix[0][1] * matrixB.m_matrix[1][1]),
//		(matrixA.m_matrix[1][0] * matrixB.m_matrix[0][0]) + (matrixA.m_matrix[1][1] * matrixB.m_matrix[1][0]),
//		(matrixA.m_matrix[1][0] * matrixB.m_matrix[0][1]) + (matrixA.m_matrix[1][1] * matrixB.m_matrix[1][1])
//	);
//
//	return newMatrix;
//}

LibMath::Matrix2 LibMath::operator*(Matrix2& matrix2, float scalar) // TODO: change to const Matrix2&
{
	// Multiply each element in the matrix by the given scalar
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			matrix2.m_matrix[i][j] *= scalar;
		}
	}

	return matrix2;
}
