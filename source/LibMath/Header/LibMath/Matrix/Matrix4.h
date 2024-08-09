#ifndef __LIBMATH__MATRIX__MATRIX4_H__
#define __LIBMATH__MATRIX__MATRIX4_H__

#include "../Vector/Vector3.h"

namespace LibMath
{
	class Matrix4
	{
	public:
						Matrix4();											// Initialize 4x4 identity matrix
		explicit		Matrix4(float scalar);								// Initialize 4x4 identity matrix multiplied by a scalar
						Matrix4(float values[4][4]);						// Initialize 4x4 matrix using values

						~Matrix4();

		float			Determinant(const Matrix4& matrix);					// Return the determinant of a 4x4 matrix
		Matrix4			Identity(float scalar = 1.0f);						// Return an identity matrix multiplied by a scalar (1 if left empty)
		Matrix4			Transpose(const Matrix4& matrix);					// Invert matrix rows and columns
		Matrix4			Minor(const Matrix4& matrix);						// Return the matrix of the result of every submatrix for every elements
		Matrix4			Cofactor(const Matrix4& matrix);
		Matrix4			Adjugate(Matrix4& matrix);
		Matrix4			GetInverse() const;									// Return matrix to the power of -1

		static Matrix4            XRotation(float angle, bool rowMajor = false);                        // Return a rotation matrix for the x axis
		static Matrix4            YRotation(float angle, bool rowMajor = false);                        // Return a rotation matrix for the y axis
		static Matrix4            ZRotation(float angle, bool rowMajor = false);						// Return a rotation matrix for the z axis

		static Matrix4				Translate(const Vector3& translation, bool rowMajor = false);

		static Matrix4				Scale(const Vector3& scale);

		static Matrix4	PerspectiveProjection(float fovy, float aspect, float near, float far);

		static Matrix4	Orthographique(LibMath::Vector3 min, LibMath::Vector3 max);

		Matrix4&		operator=(const Matrix4& matrixB);					// Set a matrix equal to another
		Matrix4			operator+(const Matrix4& matrixB) const;			// Add 2 matrices together & return the result
		Matrix4			operator-(const Matrix4& matrixB) const;			// Subtract one matrix from another & return the result
		Matrix4			operator*(const Matrix4& matrixB) const;			// Multiply two matrices together & return the result
		Matrix4			operator*(const float& num) const;					// Multiply a matrix with a scalar & return the result
		Matrix4&		operator+=(const Matrix4 matrixB);					// Add 2 matrices together & set the first matrix equal to the result
		Matrix4&		operator-=(const Matrix4 matrixB);					// Subtract 1 matrix from another & set the first matrix equal to the result
		Matrix4&		operator*=(const Matrix4 matrixB);					// Multiply 2 matrices & set the first matrix equal to the result
		Matrix4&		operator*=(const float& num);						// Multiply a matrix by a scalar & modify the matrix values with the result
		bool			operator==(const Matrix4 matrixB) const;			// Compare 2 matrices return true if they are equal to one another
		bool			operator!=(const Matrix4 matrixB) const;			// Compare 2 matrices return true if they are not equal to one another

		float m_matrix[4][4];
	private:
		void			SwapRows(float matrix4[4][4], int row1, int row2) const;
	};

}

#endif // !__LIBMATH__MATRIX__MATRIX4_H__
