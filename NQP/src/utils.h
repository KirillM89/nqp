
#ifndef NNLS_QP_SOLVER_UTILS_H
#define NNLS_QP_SOLVER_UTILS_H
#include <cassert>
#include "types.h"
namespace QP_NNLS {

	void ComputeCholFactor(const matrix_t& M, matrix_t& cholF) ; // M = cholF * cholF_T

	bool ComputeCholFactorT(const matrix_t& M, matrix_t& cholF, CholetskyOutput& output); // M = cholF_T * cholF

	int ComputeCholFactorTFullPivoting(matrix_t& M, matrix_t& cholF, std::vector<int>& permut); // P_T * M * P = cholF_T * cholF

	void Mult(const matrix_t& M1, const matrix_t& M2, matrix_t& mult); // M1 * M2

	void MultTransp(const matrix_t& M, const std::vector<double>& v, std::vector<double>& MTv); // MTv = M_T * v

	void MultTransp(const matrix_t& M, const std::vector<double>& v, const std::vector<int>& activesetIndices, std::vector<double>& MTv); // M_T * v on active set

	void M1M2T(const matrix_t& M1, const matrix_t& M2, matrix_t& MMT); // MMT = M1 * M2_T

	void M2M1T(const matrix_t& M1, const matrix_t& M2, matrix_t& MMT); // MMT = M2 * M1_T

	void M1TM2(const matrix_t& M1, const matrix_t& M2, matrix_t& MMT); // MMT = M1_T * M2

	void swapColumns(matrix_t& M, int r1, int r2);

	void InvertTriangle(const matrix_t& M, matrix_t& Minv);// M^-1 M -low triangular matrix

	void Mult(const matrix_t& M, const std::vector<double>& v, std::vector<double>& Mv); // Mv = M * v

	void VSum(const std::vector<double>& v1, const std::vector<double>& v2, std::vector<double>& sum); // sum = v1 + v2

	void VAdd(std::vector<double>& v1, const std::vector<double>& v2); // v1 += v2

	double DotProduct(const std::vector<double>& v1, const std::vector<double>& v2); // <v1,v2>

	double DotProduct(const std::vector<double>& v1, const std::vector<double>& v2,  const std::vector<int>& activeSetIndices); // <v1,v2> for active set indices 

	void InvertByGauss(const matrix_t& M, matrix_t& Minv); // invert matrix M using Gauss Elimination with pivoting, M_inv must be filled with zeros in advance

	void InvertLTrByGauss(const matrix_t& M, matrix_t& Minv); // invert low triangular matrix M using Gauss Elimination with pivoting, M_inv must be filled with zeros in advance

	void InvertEigen(const matrix_t& M, matrix_t& Inv); // invert matrix M using Eigen library; inverse() method

	void GetIdentityMatrix(int size, matrix_t& M); // set M as identity matrix 

	double bTAb(const std::vector<double>& b, const matrix_t& A); // b_T * A * b

	void RRF(matrix_t& M); // Transform M to reduced row echelone form

	void RRFB(matrix_t& matrix, std::vector<double>& b); // Transform  M to reduced row echelone form and b apply all the changes to vector b 

	void RCFB(matrix_t& A, std::vector<double>& b); // Transform M to reduced column echelone form

	void MultStrictLowTriangular(const matrix_t& M1, const matrix_t& M2, matrix_t& M1M2, int m1Bg = 1, int m2Bg = 1); // M1M2 = M1 * M2 , M1 and M2 -are strict low triangular

	void InvertTriangularFactorization(const matrix_t& M, matrix_t& Minv); // M-1 Minv, invert low triangular matrix using factorization method

	void PermuteColumns(matrix_t& A, const std::vector<int>& pmt);  // swaps columns of matrix A: A[i] <-> A[pmt[i]]

    void PTV(std::vector<double>& v, const std::vector<int>& pmt); // v -> P_T * v

	matrix_t& operator-(matrix_t& M); // M -> -M

	static inline bool isSame(double cand, double val, double tol = 1.0e-16) {
		assert(tol > 0.0);
		const double diff = cand - val;
		return ((diff >= -tol) && (diff <= tol));
	}
}
#endif