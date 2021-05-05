include("dualword-chem.version.pri")

TARGET = dualword-chem
TEMPLATE = app
CONFIG += qt thread console release
QT += svg

DEPENDPATH += .
INCLUDEPATH += .

QMAKE_CXXFLAGS += $$CFLAGS $$CPPFLAGS $$CXXFLAGS
QMAKE_LFLAGS += $$LDFLAGS
QMAKE_CXXFLAGS += -std=c++11

#RDBASE=

QMAKE_CXXFLAGS += -I$${RDBASE}/Code

LIBS += -L$${RDBASE}/build/Code/GraphMol/SmilesParse/ -lSmilesParse_static
LIBS += -L$${RDBASE}/build/Code/GraphMol/FileParsers/ -lFileParsers_static
LIBS += -L$${RDBASE}/build/Code/GraphMol/Descriptors/ -lDescriptors_static
LIBS += -L$${RDBASE}/build/Code/GraphMol/Depictor/ -lDepictor_static
LIBS += -L$${RDBASE}/build/Code/GraphMol/Substruct/ -lSubstructMatch_static
LIBS += -L$${RDBASE}/build/Code/GraphMol/PartialCharges/ -lPartialCharges_static
LIBS += -L$${RDBASE}/build/Code/GraphMol/Subgraphs/ -lSubgraphs_static
LIBS += -L$${RDBASE}/build/Code/GraphMol/ -lGraphMol_static
LIBS += -L$${RDBASE}/build/Code/GraphMol/MolDraw2D/ -lMolDraw2D_static
LIBS += -L$${RDBASE}/build/Code/DataStructs/ -lDataStructs_static
LIBS += -L$${RDBASE}/build/Code/Geometry/ -lRDGeometryLib_static
LIBS += -L$${RDBASE}/build/Code/RDGeneral/ -lRDGeneral_static
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
