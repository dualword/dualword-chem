/*
 *	Copyright (C) 2019 Alexander Busorgin
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

#ifndef SRC_LOADER_H_
#define SRC_LOADER_H_

#include <QtCore>

namespace RDKit {class ROMol;}
using namespace RDKit;

class Loader : public QThread{
	Q_OBJECT

public:
	Loader(const QString&, QObject *p = 0);
	virtual ~Loader();

	void setStop(bool b) {
		this->stop = b;
	}

signals:
	void loadMolecule(const QSharedPointer<ROMol>&);
	void showMessage(const QString&, int);
	void errMessage(const QString&);

protected:
     void run();

private:
     bool stop;
     QString name;
};

#endif /* SRC_LOADER_H_ */
