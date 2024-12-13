#include "linSolvers.h"
#include "NNLSQPSolver.h" // MMTBSolver
namespace QP_NNLS {
CumulativeSolver::CumulativeSolver(const matrix_t& M,
                                   const std::vector<double>& s ):
    nConstraints(M.size()),
    nVariables(0),
    nActive(0),
    gamma(1.0),
    M(M),
    s(s)
{
    if (nConstraints > 0) {
        nVariables = M.front().size();
    }
    activeSet.resize(nConstraints, false);
}
bool CumulativeSolver::Add(const std::vector<double>& mp, double sp, unsg_t indx) {
    activeSet[indx] = true;
    ++nActive;
    return true;
}
bool CumulativeSolver::Delete(unsg_t indx) {
    activeSet[indx] = false;
    if (nActive > 0) {
        --nActive;
    }
    return true;
}
CumulativeLDLTSolver::CumulativeLDLTSolver(const matrix_t& M,
                                           const std::vector<double>& s):
    CumulativeSolver(M, s)
{}

const LinSolverOutput& CumulativeLDLTSolver::Solve() {
    matrix_t m;
    std::vector<double> b;
    std::vector<double> vAdd;
    output.indices.clear();
    for (unsg_t i = 0; i < nConstraints; ++i) {
        if (activeSet[i]) {
            vAdd = M[i];
            vAdd.push_back(s[i]);
            m.push_back(vAdd);
            b.push_back(-gamma * s[i]);
            output.indices.push_back(i);
        }
    }
    if (m.size() > 0) {
        MMTbSolver mmtb;
        int nDNegative = mmtb.Solve(m, b);
        output.nDNegative = nDNegative;
        output.solution = mmtb.GetSolution();
    } else {
        output.solution = std::vector<double>(nConstraints, 0.0);
    }
    return output;
}

const LinSolverOutput& CumulativeEGNSolver::Solve() {
    if (nActive  == 0) {
        output.indices.clear();
        output.solution =  std::vector<double>(nConstraints, 0.0);
    } else {
        matrix_t A(nActive, std::vector<double>(nActive, 0.0));
        std::vector<double> b(nActive);
        unsg_t ii = 0;
        for (unsg_t i = 0; i < nConstraints; ++i) {
            if (activeSet[i]) {
                unsg_t jj = 0;
                for (unsg_t c = 0; c < nConstraints; ++c) {
                    if (activeSet[c]) {
                        A[ii][jj] = 0.0;
                        for (unsg_t j = 0; j < nVariables; ++j) {
                            A[ii][jj] += (M[ii][j] * M[c][j] + s[ii] * s[jj]);
                        }
                    }
                    ++jj;
                }
                b[ii] = -gamma * s[i];
                ++ii;
            }
        }
        SolveByEGN(A, b);
    }
    return output;
}

void CumulativeEGNSolver::SolveByEGN(const matrix_t& A, const std::vector<double>& b) {

}
} //namespace QP_NNLS
