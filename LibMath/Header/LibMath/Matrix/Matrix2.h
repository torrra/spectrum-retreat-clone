#ifndef __LIBMATH__MATRIX__MATRIX2_H__
#define __LIBMATH__MATRIX__MATRIX2_H__

#include "Matrix3.h"

namespace LibMath
{
	class Matrix2
	{
	public:
					Matrix2(void);									// Identity matrix
		explicit	Matrix2(float scalar);							// Identity matrix multiplied by a given scalar
					Matrix2(float x, float y, float z, float w);	// Initialized matrix with 4 given values

					~Matrix2();

		float		Determinant(const Matrix2 matrix);				// Return the determinant of a 2x2 matrix
		Matrix2		Identity(float scalar = 1.0f);					// Return an identity matrix multiplied by a scalar (scalar set to 1 if left empty)
		Matrix2		Transpose(const Matrix2& matrix);				// Swap rows & columns, then return this new matrix
		Matrix2		Minor(const Matrix2& mat);						// Return the matrix of every submatrix for every elements in the matrix
		Matrix2		Cofactor(const Matrix2& mat);
		Matrix2&	Adjugate(const Matrix2& mat);
		Matrix2		Inverse(const Matrix2& mat);

		Matrix2&	GetMatrix2(const Matrix3& matrix3, int row, int column);

		Matrix2		operator=(const Matrix2 matB);					// Set the value of 1 matrix equal to another
		Matrix2		operator+(const Matrix2 matB) const;			// Add to matrices together
		Matrix2		operator-(const Matrix2 matB) const;			// Subtract to matrices
		Matrix2		operator*(const Matrix2 matB) const;			// Multiply 2 matrices together
		Matrix2&	operator+=(const Matrix2& matB);				// Add 2 matrices together & set the first matrix equal to the result
		Matrix2&	operator-=(const Matrix2& matB);				// Subtract 1 matrix from another & set the first matrix equal to the result
		Matrix2&	operator*=(const Matrix2& matB);				// Multiply 2 matrices & set the first matrix equal to the result
		bool		operator==(Matrix2 matB) const;					// Compare 2 matrices return true if they are equal to one another
		bool		operator!=(const Matrix2 matB);					// Compare 2 matrices return true if they are not equal to one another

		float m_matrix[2][2];
	private:
	};

	//Matrix2	operator*(const Matrix2& matrixA, const Matrix2& matrixB);
	Matrix2	operator*(Matrix2& matrix, float scalar);
}

#endif // !__LIBMATH__MATRIX__MATRIX2_H__