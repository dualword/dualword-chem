include("dualword-chem.version.pri")

TARGET = dualword-chem
TEMPLATE = app
CONFIG += qt thread console release
QT += widgets svgwidgets
DEPENDPATH += .
INCLUDEPATH += .
QMAKE_CXXFLAGS += -std=c++17

QMAKE_CXXFLAGS +=
LIBS +=
LIBS += -lRDKitRDGeometryLib -lRDKitSubstructMatch -lRDKitFingerprints -lRDKitAlignment
LIBS += -lRDKitMolAlign -lRDKitFileParsers -lRDKitChemReactions -lRDKitFilterCatalog -lRDKitDepictor
LIBS += -lRDKitMolTransforms -lRDKitPartialCharges -lRDKitChemTransforms -lRDKitDescriptors  -lRDKitCatalogs
LIBS += -lRDKitRDGeneral -lRDKitSubstructLibrary -lRDKitSubgraphs -lRDKitGraphMol -lRDKitDistGeometry -lRDKitDistGeomHelpers
LIBS += -lRDKitForceField -lRDKitForceFieldHelpers -lRDKitEigenSolvers -lRDKitDataStructs -lRDKitSmilesParse
LIBS += -lRDKitTrajectory -lRDKitOptimizer -lRDKitMolDraw2D
LIBS += -lboost_regex -lboost_system -lboost_thread

HEADERS = \
	MainWindow.h \
	DualWordChem.h \
	Loader.h
		
SOURCES = \
	MainWindow.cpp \
	DualWordChem.cpp \
	Loader.cpp \
	main.cpp

FORMS += \
	MainWindow.ui
							
OBJECTS_DIR = .build/obj
MOC_DIR     = .build/moc
RCC_DIR     = .build/rcc
UI_DIR      = .build/ui
