#include "adapter.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

void logWrapper(const char *str, void *data)
{
	auto f = *reinterpret_cast<std::function<void(const char*)>*>(data);
	f(str);
}

NissyAdapter::NissyAdapter()
{
	std::vector<std::string> solverNames {
		"h48h3k2"
	};

	for (auto s : solverNames)
		initSolver(s);

	writeLog = [&](const char *str) {
		emit appendLog(QString::fromStdString(str));
	};

	nissy::set_logger(&logWrapper, &writeLog);
}

void NissyAdapter::initSolver(const std::string& s) {
	auto se = nissy::solver::get(s);
	if (std::holds_alternative<nissy::error>(se)) {
		qDebug("Error loading solver!");
		return;
	}
	auto ss = std::get<nissy::solver>(se);
	solvers.push_back(ss);
}

bool NissyAdapter::loadSolverData(nissy::solver& solver) {
	if (solver.data_checked)
		return true;

	std::filesystem::path filePath("./tables/" + solver.id);
	if (!std::filesystem::exists(filePath)) {
		auto err = solver.generate_data();
		if (!err.ok()) {
			emit solverError(QString("Error generating data!"));
			return false;
		}
		std::filesystem::create_directory("./tables/");
		std::ofstream ofs(filePath, std::ios::binary);
		ofs.write(reinterpret_cast<char *>(solver.data.data()),
		    solver.size);
		ofs.close();
	} else {
		std::ifstream ifs(filePath, std::ios::binary);
		solver.read_data(ifs);
		ifs.close();
	}

	if (!solver.check_data().ok()) {
		emit solverError(QString("Error reading data!"));
		return false;
	}

	return true;
}

Q_INVOKABLE bool NissyAdapter::isValidScramble(QString qscr)
{
	nissy::cube c;
	return c.move(qscr.toStdString()).ok();
}

Q_INVOKABLE void NissyAdapter::requestSolve(
	QString scramble,
	QString solver,
	int minmoves,
	int maxmoves,
	int maxsolutions,
	int optimal
)
{
	nissy::cube c;
	if (!c.move(scramble.toStdString()).ok()) {
		emit solverError(QString("Unexpected error: invalid scramble"));
		return;
	}

	nissy::solver *ss = nullptr;
	for (auto& s : solvers)
		if (s.name == solver)
			ss = &s;
	if (ss == nullptr) {
		std::string msg = "Error: solver '" + solver.toStdString()
		    + "' not available";
		emit solverError(QString::fromStdString(msg));
		return;
	}

	SolveOptions opts{c, ss, (unsigned)minmoves, (unsigned)maxmoves,
	    (unsigned)maxsolutions, optimal};
	auto _ = QtConcurrent::run(&NissyAdapter::startSolve, this, opts);
	return;
}

void NissyAdapter::startSolve(SolveOptions opts)
{
	loadSolverData(*opts.solver);

	auto result = opts.solver->solve(opts.cube, nissy::nissflag::NORMAL,
	    opts.minmoves, opts.maxmoves, opts.maxsolutions, opts.optimal, 8);

	if (!result.err.ok()) {
		std::string msg = "Error computing solutions: " +
		    std::to_string(result.err.value);
		emit solverError(QString::fromStdString(msg));
		return;
	}

	auto& sols = result.solutions;
	if (sols.size() == 0) {
		emit solutionsReady("No solution found", "");
	} else {
		std::stringstream hs;
		hs << "Found " << sols.size() << " solution"
		    << (sols.size() > 1 ? "s:" : ":");

		std::stringstream ss;
		for (auto s : sols) {
			auto n = nissy::count_moves(s).value;
			ss << s << "(" << n << ")" << std::endl; // TODO: remove last newline
		}
		emit solutionsReady(QString::fromStdString(hs.str()),
		   QString::fromStdString(ss.str()));
	}
}
