 #-------------------------------------------------
#
# Project created by QtCreator 2015-08-22T07:26:41
#
#-------------------------------------------------

QT       += core gui-private network
include (excel/build.pri)
include(xlsx/qtxlsx.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Skripsi_11500060
TEMPLATE = app
CONFIG +=   
DEFINES = UNICODE BIT32  
win32-g* {
  CONFIG += c++14
  QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-return-type
  DEFINES += MINGWBUILD
  LIBS += -lws2_32
}
linux-clang {
  QMAKE_CXXFLAGS += -std=c++14
}
linux-gcc* {
 QMAKE_CXXFLAGS += -std=c++14
}  
copylib=
contains (DEFINES, BIT32){
    win32-msvc* {
    RC_FILE= res.rc
    target.path= D:\masteraplikasi\transferh11nov\qprocess
	CONFIG(debug,debug|release){
    copylib.commands= windeployqt --debug --compiler-runtime -webkitwidgets -printsupport -concurrent D:\masteraplikasi\transferh11nov\qprocess\Skripsi_11500060.exe && "copy /y $$BOTANDIR\debug\lib\botan.dll D:\masteraplikasi\transferh11nov\qprocess"
    }
	CONFIG(release,debug|release){
	copylib.commands= windeployqt --release --compiler-runtime -webkitwidgets -printsupport -concurrent D:\masteraplikasi\transferh11nov\qprocess\Skripsi_11500060.exe && "copy /y $$BOTANDIR\release\lib\botan.dll D:\masteraplikasi\transferh11nov\qprocess"
	}
	QMAKE_EXTRA_TARGETS+=copylib
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
RC_FILE =kommas.rc

win32-msvc*{
#	DEFINES += BUILD_WEBKIT
BREAKPAD = D:\masteraplikasi\transferh11nov\breakpadgit\breakpad
 HEADERS += $$BREAKPAD/src/common/windows/string_utils-inl.h
 HEADERS += $$BREAKPAD/src/common/windows/guid_string.h
 HEADERS += $$BREAKPAD/src/client/windows/handler/exception_handler.h
 HEADERS += $$BREAKPAD/src/client/windows/common/ipc_protocol.h
 HEADERS += $$BREAKPAD/src/google_breakpad/common/minidump_format.h
 HEADERS += $$BREAKPAD/src/google_breakpad/common/breakpad_types.h
 HEADERS += $$BREAKPAD/src/client/windows/crash_generation/crash_generation_client.h
 HEADERS += $$BREAKPAD/src/common/scoped_ptr.h
 SOURCES += $$BREAKPAD/src/client/windows/handler/exception_handler.cc
 SOURCES += $$BREAKPAD/src/common/windows/string_utils.cc
 SOURCES += $$BREAKPAD/src/common/windows/guid_string.cc
 SOURCES += $$BREAKPAD/src/client/windows/crash_generation/crash_generation_client.cc
INCLUDEPATH += $$BREAKPAD $$BREAKPAD/src $$PWD
DEPENDPATH += $$BREAKPAD $$BREAKPAD/src
    SOURCES +=crash_handler.cpp 
    HEADERS += crash_handler.h
}

contains(DEFINES, BUILD_WEBKIT){
	message("Membangun dokumentsi engine webkit...")
	QT += webkitwidgets
	SOURCES += webviewevent.cpp \
				helpdialog.cpp 
				
	HEADERS += helpdialog.h \
				webviewevent.h 
	FORMS += helpdialog.ui

}

SOURCES += main.cpp\
        mainwindow.cpp \
    settingprogram.cpp \
    settinghandler.cpp  \
    parserlintwrapper.cpp xlswriter.cpp \  
    benchmarkhandler.cpp \
    jsonsocketparserworkaround.cpp \
    lastfiledialogfolderdir.cpp

HEADERS  += mainwindow.h \
    settingprogram.h \
    settinghandler.h  \
    parserlintwrapper.h xlswriter.h parserdef.h \
    benchmarkhandler.h \
    jsonsocketparserworkaround.h \
    lastfiledialogfolderdir.h
    

FORMS    += mainwindow.ui \
    settingprogram.ui 

 
INSTALLS = target 

RESOURCES += \
    res.qrc \
    icons.qrc

DISTFILES += \
    res.rc 