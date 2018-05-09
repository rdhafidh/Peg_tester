QT += core network gui-private
QT += testlib
CONFIG +=release 
#CONFIG += console
CONFIG -= app_bundle
DEFINES = 32BIT BUILDNOEXCELUI
include (xlsx/qtxlsx.pri)
win32*{
BOTANDIR=D:\masteraplikasi\transferh11nov\botangit\msvc
CONFIG(debug,debug|release){
    INCLUDEPATH +=  $$BOTANDIR/debug/include
    LIBS += $$BOTANDIR/debug/lib/botan.lib
}else{
    INCLUDEPATH +=  $$BOTANDIR/release/include
    LIBS += $$BOTANDIR/release/lib/botan.lib
}
}

DEPLOY=
win32{
INCLUDEPATH += D:\masteraplikasi\transferh11nov\rapidjsongit\rapidjson\include
}
include (csvlib.pri)
INCLUDEPATH += .
win32-g++{
  QMAKE_CXXFLAGS += -std=c++14
}
linux-clang {
  QMAKE_CXXFLAGS += -std=c++14
}
linux-gcc* {
 QMAKE_CXXFLAGS += -std=c++14
}

TEMPLATE = app

SOURCES += main.cpp \
    parserlint.cpp \
    benchmarkmanager.cpp \
    subsetexceltestsuite.cpp evalbase.cpp calculatorpeg.cpp

HEADERS += parserlint.h \
    parserdef.h \
    benchmarkmanager.h \
    subsetexceltestsuite.h evalbase.h calculatorpeg.h testoverflow.h

contains (DEFINES, 32BIT){
    win32-msvc*{
	CONFIG(debug,debug|release){
	DEPLOY.commands= windeployqt --debug --compiler-runtime -core -network D:\masteraplikasi\transferh11nov\qprocess\parserlint\32\vs\parserlint1_vs2015.exe && "copy /y $$BOTANDIR\debug\lib\botan.dll D:\masteraplikasi\transferh11nov\qprocess\parserlint\32\vs"
	}
	CONFIG(release,release|debug){
	DEPLOY.commands= windeployqt --release --compiler-runtime -core -network D:\masteraplikasi\transferh11nov\qprocess\parserlint\32\vs\parserlint1_vs2015.exe && "copy /y $$BOTANDIR\release\lib\botan.dll D:\masteraplikasi\transferh11nov\qprocess\parserlint\32\vs"
	}
	QMAKE_EXTRA_TARGETS+= DEPLOY
target.path=D:\masteraplikasi\transferh11nov\qprocess\parserlint\32\vs        
    }
    win32-g++{
target.path=D:\masteraplikasi\stuff\projectall\parserlint1\release\32\mingw
    }
    linux-clang{
    target.path =
    }
    linux-gcc*{
    target.path =
    }
}
contains (DEFINES, 64BIT){
    win32-msvc*{
target.path=D:\masteraplikasi\stuff\projectall\parserlint1\release\64\vs        
    }
    win32-g++{
target.path=D:\masteraplikasi\stuff\projectall\parserlint1\release\64\mingw
    }
    linux-clang{
    target.path =
    }
    linux-gcc*{
    target.path =
    }
}
win32-msvc*{
TARGET = parserlint1_vs2015
}
win32-g++{
TARGET = parserlint1_mingw
}
linux-clang{
    TARGET = parserlint1_clang
}
linux-gcc*{
    TARGET = parserlint1_gcc
}
INSTALLS = target

DISTFILES += \
    models/class_peg_action.qmodel

