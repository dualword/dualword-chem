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

#include "MainWindow.h"
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
#include <GraphMol/Fingerprints/Fingerprints.h>
#include <DataStructs/BitOps.h>

Q_DECLARE_METATYPE(QSharedPointer<ROMol>)

template <typename T>
Mol<T>::Mol(const QString& s, T* rm) : smiles(s), m(rm) {}

template <typename T>
Mol<T>::~Mol() {}

template <typename T>
QString Mol<T>::getSvg(int w, int h){
	RWMol m(static_cast<T>(*this->m));
	MolOps::Kekulize(m);
	RDDepict::compute2DCoords(m);
	MolDraw2DSVG s(w,h);
	s.drawMolecule(m);
	s.finishDrawing();
	return QString::fromStdString(s.getDrawingText());
}

MainWindow::MainWindow(QWidget *p, Qt::WindowFlags f) : QMainWindow(p, f), idx(0), mol(0), loader(nullptr),
		molSim(nullptr) {

	setupUi(this);
	qRegisterMetaType<QSharedPointer<ROMol>>("ROMol");
	setWindowTitle(qApp->applicationName());
	lbl->setFixedWidth(100);
	smiles->hide();
	svg->setFocusPolicy(Qt::StrongFocus);
	actionImportSDF->setIcon(style()->standardIcon(QStyle::SP_FileDialogStart, 0, this));
	actionImportSmiles->setIcon(style()->standardIcon(QStyle::SP_FileIcon, 0, this));
	actionSmiles->setIcon(style()->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton, 0, this));
	actionConsole->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView, 0, this));
	actSim->setIcon(style()->standardIcon(QStyle::SP_FileLinkIcon, 0, this));
	console->setVisible(false);
	actionDeleteAll->setIcon(style()->standardIcon(QStyle::SP_TrashIcon, 0, this));
	actionAbout->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView, 0, this));
    connect(actionImportSDF,SIGNAL(triggered()), SLOT(importSDF()));
    connect(actionImportSmiles,SIGNAL(triggered()), SLOT(importSmiles()));
    connect(actionSmiles,SIGNAL(toggled(bool)), smiles, SLOT(setVisible(bool)));
    connect(actionDeleteAll,SIGNAL(triggered()), SLOT(deleteAll()));
    connect(actionAbout,SIGNAL(triggered()), SLOT(showAbout()));
    connect(actionConsole,SIGNAL(toggled(bool)), console, SLOT(setVisible(bool)));
    connect(actSim,SIGNAL(triggered()), SLOT(setMol()));
	QStringList list;
	list.push_back("Property");
	list.push_back("Value");
	tbl->setHorizontalHeaderLabels(list);
	tbl->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	connect(bBack,SIGNAL(clicked()), SLOT(back()));
	connect(bNext,SIGNAL(clicked()), SLOT(next()));
	connect(bFirst,SIGNAL(clicked()), SLOT(first()));
	connect(bLast,SIGNAL(clicked()), SLOT(last()));
	menu = new QMenu(this);
	QMenu *m = menu->addMenu("Import");
	QAction *a = new QAction("SMILES",menu);
	m->addAction(a);
	connect(a,SIGNAL(triggered()), SLOT(importSmiles()));
	a = new QAction("SDF",menu);
	m->addAction(a);
	connect(a,SIGNAL(triggered()), SLOT(importSDF()));
	a = new QAction("Save SVG",menu);
	menu->addAction(a);
	connect(a,SIGNAL(triggered()), SLOT(saveMol()));
	a = new QAction("Delete",menu);
	menu->addAction(a);
	connect(a,SIGNAL(triggered()), SLOT(deleteMol()));
	menu->addSeparator();
	a = new QAction("About",menu);
	menu->addAction(a);
	connect(a,SIGNAL(triggered()), SLOT(showAbout()));
	svg->installEventFilter(this);
	connect(svg,SIGNAL(customContextMenuRequested(const QPoint&)),
			SLOT(contextMenuRequested(const QPoint&)));
}

MainWindow::~MainWindow() {}

void MainWindow::contextMenuRequested(const QPoint& p){
	menu->popup(mapToGlobal(p));
}

void MainWindow::resizeEvent (QResizeEvent *e){
	Q_UNUSED(e);
	showMol();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* key = static_cast<QKeyEvent*>(event);
		if ( (key->key()==Qt::Key_Right)) {
			next();
		}else if ( (key->key()==Qt::Key_Left)) {
			back();
		} else {
			return QObject::eventFilter(obj, event);
		}
		return true;
	} else {
		return QObject::eventFilter(obj, event);
	}
	return false;
}

void MainWindow::saveMol(){
	if(!mol) return;
	QFileDialog d(this,tr("Choose directory"), QDir::homePath());
	d.setFilter(QDir::Dirs | QDir::Hidden);
	d.setFileMode(QFileDialog::DirectoryOnly);
	d.setViewMode(QFileDialog::List);
	d.setOption(QFileDialog::ShowDirsOnly,true);
	d.setOption(QFileDialog::ReadOnly,true);

    if (d.exec() == QDialog::Accepted) {
        QString dir = QString(d.selectedFiles().value(0));
        QString fname = dir.append(QDir::separator()).append("svg.svg");
        statusBar()->showMessage("SVG saved in " + fname,5000);
        QFile f(fname);
        f.open(QIODevice::WriteOnly);
      	f.write(mol->getSvg(scrollArea->rect().width(),scrollArea->rect().height()).toUtf8());
      	f.flush();
      	f.close();
    }
}

void MainWindow::showMol(){
	if(!mol) return;
	svg->load(mol->getSvg(scrollArea->rect().width(),scrollArea->rect().height()).toUtf8());
	tbl->clearContents();
	tbl->setRowCount(0);
	QScopedPointer<ROMol> m(MolOps::addHs(*mol->getMol()));
	double logp {0.0}, mr {0.0}, mw = Descriptors::calcAMW(*mol->getMol());
	Descriptors::calcCrippenDescriptors(*mol->getMol(), logp, mr);
	createRow("Formula",QString::fromStdString(Descriptors::calcMolFormula(*mol->getMol())),true);
	addElements(m.data(), mw);
	createRow("Atoms",QString::number(m->getNumAtoms()));
	createRow("Heavy Atoms",QString::number(m->getNumHeavyAtoms()));
	createRow("Heteroatoms",QString::number(Descriptors::calcNumHeteroatoms(*mol->getMol())));
	createRow("Bonds",QString::number(m->getNumBonds()));
	createRow("Amide Bonds",QString::number(Descriptors::calcNumAmideBonds(*mol->getMol())));
	createRow("Rotatable Bonds",QString::number(Descriptors::calcNumRotatableBonds(*mol->getMol())));
	createRow("Molecular weight",QString::number(mw));
	createRow("Monoisotopic",QString::number(Descriptors::calcExactMW(*mol->getMol())));
	createRow("TPSA",QString::number(Descriptors::calcTPSA(*mol->getMol())));
	createRow("LogP",QString::number(logp));
	createRow("HBA",QString::number(Descriptors::calcNumHBA(*mol->getMol())));
	createRow("HBD",QString::number(Descriptors::calcNumHBD(*mol->getMol())));
	createRow("Lipinski HBA",QString::number(Descriptors::calcLipinskiHBA(*mol->getMol())));
	createRow("Lipinski HBD",QString::number(Descriptors::calcLipinskiHBD(*mol->getMol())));
	createRow("Rings",QString::number(Descriptors::calcNumRings(*mol->getMol())), true);
	createRow("Aromatic Rings",QString::number(Descriptors::calcNumAromaticRings(*mol->getMol())), true);
	createRow("Aliphatic Rings",QString::number(Descriptors::calcNumAliphaticRings(*mol->getMol())), true);
	createRow("Saturated Rings",QString::number(Descriptors::calcNumSaturatedRings(*mol->getMol())), true);
	createRow("Heterocycles",QString::number(Descriptors::calcNumHeterocycles(*mol->getMol())),true);
	createRow("Aromatic Heterocycles",QString::number(Descriptors::calcNumAromaticHeterocycles(*mol->getMol())), true);
	createRow("Aliphatic Heterocycles",QString::number(Descriptors::calcNumAliphaticHeterocycles(*mol->getMol())), true);
	createRow("Saturated Heterocycles",QString::number(Descriptors::calcNumSaturatedHeterocycles(*mol->getMol())), true);
	createRow("Aromatic Carbocycles",QString::number(Descriptors::calcNumAromaticCarbocycles(*mol->getMol())), true);
	createRow("Aliphatic Carbocycles",QString::number(Descriptors::calcNumAliphaticCarbocycles(*mol->getMol())), true);
	createRow("Saturated Carbocycles",QString::number(Descriptors::calcNumSaturatedCarbocycles(*mol->getMol())), true);
	smiles->setText(mol->getSmiles());
}

void MainWindow::createRow(const QString& p, const QString& v, bool c){
	tbl->insertRow(tbl->rowCount());
	int row = tbl->rowCount()-1;
	QTableWidgetItem *item = new QTableWidgetItem(tr("%1").arg(p));
	if (c) item->setBackground(QBrush(QColor(153,255,255)));
	tbl->setItem(row, 0, item);
	item = new QTableWidgetItem(tr("%1").arg(v));
	if (c) item->setBackground(QBrush(QColor(153,255,255)));
	tbl->setItem(row, 1, item);
}

void MainWindow::showAbout(){
	QString str;
	str.append(qApp->applicationName());
	str.append(" ").append(qApp->applicationVersion()).append("<br>");
	str.append("License: GPL v3 <br/>");
	str.append("Website: <a href='http://github.com/dualword/dualword-chem'>Dualword-chem</a> <br/>");
	str.append("&copy;2021 Alexander Busorgin <br/>");
	QMessageBox::about(this, tr("About"), str );
}

void MainWindow::importSmiles(){
	bool ok;
	QString text = QInputDialog::getText(this, 0, tr("Enter SMILES"), QLineEdit::Normal, "", &ok);

	if (ok && !text.isEmpty()){
		try{
			QScopedPointer<RWMol> mol(SmilesToMol(text.toStdString()));
			QSharedPointer<Mol<ROMol>> m(new Mol<ROMol>(text, mol.take()));
			if(!m->getMol()){
				QMessageBox::critical(this, tr("Error"), "Not valid SMILES string." );
				return;
			}
			list.push_back(m);
			idx = list.size()-1;
			refresh();
		} catch (const MolSanitizeException& e) {
			QMessageBox::critical(this, tr("MolSanitizeException"), QString(e.message()) );
			return;
		}
	}
}

void MainWindow::setMol(){
	bool ok;
	QString text = QInputDialog::getText(this, 0, tr("Enter SMILES"), QLineEdit::Normal, "", &ok);

	if (ok && !text.isEmpty()){
		try{
			QScopedPointer<RWMol> mol(SmilesToMol(text.toStdString()));
			molSim.reset(new Mol<ROMol>(text, mol.take()));
			if(!molSim->getMol()){
				QMessageBox::critical(this, tr("Error"), "Not valid SMILES string." );
				return;
			}
			svg2->load(molSim->getSvg(scrollArea2->rect().width(),scrollArea2->rect().height()).toUtf8());
		} catch (const MolSanitizeException& e) {
			QMessageBox::critical(this, tr("MolSanitizeException"), QString(e.message()) );
			return;
		}
	}
}

void MainWindow::importSDF(){
	QFileDialog d(this,tr("Choose file"), QDir::homePath());
	d.setFilter(QDir::AllEntries | QDir::Hidden);
	d.setFileMode(QFileDialog::ExistingFile);
	d.setViewMode(QFileDialog::List);
	d.setOption(QFileDialog::ReadOnly,true);
    if (d.exec() == QDialog::Accepted) {
        QString fname = QString(d.selectedFiles().value(0));
        if(loader.isNull() || !loader->isRunning()){
        	loader.reset(new Loader(fname));
        	connect(loader.data(),SIGNAL(finished()), SLOT(back()));
        	connect(loader.data(),SIGNAL(loadMolecule(const  QSharedPointer<ROMol>&)),
        			SLOT(loadMolecule(const QSharedPointer<ROMol>&)));
        	connect(loader.data(),SIGNAL(showMessage(const QString&,int)),
        			statusbar, SLOT(showMessage(const QString&,int)));
        	connect(loader.data(),SIGNAL(errMessage(const QString&)), SLOT(log(const QString&)));
        	loader->start();
        }
    }
}

void MainWindow::back(){
	if(list.size() == 0) return;
	if(--idx<0) idx = 0;
	refresh();
}

void MainWindow::next(){
	if(list.size() == 0) return;
	if(++idx>list.size()-1) idx = list.size()-1;
	refresh();
}

void MainWindow::first(){
	if(list.size() == 0) return;
	idx = 0;
	refresh();
}

void MainWindow::last(){
	if(list.size() == 0) return;
	idx = list.size()-1;
	refresh();
}

void MainWindow::refresh(){
	lbl->setText(QString::number(idx+1)+ "/" +  QString::number(list.size()));
	mol = list.at(idx).data();
	showMol();

	if(molSim.isNull() || !molSim->getMol()) return;
	ExplicitBitVect* fp1 = RDKit::RDKFingerprintMol(*molSim->getMol());
	ExplicitBitVect* fp2 = RDKit::RDKFingerprintMol(*mol->getMol());
	lblSim->setText(QString::number(TanimotoSimilarity(*fp1, *fp2),'g',3));
}

void MainWindow::deleteMol(){
	if(list.size() > 0) list.removeAt(idx);
	if(list.isEmpty() ){
		mol = nullptr;
		svg->load(QByteArray(""));
		tbl->clearContents();
		tbl->setRowCount(0);
		lbl->clear();
		smiles->clear();
	}
	back();
}

void MainWindow::deleteAll(){
	list.clear();
	idx = 0;
	deleteMol();
}

void MainWindow::loadMolecule(const QSharedPointer<ROMol>& p){
	try{
		QString s = QString::fromStdString(MolToSmiles(*p));
		QScopedPointer<RWMol> rwm(SmilesToMol(s.toStdString()));
		QSharedPointer<Mol<ROMol>> m(new Mol<ROMol>(s, new ROMol(*p.data())));
		if(!m->getMol()) return;
		list.push_back(m);
		lbl->setText(QString::number(idx++)+ "/" +  QString::number(list.size()));
	}catch (const MolSanitizeException& e) {
		//
	}
}

void MainWindow::addElements(const ROMol* m, const double& mw){
	QMap<QString, double> map, m2;
	for(auto it = m->beginAtoms(); it != m->endAtoms(); ++it){
		map.insert((*it)->getSymbol().c_str(),
				map.value((*it)->getSymbol().c_str()) + (*it)->getMass());
		m2.insert((*it)->getSymbol().c_str(), m2.value((*it)->getSymbol().c_str()) + 1);
	}
	QMapIterator<QString, double> i(map);
	while (i.hasNext()){
	     i.next();
	     createRow(i.key()+" - "+QString::number(m2.value(i.key())),
	    		 QString::number(i.value()).append(" (")
				 .append(QString::number(i.value()/mw*100)).append(" %)"), true);
	 }
}

void MainWindow::log(const QString& str){
	console->appendPlainText(str);
}
