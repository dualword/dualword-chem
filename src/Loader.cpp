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

#include "Loader.h"

#include <RDGeneral/types.h>
#include <GraphMol/RDKitBase.h>
#include <GraphMol/MolOps.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/FileParsers/MolSupplier.h>
#include <GraphMol/Depictor/RDDepictor.h>
#include <GraphMol/MolDraw2D/MolDraw2DSVG.h>
#include <GraphMol/Descriptors/MolDescriptors.h>
#include <GraphMol/FileParsers/MolFileStereochem.h>

Loader::Loader(const QString& name, QObject *p) : QThread(p), stop(false){
	this->name = name;
}

Loader::~Loader() {

}

void Loader::run(){
	emit showMessage(" Loading:" + name, 0);
	int i = 0, count = 0, err = 0;
	SDMolSupplier suppl(name.toStdString());
	 while (!stop && !suppl.atEnd()) {
		try{
			QSharedPointer<ROMol> m(suppl.next());
			++count;
			if(!m.data()){
				++err;
				continue;
			}
			emit loadMolecule(m);
			++i;
		} catch (const MolSanitizeException& e) {
			++err;
			continue;
		}
	}
	 //+" Err:"+QString::number(err) + " Loaded:" +	QString::number(i)
	 emit showMessage("Total:"+QString::number(count) + " File:" + name, 5000);
}
