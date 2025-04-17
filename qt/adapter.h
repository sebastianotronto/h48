#ifndef ADAPTER_H
#define ADAPTER_H

#include "../cpp/nissy.h"

#include <map>
#include <string>
#include <QObject>
#include <QtQmlIntegration>

struct SolveOptions {
	nissy::cube cube;
	nissy::solver *solver;
	unsigned minmoves;
	unsigned maxmoves;
	unsigned maxsolutions;
	unsigned optimal;
};

class NissyAdapter : public QObject {
	Q_OBJECT
	QML_SINGLETON
	QML_ELEMENT
	
public:
	static constexpr int maxSolutionsHardLimit = 9999;

	NissyAdapter();

	Q_INVOKABLE bool isValidScramble(QString);
	Q_INVOKABLE void requestSolve(
		QString scramble,
		QString solver,
		int minmoves,
		int maxmoves,
		int maxsolutions,
		int optimal
	);

signals:
	void solutionsReady(QString, QString);
	void solverError(QString);
	void appendLog(QString);

private:
	std::vector<nissy::solver> solvers;
	std::function<void(std::string)> writeLog;

	void initSolver(const std::string&);
	void startSolve(SolveOptions);
	bool loadSolverData(nissy::solver&);
	void logLine(std::string);
};

#endif
