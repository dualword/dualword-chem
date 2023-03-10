/*
 *	Copyright (C) 2016 Alexander Busorgin
 *
 *	This file is part of DualWord-chem.
 *
 *	DualWord-chem is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	DualWord-chem is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with DualWord-chem.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QtWidgets>
#include <QtSvg>

#include "ui_MainWindow.h"

namespace RDKit {class ROMol;}
using namespace RDKit;

class Loader;

template<typename T>
class Mol{
public:
	Mol(const QString&, T*);
	virtual ~Mol();
	QString getSvg(int w, int h);
	const T* getMol() const {return m.get();};
	const QString& getSmiles() const {return smiles;}

private:
	QString smiles;
	QScopedPointer<T> m;
};

class MainWindow : public QMainWindow, private Ui::MainWindowForm {
  Q_OBJECT

public:
	MainWindow(QWidget *p = 0, Qt::WindowFlags f = 0);
	virtual ~MainWindow();

protected:
	void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
	void contextMenuRequested(const QPoint&);
	void importSmiles();
	void importSimilarity();
	void importSDF();
	void showAbout();
	void showMol();
	void saveMol();
	void deleteMol();
	void back();
	void next();
	void first();
	void last();
	void deleteAll();
	void loadMolecule(const QSharedPointer<ROMol>&);
	void log(const QString&);

private:
	void refresh();
	void createRow(const QString&, const QString&, bool = false);
	void addElements(const ROMol*, const double&);

	QMenu *menu;
	QList<QSharedPointer<Mol<ROMol>>> list;
	QScopedPointer<Loader> loader;
	int idx;
	Mol<ROMol> *mol;
	QScopedPointer<Mol<ROMol>> molSim;

};

#endif /* MAINWINDOW_H_ */
