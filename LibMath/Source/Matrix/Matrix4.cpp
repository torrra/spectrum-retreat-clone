#include <cmath>
#include <float.h>

#include "Matrix/Matrix4.h"
#include "Matrix/Matrix3.h"
#include "Arithmetic.h"

LibMath::Matrix4::Matrix4()
{
	// Initialize as identity matrix
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			this->m_matrix[i][j] = (i == j ? 1.0f : 0.0f);
		}
	}
}

LibMath::Matrix4::Matrix4(float scalar)
{
	// Initialize as identity matrix multiplied by a scalar
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			this->m_matrix[i][j] = (i == j ? scalar : 0.0f);
		}
	}
}

LibMath::Matrix4::Matrix4(float arr[4][4])
{
	for (int i = 0; i < 16; ++i)
	{
		this->m_matrix[i / 4][i % 4] = arr[i / 4][i % 4];
	}
}

LibMath::Matrix4::~Matrix4()
{
}

float LibMath::Matrix4::Determinant(const Matrix4& matrix)
{
	float determinant = 0.0f;

	for (int i = 0; i < 4; ++i)
	{
		// Create 3x3 matrix
		LibMath::Matrix3 minor;

		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				minor.m_matrix[j][k] = (k < i) ? matrix.m_matrix[j + 1][k] : matrix.m_matrix[j + 1][k + 1];
			}
		}

		float minorDeterminant = minor.Determinant(minor);
		determinant += (i % 2 == 0) ? matrix.m_matrix[0][i] * minorDeterminant : -matrix.m_matrix[0][i] * minorDeterminant;
	}

	return determinant;
}

LibMath::Matrix4 LibMath::Matrix4::Identity(float scalar)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			this->m_matrix[i][j] = (i == j ? scalar : 0.0f);
		}
	}

	return *this;
}

LibMath::Matrix4 LibMath::Matrix4::Transpose(const Matrix4& matrix)
{
	LibMath::Matrix4 matrix4 = matrix;

	// Transpose matrix
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			this->m_matrix[j][i] = matrix4.m_matrix[i][j];
		}
	}

	return *this;
}

LibMath::Matrix4 LibMath::Matrix4::Minor(const Matrix4& matrix)
{
	LibMath::Matrix3 matrix3;
	LibMath::Matrix4 matrixCopy(matrix);
	LibMath::Matrix4 resultMatrix;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			matrix3.GetMatrix3(matrixCopy, i, j);
			float det = matrix3.Determinant(matrix3);
			resultMatrix.m_matrix[i][j] = det;
		}
	}

	return resultMatrix;
}

LibMath::Matrix4 LibMath::Matrix4::Cofactor(const Matrix4& matrix)
{
	LibMath::Matrix4 resultMatrix;
	resultMatrix = resultMatrix.Minor(matrix);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			resultMatrix.m_matrix[i][j] *= ((i + j) % 2 == 0 ? 1.0f : -1.0f);
		}
	}

	return resultMatrix;
}

LibMath::Matrix4 LibMath::Matrix4::Adjugate(Matrix4& matrix)
{
	LibMath::Matrix4 matrix4;

	matrix = matrix.Cofactor(matrix);
	matrix = matrix4.Transpose(matrix);

	return matrix;
}

LibMath::Matrix4 LibMath::Matrix4::GetInverse() const
{
	// Check if inverse exists
	LibMath::Matrix4 matrixCopy = *this;
	LibMath::Matrix4 resultMatrix;

	float det = resultMatrix.Determinant(matrixCopy);

	// Check determinant is not zero
	if (det == 0.0f)
		return resultMatrix;

	resultMatrix = matrixCopy.Adjugate(matrixCopy) * (1.0f / det);

	return resultMatrix;
}

LibMath::Matrix4 LibMath::Matrix4::XRotation(float angle, bool rowMajor)
{
	LibMath::Matrix4 matrix4;

	float sinAngle = sinf(angle);
	float cosAngle = cosf(angle);


	matrix4.m_matrix[0][0] = 1.0f;
	matrix4.m_matrix[3][3] = 1.0f;

	if (rowMajor)
	{

		matrix4.m_matrix[1][1] = cosAngle;
		matrix4.m_matrix[1][2] = -sinAngle;
		matrix4.m_matrix[2][1] = sinAngle;
		matrix4.m_matrix[2][2] = cosAngle;
	}
	else
	{
		matrix4.m_matrix[1][1] = cosAngle;
		matrix4.m_matrix[1][2] = sinAngle;
		matrix4.m_matrix[2][1] = -sinAngle;
		matrix4.m_matrix[2][2] = cosAngle;
	}

	return matrix4;
}

LibMath::Matrix4 LibMath::Matrix4::YRotation(float angle, bool rowMajor)
{
	LibMath::Matrix4 matrix4;

	float sinAngle = sinf(angle);
	float cosAngle = cosf(angle);

	matrix4.m_matrix[1][1] = 1.0f;
	matrix4.m_matrix[3][3] = 1.0f;

	if (rowMajor)
	{
		matrix4.m_matrix[0][0] = cosAngle;
		matrix4.m_matrix[0][2] = sinAngle;
		matrix4.m_matrix[2][0] = -sinAngle;
		matrix4.m_matrix[2][2] = cosAngle;
	}
	else
	{
		matrix4.m_matrix[0][0] = cosAngle;
		matrix4.m_matrix[0][2] = -sinAngle;
		matrix4.m_matrix[2][0] = sinAngle;
		matrix4.m_matrix[2][2] = cosAngle;
	}

	return matrix4;
}

LibMath::Matrix4 LibMath::Matrix4::ZRotation(float angle, bool rowMajor)
{
	LibMath::Matrix4 matrix4;

	float sinAngle = sinf(angle);
	float cosAngle = cosf(angle);

	if (rowMajor)
	{
		matrix4.m_matrix[0][0] = cosAngle;
		matrix4.m_matrix[0][1] = sinAngle;
		matrix4.m_matrix[1][0] = -sinAngle;
		matrix4.m_matrix[1][1] = cosAngle;
		matrix4.m_matrix[2][2] = 1.0f;
		matrix4.m_matrix[3][3] = 1.0f;
	}
	else
	{
		matrix4.m_matrix[0][0] = cosAngle;
		matrix4.m_matrix[0][1] = -sinAngle;
		matrix4.m_matrix[1][0] = sinAngle;
		matrix4.m_matrix[1][1] = cosAngle;
		matrix4.m_matrix[2][2] = 1.0f;
		matrix4.m_matrix[3][3] = 1.0f;
	}

	return matrix4;
}

LibMath::Matrix4 LibMath::Matrix4::Translate(const Vector3& translation, bool rowMajor)
{
	if (rowMajor)
	{
		float matrix[4][4] =
		{
			{1.f, 0.f, 0.f, translation.m_x},
			{0.f, 1.f, 0.f, translation.m_y},
			{0.f, 0.f, 1.f, translation.m_z},
			{0.f, 0.f, 0.f, 1.f}
		};

		return Matrix4(matrix);
	}
	else
	{
		float matrix[4][4] =
		{
			{1.f, 0.f, 0.f, 0.f},
			{0.f, 1.f, 0.f, 0.f},
			{0.f, 0.f, 1.f, 0.f},
			{translation.m_x, translation.m_x, translation.m_x, 1.f}
		};

		return Matrix4(matrix);
	}
}

LibMath::Matrix4 LibMath::Matrix4::Scale(const Vector3& scale)
{
	float matrix[4][4] =
	{
		{scale.m_x, 0.f, 0.f, 0.f},
		{0.f, scale.m_y, 0.f, 0.f},
		{0.f, 0.f,scale.m_z, 0.f},
		{0.f, 0.f, 0.f, 1.f}
	};

	return Matrix4(matrix);
}

LibMath::Matrix4 LibMath::Matrix4::PerspectiveProjection(float fovy, float aspect, float near, float far)
{
	LibMath::Matrix4 projectionMatrix;
	const float pi = 3.14f;
	fovy = fovy * (pi / 180.0f);

	float tanAngle = tanf(fovy / 2.0f);

	projectionMatrix.m_matrix[0][0] = 1.0f / (aspect * tanAngle);
	projectionMatrix.m_matrix[1][1] = 1.0f / tanAngle;
	projectionMatrix.m_matrix[2][2] = -(far + near) / (far - near);
	projectionMatrix.m_matrix[2][3] = -(2.0f * far * near) / (far - near);
	projectionMatrix.m_matrix[3][2] = -1.0f;
	projectionMatrix.m_matrix[3][3] = 0.0f;

	return projectionMatrix.Transpose(projectionMatrix);
}

LibMath::Matrix4 LibMath::Matrix4::Orthographique(LibMath::Vector3 min, LibMath::Vector3 max)
{
	LibMath::Matrix4 ortho;

	ortho.m_matrix[0][0] = 2 / (max.m_x - min.m_x);
	ortho.m_matrix[3][0] = -(max.m_x + min.m_x) / (max.m_x - min.m_x);
	ortho.m_matrix[1][1] = 2 / (max.m_y - min.m_y);
	ortho.m_matrix[3][1] = -(max.m_y + min.m_y) / (max.m_y - min.m_y);
	ortho.m_matrix[2][2] = -2 / (max.m_z - min.m_z);
	ortho.m_matrix[3][2] = -(max.m_z + min.m_z) / (max.m_z - min.m_z);
	ortho.m_matrix[3][3] = 1;

	return ortho;
}

LibMath::Matrix4& LibMath::Matrix4::operator=(const Matrix4& matrixB)
{
	for (int i = 0; i < 16; ++i)
	{
		this->m_matrix[i / 4][i % 4] = matrixB.m_matrix[i / 4][i % 4];
	}

	return *this;
}

LibMath::Matrix4 LibMath::Matrix4::operator+(const Matrix4& matrixB) const
{
	LibMath::Matrix4 result = LibMath::Matrix4();

	for (int i = 0; i < 16; ++i)
	{
		result.m_matrix[i / 4][i % 4] = this->m_matrix[i / 4][i % 4] + matrixB.m_matrix[i / 4][i % 4];
	}

	return result;
}

LibMath::Matrix4 LibMath::Matrix4::operator-(const Matrix4& matrixB) const
{
	LibMath::Matrix4 result = LibMath::Matrix4();

	for (int i = 0; i < 16; ++i)
	{
		result.m_matrix[i / 4][i % 4] = this->m_matrix[i / 4][i % 4] - matrixB.m_matrix[i / 4][i % 4];
	}

	return result;
}

LibMath::Matrix4 LibMath::Matrix4::operator*(const Matrix4& matrixB) const
{
	// Create a null 4x4 matrix
	float initValues[4][4] =
	{
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f}
	};

	LibMath::Matrix4 result = LibMath::Matrix4(initValues);

	// Iterate through matrix to set the result matrix equal to the result of the 2 matrices multiplied
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				// Add the value obtained from multiplying the 2 matrices row * column
				result.m_matrix[i][j] += this->m_matrix[i][k] * matrixB.m_matrix[k][j];
			}
		}
	}

	return result;
}

LibMath::Matrix4 LibMath::Matrix4::operator*(const float& num) const
{
	Matrix4 tmp = LibMath::Matrix4();

	for (int i = 0; i < 16; ++i)
	{
		tmp.m_matrix[i / 4][i % 4] = m_matrix[i / 4][i % 4] * num;
	}

	return tmp;
}

LibMath::Matrix4& LibMath::Matrix4::operator+=(const Matrix4 matrixB)
{
	return *this = *this + matrixB;
}

LibMath::Matrix4& LibMath::Matrix4::operator-=(const Matrix4 matrixB)
{
	return *this = *this - matrixB;
}

LibMath::Matrix4& LibMath::Matrix4::operator*=(const Matrix4 matrixB)
{
	return *this = *this * matrixB;
}

LibMath::Matrix4& LibMath::Matrix4::operator*=(const float& num)
{
	return *this = *this * num;
}

bool LibMath::Matrix4::operator==(const Matrix4 matrixB) const
{
	// Check equality via epsilon test
	float difference = 0.0f;

	for (int i = 0; i < 16; ++i)
	{
		float num1 = LibMath::absolute(m_matrix[i / 4][i % 4]);
		float num2 = LibMath::absolute(matrixB.m_matrix[i / 4][i % 4]);

		float scaledEpsilon = FLT_EPSILON * LibMath::max(num1, num2);

		if (difference > scaledEpsilon)
			return false;
	}

	return true;
}

bool LibMath::Matrix4::operator!=(const Matrix4 matrixB) const
{
	// Check equality via epsilon test
	float difference = 0.0f;

	for (int i = 0; i < 16; ++i)
	{
		float num1 = LibMath::absolute(m_matrix[i / 4][i % 4]);
		float num2 = LibMath::absolute(matrixB.m_matrix[i / 4][i % 4]);

		float scaledEpsilon = FLT_EPSILON * LibMath::max(num1, num2);

		if (difference <= scaledEpsilon)
			return false;
	}

	return true;
}

void LibMath::Matrix4::SwapRows(float matrix[4][4], int row1, int row2) const
{
	for (int i = 0; i < 4; ++i)
	{
		float tmp = matrix[row1][i];
		matrix[row1][i] = matrix[row2][i];
		matrix[row2][i] = tmp;
	}
}

