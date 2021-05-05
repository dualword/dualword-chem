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

#include "DualWordChem.h"
#include "MainWindow.h"

DualWordChem::DualWordChem(int &argc, char **argv) : QApplication(argc, argv) {
	QCoreApplication::setApplicationName(QString(_DUALWORD));

	#ifdef _VER
		QApplication::setApplicationVersion(_VER);
	#endif

	QApplication::setQuitOnLastWindowClosed(true);

}

DualWordChem::~DualWordChem() {

}

void DualWordChem::start() {
	win.reset(new MainWindow());
    win->show();
}
