#include "log.h"
namespace QP_NNLS {

	std::ostream& operator << (std::ostream& f, const matrix_t& mat) {
		#ifdef CPP_FORMAT 
		f << "{";
		#endif
		for (const auto& row : mat) {
			f << row;
			#ifdef CPP_FORMAT
	        if (&row != &mat.back()) {
				f << ",";
			}
			#endif
		}
		#ifdef CPP_FORMAT 
		f << "}\n";
		#endif

		return f;
	}

	bool Logger::SetFile(const std::string& fileName) {
		logFile = fileName;
		fid.open(logFile, std::ios::out);
		return fid.is_open();
	}

	void Logger::SetStage(const std::string& stageName) {
		const char* stageSep = "--------------";
		fid << "\n" << stageSep << stageName << stageSep << "\n";
	}
	void Logger::ToNewLine() {
		fid << "\n";
	}
	void Logger::PrintActiveSetIndices(const std::vector<int>& indices) {
		const int n = static_cast<int>(indices.size());
		fid << "active set indices: ";
		for (int indx = 0; indx < n; ++indx) {
			if (indices[indx] == 1) {
				fid << SEP << indx;
			}
		}
		fid << "\n";
	}

    Callback1::Callback1(const std::string& filePath):
        logger(std::make_unique<Logger>())
    {
        logger->SetFile(filePath);
    }


    void Callback1::ProcessData(int stage) {
        if (stage == 1) { // dump data after init stage
            logger->SetStage("INITIALIZATION");
            logger->dump("Choletsky", initData.Chol);
            logger->dump("CholetskyInv", initData.CholInv);
            logger->dump("Matrix M", initData.M);
            logger->dump("vector s", initData.s);
            logger->dump("vector c", initData.c);
            logger->dump("vector b", initData.b);
            logger->message("t Chol", initData.tChol);
            logger->message("t Inv", initData.tInv);
            logger->message("t M", initData.tM);
        } else if (stage == 2) { // dump iteration data
            logger->message("ITERATION", iterData.iteration);
            logger->dump("active set", *iterData.activeSet);
            logger->dump("primal", *iterData.primal);
            logger->dump("dual", *iterData.dual);
            logger->message("new active component", iterData.newIndex,
                            "isSingular", iterData.singular ? 1 : 0);
        }
    }


}
