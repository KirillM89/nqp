
#include <vector>
#include <list>
#include <string>
#include "timers.h"
#ifndef NNLS_QP_SOLVER_TYPES_H
#define NNLS_QP_SOLVER_TYPES_H
namespace QP_NNLS {
using matrix_t = std::vector<std::vector<double>>;
namespace CONSTANTS {
	constexpr double cholFactorZero = 1.0e-14;
	constexpr double pivotZero = 1.0e-14;
}
static_assert(CONSTANTS::cholFactorZero > 0.0);
static_assert(CONSTANTS::pivotZero > 0.0);

struct CholetskyOutput {
    std::list<std::pair<int, double>> negativeDiag; // negative diagonal elements in range [-cholFactorZero, 0)
	double negativeBlocking = 1.0; // neagtive diagonal element which block all the next steps
	bool pivoting = false;
};

constexpr double gammaBegin = 1.0;

enum class SolverRetStatus {
	SUCCESS = 0,
	FAIL,
};
enum class ProblemConfiguration {
	DENSE = 0,
	SPARSE,
};
enum class GammaUpdateStrategyPrimal {
	NO_UPDATE = 0,
	DECREMENT_BY_D_NORM
};

enum class DBScalerStrategy {
	SCALE_FACTOR, 
	BALANCE,
	UNKNOWN
};

enum class CholPivotingStrategy {
	NO_PIVOTING,
	FULL,
	PARTIAL,
	UNKNOWN
};

enum class GammaUpdateStrategyDual {
	NO_UPDATE = 0,
	INCREMENT_BY_S_COMPONENT,
};

struct UserSettings {
	ProblemConfiguration configuration = ProblemConfiguration::DENSE;
	DBScalerStrategy dbScalerStrategy = DBScalerStrategy::SCALE_FACTOR;
	CholPivotingStrategy cholPvtStrategy = CholPivotingStrategy::NO_PIVOTING;
	int nDualIterations = 100;
	int nPrimalIterations = 100;
    int logLevel = 3;
	double nnlsResidNormFsb = 1.0e-16;
	double origPrimalFsb = 1.0e-6;
	double nnlsPrimalZero = -1.0e-16; // -1.0e-12; //zp < 0 => zp < nnlsPrimalZero
	double minNNLSDualTol = -1.0e-12;
	std::string logFile = "logNNLS.txt";
	bool checkProblem = false;
};

struct DenseQPProblem {
	//1/2xtHx + cx
	//Ax <= b
	//Fx = g
	matrix_t H;
	matrix_t A;
	matrix_t F;
	std::vector<double> b;
	std::vector<double> c;
	std::vector<double> g;
	std::vector<double> up;
	std::vector<double> lw;
};

struct SparseQPProblem {

};

struct ProblemSettings {
	ProblemSettings() = delete;
	ProblemSettings(const UserSettings& settings, const DenseQPProblem& problem):
		uSettings(settings), problemD(problem)
	{};
	virtual ~ProblemSettings() = default;
	UserSettings uSettings;
	const DenseQPProblem& problemD;
};


enum class DualLoopExitStatus {
    ALL_DUAL_POSITIVE = 0,
	FULL_ACTIVE_SET,
	ITERATIONS,
	INFEASIBILITY,
	UNKNOWN
};

enum class PrimalLoopExitStatus {
	EMPTY_ACTIVE_SET,
	ALL_PRIMAL_POSITIVE,
	ITERATIONS,
	EMPTY_ACTIVE_SET_ON_ZERO_ITERATION,
	SINGULAR_MATRIX,
	DIDNT_STARTED,
	UNKNOWN
};

enum class SolverExitStatus {
	CONSTRAINTS_VIOLATION = 0,
	SOLVE_PRIMAL,
	SUCCESS,
	UNKNOWN
};

struct SolverOutput {
	DualLoopExitStatus dualExitStatus = DualLoopExitStatus::UNKNOWN;
	PrimalLoopExitStatus primalExitStatus = PrimalLoopExitStatus::UNKNOWN;
	int nDualIterations;
	double maxViolation;
	double dualityGap;
	TimeIntervals tIntervals;
};

}
#endif
