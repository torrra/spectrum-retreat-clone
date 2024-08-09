#include <iostream>
#include <cerrno>
#include <float.h>

#include "Matrix/Matrix3.h"
#include "Matrix/Matrix2.h"
#include "Arithmetic.h"
#include "Angle.h"

// Constructor to create a 3x3 identity matrix
LibMath::Matrix3::Matrix3()
{
	// Initialize as identity matrix
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->m_matrix[i][j] = (i == j ? 1.0f : 0.0f);
		}
	}
}

// Constructor to create a 3x3 identity matrix multiplied by a scalar
LibMath::Matrix3::Matrix3(float scalar)
{
	// Initialize as identity matrix multiplied by a scalar
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->m_matrix[i][j] = (i == j ? scalar : 0.0f);
		}
	}
}

LibMath::Matrix3::Matrix3(float* arr)
{
	for (int i = 0; i < 9; i++)
	{
		if (i < 3)
			this->m_matrix[0][i] = arr[i];
		else if (i < 6)
			this->m_matrix[1][i - 3] = arr[i];
		else
			this->m_matrix[2][i - 6] = arr[i];
	}
}

// Destructor
LibMath::Matrix3::~Matrix3()
{
}

LibMath::Matrix3 LibMath::Matrix3::Transpose(const Matrix3& matrix)
{
	LibMath::Matrix3 matrix3 = matrix;

	// Transpose matrix
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->m_matrix[j][i] = matrix3.m_matrix[i][j];
		}
	}

	return *this;
}

LibMath::Matrix3 LibMath::Matrix3::Identity(float scalar)
{
	// Create an identity matrix and multiply by scalar
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->m_matrix[i][j] = (i == j ? scalar : 0.0f);
		}
	}

	return *this;
}

LibMath::Matrix3 LibMath::Matrix3::Minor(const Matrix3& matrix)
{
	LibMath::Matrix2 matrix2;
	LibMath::Matrix3 matrixCopy(matrix);
	LibMath::Matrix3 resultMatrix;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			matrix2.GetMatrix2(matrixCopy, i, j);
			float det = matrix2.Determinant(matrix2);
			resultMatrix.m_matrix[i][j] = det;
		}
	}

	return resultMatrix;
}

LibMath::Matrix3 LibMath::Matrix3::Cofactor(const Matrix3& matrix)
{
	LibMath::Matrix3 resultMatrix;
	resultMatrix = resultMatrix.Minor(matrix);

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			resultMatrix.m_matrix[i][j] *= ((i + j) % 2 == 0 ? 1.0f : -1.0f);
		}
	}

	return resultMatrix;
}

LibMath::Matrix3& LibMath::Matrix3::Adjugate(const Matrix3 mat)
{
	Matrix2 matrix2;

	// Row 1
	this->m_matrix[0][0] = +matrix2.Determinant({mat.m_matrix[1][1], mat.m_matrix[1][2], mat.m_matrix[2][1], mat.m_matrix[2][2]});
	this->m_matrix[0][1] = -matrix2.Determinant({mat.m_matrix[0][1], mat.m_matrix[0][2], mat.m_matrix[2][1], mat.m_matrix[2][2]});
	this->m_matrix[0][2] = +matrix2.Determinant({mat.m_matrix[0][1], mat.m_matrix[0][2], mat.m_matrix[1][1], mat.m_matrix[1][2]});

	// Row 2
	this->m_matrix[1][0] = -matrix2.Determinant({mat.m_matrix[1][0], mat.m_matrix[1][2], mat.m_matrix[2][0], mat.m_matrix[2][2]});
	this->m_matrix[1][1] = +matrix2.Determinant({mat.m_matrix[0][0], mat.m_matrix[0][2], mat.m_matrix[2][0], mat.m_matrix[2][2]});
	this->m_matrix[1][2] = -matrix2.Determinant({mat.m_matrix[0][0], mat.m_matrix[0][2], mat.m_matrix[1][0], mat.m_matrix[1][2]});

	// Row 3
	this->m_matrix[2][0] = +matrix2.Determinant({mat.m_matrix[1][0], mat.m_matrix[1][1], mat.m_matrix[2][0], mat.m_matrix[2][1]});
	this->m_matrix[2][1] = -matrix2.Determinant({mat.m_matrix[0][0], mat.m_matrix[0][1], mat.m_matrix[2][0], mat.m_matrix[2][1]});
	this->m_matrix[2][2] = +matrix2.Determinant({mat.m_matrix[0][0], mat.m_matrix[0][1], mat.m_matrix[1][0], mat.m_matrix[1][1]});

	return *this;
}

LibMath::Matrix3 LibMath::Matrix3::Inverse(const Matrix3& matrix)
{
	// Check if inverse exists
	LibMath::Matrix3 matrixCopy = matrix;
	LibMath::Matrix3 resultMatrix;

	float det = resultMatrix.Determinant(matrixCopy);

	// Check determinant is not zero
	if (det == 0.0f)
		return resultMatrix;

	resultMatrix = matrixCopy.Adjugate(matrixCopy) * (1.0f / det);

	return resultMatrix;
}

float LibMath::Matrix3::Determinant(const Matrix3& matrix)
{
	/*
		Split 3x3 matrix into 3 2x2 matrix and multiply by the coefficient

		Equation:
		|a b c|
		|d e f|
		|g h i|

		Determinant = a(ei - fh) - b(di - gf) + c(dh - eg)
	*/
	float a = matrix.m_matrix[0][0] * ((matrix.m_matrix[1][1] * matrix.m_matrix[2][2]) - (matrix.m_matrix[1][2] * matrix.m_matrix[2][1]));
	float b = matrix.m_matrix[0][1] * ((matrix.m_matrix[1][0] * matrix.m_matrix[2][2]) - (matrix.m_matrix[1][2] * matrix.m_matrix[2][0]));
	float c = matrix.m_matrix[0][2] * ((matrix.m_matrix[1][0] * matrix.m_matrix[2][1]) - (matrix.m_matrix[1][1] * matrix.m_matrix[2][0]));

	return a - b + c;
}

LibMath::Matrix3 LibMath::Matrix3::XRotation(float angle, bool rowMajor)
{
	float sinAngle = sinf(angle);
	float cosAngle = cosf(angle);

	if (rowMajor)
	{
		float matrix[] =
		{
			1.0f, 0.0f, 0.0f,
			0.0f, cosAngle, -sinAngle,
			0.0f, sinAngle, cosAngle
		};

		return LibMath::Matrix3(matrix);
	}
	else
	{
		float matrix[] =
		{
			1.0f, 0.0f, 0.0f,
			0.0f, cosAngle, sinAngle,
			0.0f, -sinAngle, cosAngle
		};

		return LibMath::Matrix3(matrix);

	}

}

LibMath::Matrix3 LibMath::Matrix3::YRotation(float angle, bool rowMajor)
{
	float sinAngle = sinf(angle);
	float cosAngle = cosf(angle);

	if (rowMajor)
	{
		float matrix[] =
		{
			cosAngle, 0.0f, sinAngle,
			0.0f, 1.0f, 0.0f,
			-sinAngle, 0.0f, cosAngle
		};

		return LibMath::Matrix3(matrix);
	}
	else
	{
		float matrix[] =
		{
			cosAngle, 0.0f, -sinAngle,
			0.0f, 1.0f, 0.0f,
			sinAngle, 0.0f, cosAngle
		};

		return LibMath::Matrix3(matrix);
	}
}

LibMath::Matrix3 LibMath::Matrix3::ZRotation(float angle, bool rowMajor)
{
	float sinAngle = sinf(angle);
	float cosAngle = cosf(angle);

	if (rowMajor)
	{
		float matrix[] =
		{
			cosAngle, -sinAngle, 0.0f,
			sinAngle, cosAngle, 0.0f,
			0.0f, 0.0f, 1.0f
		};

		return LibMath::Matrix3(matrix);
	}
	else
	{
		float matrix[] =
		{
			cosAngle, sinAngle, 0.0f,
			-sinAngle, cosAngle, 0.0f,
			0.0f, 0.0f, 1.0f
		};

		return LibMath::Matrix3(matrix);
	}
}

LibMath::Matrix3 LibMath::Matrix3::Translate(const LibMath::Vector2& translation, bool rowMajor)
{
	if (rowMajor)
	{
		float matrix[] =
		{
			1.f, 0.f, translation.m_x,
			0.f, 1.f, translation.m_y,
			0.f, 0.f, 1.f

		};

		return LibMath::Matrix3(matrix);
	}
	else
	{
		float matrix[] =
		{
			1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			translation.m_x, translation.m_y, 1.f

		};

		return LibMath::Matrix3(matrix);
	}
}

LibMath::Matrix3 LibMath::Matrix3::Scale(const Vector2& scale)
{
	float matrix[] =
	{
		scale.m_x, 0.f, 0.f,
		0.f, scale.m_y, 0.f,
		0.f, 0.f, 1.f
	};

	return LibMath::Matrix3(matrix);
}

LibMath::Matrix3& LibMath::Matrix3::GetMatrix3(const Matrix4& matrix4, int row, int column)
{
	int iOffset = 0;
	int jOffset = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (i == row)
		{
			jOffset = -1;
			continue;
		}

		iOffset = 0;
		for (int j = 0; j < 4; ++j)
		{
			if (j == column)
			{
				iOffset = -1;
				continue;
			}

			m_matrix[i + jOffset][j + iOffset] = matrix4.m_matrix[i][j];
		}
	}

	return *this;
}

LibMath::Matrix3& LibMath::Matrix3::operator=(const Matrix3& matrixB)
{
	// Copy all matrix values
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			this->m_matrix[i][j] = matrixB.m_matrix[i][j];
		}
	}

	return *this;
}

LibMath::Matrix3 LibMath::Matrix3::operator+(const Matrix3 matrixB) const
{
	Matrix3 tmp;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			tmp.m_matrix[i][j] = this->m_matrix[i][j] + matrixB.m_matrix[i][j];
		}
	}

	return tmp;
}

LibMath::Matrix3 LibMath::Matrix3::operator-(const Matrix3 matrixB) const
{
	Matrix3 tmp = LibMath::Matrix3();

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			tmp.m_matrix[i][j] = this->m_matrix[i][j] - matrixB.m_matrix[i][j];
		}
	}

	return tmp;
}

LibMath::Matrix3 LibMath::Matrix3::operator*(const Matrix3 matrixB) const
{

	// Create empty 3x3 matrix
	float initValues[9] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	Matrix3 result(initValues);

	// Iterate through matrix to set the result matrix equal to the result of the 2 matrices multiplied
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				// Add the value obtained from multiplying the 2 matrices row * column
				result.m_matrix[i][j] += this->m_matrix[i][k] * matrixB.m_matrix[k][j];
			}
		}
	}

	return result;
}

LibMath::Matrix3 LibMath::Matrix3::operator*(const float num) const
{
	Matrix3 tmp;

	for (int i = 0; i < 9; ++i)
	{
		tmp.m_matrix[i / 3][i % 3] = m_matrix[i / 3][i % 3] * num;
	}

	return tmp;
}

LibMath::Matrix3& LibMath::Matrix3::operator+=(const Matrix3 matrixB)
{
	return *this = *this + matrixB;
}

LibMath::Matrix3& LibMath::Matrix3::operator-=(const Matrix3 matrixB)
{
	return *this = *this - matrixB;
}

LibMath::Matrix3& LibMath::Matrix3::operator*=(const Matrix3 matrixB)
{
	return *this = *this * matrixB;
}

LibMath::Matrix3& LibMath::Matrix3::operator*=(const float& num)
{
	return *this = *this * num;
}

bool LibMath::Matrix3::operator==(const Matrix3 matrixB) const
{
	// Check equality via epsilon test
	float difference = 0.0f;

	for (int i = 0; i < 9; ++i)
	{
		float num1 = LibMath::absolute(m_matrix[i / 3][i % 3]);
		float num2 = LibMath::absolute(matrixB.m_matrix[i / 3][i % 3]);

		float scaledEpsilon = FLT_EPSILON * LibMath::max(num1, num2);

		if (difference > scaledEpsilon)
			return false;
	}

	return true;
}

bool LibMath::Matrix3::operator!=(const Matrix3 matrixB) const
{
	// Check equality via epsilon test
	float difference = 0.0f;

	for (int i = 0; i < 9; ++i)
	{
		float num1 = LibMath::absolute(m_matrix[i / 3][i % 3]);
		float num2 = LibMath::absolute(matrixB.m_matrix[i / 3][i % 3]);

		float scaledEpsilon = FLT_EPSILON * LibMath::max(num1, num2);

		if (difference <= scaledEpsilon)
			return false;
	}

	return true;
}

