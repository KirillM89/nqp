#include <random>
#include <cmath>
#include <algorithm>
#include <gtest/gtest.h>
#include "test_data.h"
#include "types.h"
#include "qp.h"

#ifndef NNLS_TESTS_UTILS_H
#define NNLS_TESTS_UTILS_H
static std::random_device rd;  // Will be used to obtain a seed for the random number engine
static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
enum class CompareType {
	RELATIVE, 		// compare all the output values   abs((value[i] - baseline[i]) / baseline[i]) < tol
	STRICT, 		// compare all the output values   isSame(value[i], baseline[i], tol)  
	COST, 			// success if final cost <= baseline cost + tol
};

using namespace QP_NNLS;

enum class QPSolvers {
	QLD = 0,
};
struct CompareSettings {
	CompareSettings():
		compareType(CompareType::RELATIVE),
		qpSolver(QPSolvers::QLD)
	{
		uSettings.logLevel = 3;
	}
	CompareType compareType;
	QPSolvers qpSolver;
	UserSettings uSettings;
};
struct QPBaseline {
	matrix_t xOpt; // may be many optimal solutions
	double cost;
	DualLoopExitStatus dualStatus = DualLoopExitStatus::UNKNOWN;
	PrimalLoopExitStatus primalStatus = PrimalLoopExitStatus::UNKNOWN;
};

std::vector<double> GenRandomVector(int dim, double lw, double up);
matrix_t GenRandomMatrix(int nRows, int nCols, double lw, double up);
matrix_t GetRandomPosSemidefMatrix(int dim, double lw, double up);
matrix_t GetRandomPosSemidefSymmetricMatrix(int dim, double lw, double up);
matrix_t genRandomStrictLowerTriangular(int mSize, int iBg);
bool isNumber(const double& val);
void TestMatrixMult(const matrix_t& m1, const matrix_t& m2, const matrix_t& baseline);
void TestMatrixMultTranspose(const matrix_t& m, const std::vector<double>& v, const std::vector<double> & baseline);
void TestMatrixMultStrictLowTriangular(const matrix_t& m1, const matrix_t& m2);
void TestInvertGauss(const matrix_t& m);
void TestLinearTransformation(const QP_NNLS_TEST_DATA::QPProblem& problem, const matrix_t& trMatrix, 
							  const QP_NNLS_TEST_DATA::QPProblem& bl);
void TestM1M2T(const matrix_t& m1, const matrix_t& m2, const matrix_t& baseline);
void TestLDL(const matrix_t& M);
void TestLDLRemove(matrix_t& M ,int i);
void TestLDLAdd(matrix_t& M, const std::vector<double>& vc);
void TestMMTb(const matrix_t& M, const std::vector<double>& b);
void TestSolver(const QP_NNLS_TEST_DATA::QPProblem& problem, const UserSettings& settings, const QPBaseline& baseline);
double relativeVal(double a, double b);
class TestCholetskyBase {
public:
	TestCholetskyBase() = default;
	void virtual TestCholetsky(const matrix_t& m);
protected:
    const double zeroTol = 1.0e-10;
	const double tol = 1.0e-6;
};
class TestCholetskyFullPivoting: public TestCholetskyBase {
public:
	void TestCholetsky(const matrix_t& m) override;
};

class TestCholetskyParmetrizedRandom : public TestCholetskyBase, public ::testing::TestWithParam<std::size_t> {
public:	
	void Test(double minVal, double maxVal) {
		TestCholetsky(GetRandomPosSemidefMatrix(GetParam(), minVal, maxVal));
	}
};
class TestCholetskyFullPivotingParametrized: public TestCholetskyFullPivoting, public ::testing::TestWithParam<matrix_t> {
	// no new methods
};
class TestLDLParametrized : public ::testing::TestWithParam<std::size_t> {
protected:
	TestLDLParametrized() = default;
	void TestRandomMatrix(double minVal, double maxVal) {
		TestLDL(GetRandomPosSemidefMatrix(GetParam(), minVal, maxVal));
	}
	void TestRemoveFromSquareRandomMatrix(double minVal, double maxVal) {
		const std::size_t nRows = std::max(GetParam(), static_cast<std::size_t>(1));
		TestRemove(GetRandomPosSemidefMatrix(nRows, minVal, maxVal));
	}
	void TestRemoveFromRectRandomMatrix(double minVal, double maxVal, bool rowsGtCols = true) {
		if (rowsGtCols) {
			//generate matrix nRows x 1 ... nRows x nRows - 1 with step s, nRows > 1
			const std::size_t nRows = std::max(GetParam(), static_cast<std::size_t>(1));
			const std::size_t step = std::max(static_cast<std::size_t>(nRows / 10) , static_cast<std::size_t>(1));
			for (std::size_t nCols = 1; nCols <= nRows; nCols += step) {
				TestRemove(GenRandomMatrix(nRows, nCols, minVal, maxVal));
			}
		} else {
			const std::size_t nCols = std::max(GetParam(), static_cast<std::size_t>(1));
			const std::size_t step = std::max(static_cast<std::size_t>(nCols / 10) , static_cast<std::size_t>(1));
			for (std::size_t nRows = 1; nRows <= nCols; nRows += step) {
				TestRemove(GenRandomMatrix(nRows, nCols, minVal, maxVal));
			}
		}
	}
	void TestAdd(double minVal, double maxVal, bool rowsGtCols = true) {
		if (rowsGtCols) {
			const std::size_t nRows = std::max(GetParam(), static_cast<std::size_t>(1));
			for (int nCols = 1; nCols <= nRows; ++nCols) {
				matrix_t M = GenRandomMatrix(nRows, nCols, minVal, maxVal);
				const std::vector<double> v = GenRandomVector(nCols, minVal, maxVal);
				TestLDLAdd(M, v);
			}
		} else {
			const std::size_t nCols= std::max(GetParam(), static_cast<std::size_t>(1));
			for (int nRows = 1; nRows <= nCols; ++nRows) {
				matrix_t M = GenRandomMatrix(nRows, nCols, minVal, maxVal);
				const std::vector<double> v = GenRandomVector(nCols, minVal, maxVal);
				TestLDLAdd(M, v);
			}
		}
	}
private:
	void TestRemove(const matrix_t& M) {
		for (std::size_t row = 0; row < M.size(); ++row) {
			matrix_t Mtmp = M;
			TestLDLRemove(Mtmp, row);
		}
	}
};


class LinearTransform {
public:
	LinearTransform() = default;
	virtual ~LinearTransform() = default;
	void setQPProblem(const QP_NNLS_TEST_DATA::QPProblem& problem);
	const QP_NNLS_TEST_DATA::QPProblem& transform(const matrix_t& trfMat);
	const matrix_t& getH();
	const matrix_t& getA();
	const std::vector<double>& getC();
	const std::vector<double>& getInitCoordinates(const std::vector<double>& newCoordinates); 
protected:
	QP_NNLS_TEST_DATA::QPProblem problemTr;
	std::vector<double> b;
	std::vector<double> c;
	std::vector<double> ct;
	matrix_t A;
	matrix_t Ht;
	matrix_t At;
	matrix_t trMat;
};

class LinearTransformParametrized: public LinearTransform, public ::testing::TestWithParam<matrix_t> {
public:
	LinearTransformParametrized() = default;
	~LinearTransformParametrized() override = default;
	void SetUserSettings(const QP_NNLS::UserSettings& settings);
	virtual void TransformAndTest(const QP_NNLS_TEST_DATA::QPProblem& problem, const QPBaseline& baseline); 
	virtual QPBaseline ComputeBaseline(const QP_NNLS_TEST_DATA::QPProblem& problem);
protected:
	QP_NNLS::UserSettings settings;
};

class QPSolverComparator {
public:
	QPSolverComparator() = default;
	virtual ~QPSolverComparator() = default;
	void Set(QPSolvers solverType, CompareType = CompareType::RELATIVE);
	void Compare(const DenseQPProblem& problem, const UserSettings& settings, std::string logFile = "log.txt");
protected:
	QPSolvers solverType;
	DenseQPProblem problem;
	void ProcessResults(const QP_SOLVERS::QPOutput& outputSolver, const QP_SOLVERS::QPOutput& outputNNLS);
	void CheckConstrViolations(const matrix_t& A, const std::vector<double>& b, const std::vector<double>& lb, const std::vector<double>& ub, const std::vector<double>& x);
    void CompareSequence(const std::vector<double>& seq, const std::vector<double>& bl);
	void CompareValue(double val, double bl);
	CompareType compareType;
	double relTol = 1.0e-4;
	double strictTol = 1.0e-4;
	double costTol = 1.0e-3;
};

class HessianParametrizedTest: public ::testing::TestWithParam<double> {
public:
	enum class Modification {
		NO_MODIFICATIONS,
		DIAG_SCALING,
		UNBALANCE,
		STRATEGY_1,
	};
	HessianParametrizedTest() = default;
	double GetScaleFactor() { return GetParam(); }
	void SetModification(Modification modification) { this->modification = modification;}
	const QP_NNLS_TEST_DATA::QPProblem& getProblem(const QP_NNLS_TEST_DATA::QPProblem& baseProblem); 
protected:
	QP_NNLS_TEST_DATA::QPProblem problem;
	Modification modification = Modification::NO_MODIFICATIONS;
};
class ProblemReader {
public:
	ProblemReader() = default;
	~ProblemReader() = default;
	void Init(const matrix_t& H, const std::vector<double>& c, const matrix_t& A, const std::vector<double>& b);
	void Init(const matrix_t& H, const std::vector<double>& c, const matrix_t& A, const std::vector<double>& b, const std::vector<double>& lw, const std::vector<double>& up);
	const DenseQPProblem& getProblem() { return problem;}
protected:
    DenseQPProblem problem;
};
class QPBMComparator {
public:
	QPBMComparator() = default;
	~QPBMComparator() = default;
	void Set(const CompareSettings& settings);
	void Compare(const QP_NNLS_TEST_DATA::QPProblem& problem, const std::string& name);
protected:
    CompareSettings settings;
};
class QPTestBase {
protected:
	QPTestBase() {
		settings.uSettings.logLevel = 3;
		comparator.Set(settings);
	}
	void Set(const CompareSettings& settings) { this->settings = settings;}
	QPBMComparator comparator;
	CompareSettings settings;
};
class QPTestRelative: public QPTestBase, public::testing::Test {
protected:
	QPTestRelative() {
		settings.compareType = CompareType::RELATIVE;
	}
};
class QPTestCost: public QPTestBase, public ::testing::Test {
protected:
	QPTestCost() {
		settings.compareType = CompareType::COST;
	}
};
class CompareRelHessParametrized : public QPTestBase, public HessianParametrizedTest {
public:
	CompareRelHessParametrized() = default;
};

#endif

