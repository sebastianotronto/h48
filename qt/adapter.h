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
	int optimal;
};

class NissyAdapter : public QObject {
	Q_OBJECT
	QML_SINGLETON
	QML_ELEMENT
	
public:
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
	std::function<void(const char*)> writeLog;

	void initSolver(const std::string&);
	void startSolve(SolveOptions);
	bool loadSolverData(nissy::solver&);
};

#endif
