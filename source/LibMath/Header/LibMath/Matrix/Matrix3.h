#ifndef __LIBMATH__MATRIX__MATRIX3_H__
#define __LIBMATH__MATRIX__MATRIX3_H__

#include "Matrix4.h"
#include "Vector.h"

namespace LibMath
{
	class Matrix3
	{
	public:
					Matrix3(void);									// Default constructor initializes as identity matrix
		explicit	Matrix3(float scalar);							// Constructor which multiplies an identity matrix by the given scalar
					Matrix3(float* arr);							// Constructor which takes a list of values

					~Matrix3();

		Matrix3		Transpose(const Matrix3& matrix);
		Matrix3		Identity(float scalar = 1.0f);					// Return an identity matrix multiplied by a scalar (scalar set to 1 if left empty)
		Matrix3		Minor(const Matrix3& matrix);					// Return the matrix of the result of every submatrix for every elements
		Matrix3		Cofactor(const Matrix3& matrix);				//
		Matrix3&	Adjugate(const Matrix3 matrix);
		Matrix3		Inverse(const Matrix3& matrix);
		float		Determinant(const Matrix3& matrix);				// Return the determinant of a 3x3 matrix

		static Matrix3        XRotation(float angle, bool rowMajor = false);                    // Get X rotation matrix
		static Matrix3        YRotation(float angle, bool rowMajor = false);                    // Get Y rotation matrix
		static Matrix3        ZRotation(float angle, bool rowMajor = false);                    // Get Z rotation matrix

		static Matrix3        Translate(const Vector2& translation, bool rowMajor = false);

		static Matrix3        Scale(const Vector2& scale);

		Matrix3&	GetMatrix3(const Matrix4& matrix4, int row, int column);

		Matrix3&	operator=(const Matrix3& matrixB);				// Set a matrix equal to another
		Matrix3		operator+(const Matrix3 matrixB) const;			// Add 2 matrices together & return the result
		Matrix3		operator-(const Matrix3 matrixB) const;			// Subtract one matrix from another & return the result
		Matrix3		operator*(const Matrix3 matrixB) const;			// Multiply two matrices together & return the result
		Matrix3		operator*(const float num) const;				// Multiply a matrix with a scalar & return the result
		Matrix3&	operator+=(const Matrix3 matrixB);				// Add 2 matrices together & set the first matrix equal to the result
		Matrix3&	operator-=(const Matrix3 matrixB);				// Subtract 1 matrix from another & set the first matrix equal to the result
		Matrix3&	operator*=(const Matrix3 matrixB);				// Multiply 2 matrices & set the first matrix equal to the result
		Matrix3&	operator*=(const float& num);					// Multiply a matrix by a scalar & modify the matrix values with the result
		bool		operator==(const Matrix3 matrixB) const;		// Compare 2 matrices return true if they are equal to one another
		bool		operator!=(const Matrix3 matrixB) const;		// Compare 2 matrices return true if they are not equal to one another

		float m_matrix[3][3];
	};
}

#endif // !__LIBMATH__MATRIX__MATRIX3_H__
