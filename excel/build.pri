
INCLUDEPATH += $$PWD/src
INCLUDEPATH += .

win32*{
BOTANDIR=D:\masteraplikasi\transferh11nov\botangit\msvc
CONFIG(debug,debug|release){
    INCLUDEPATH +=  $$BOTANDIR/debug/include
    LIBS += $$BOTANDIR/debug/lib/botan.lib
}else{
    INCLUDEPATH +=  $$BOTANDIR/release/include
    LIBS += $$BOTANDIR/release/lib/botan.lib
}

DEFINES += ASKDEBUG
}

SOURCES =  $$PWD/src/evalbase.cpp \
			$$PWD/src/Cell.cpp \
			$$PWD/src/Commands.cpp \
			$$PWD/src/Graphic.cpp	\
			$$PWD/src/ItemDelegate.cpp \
			$$PWD/src/excelMainWindow.cpp \
			$$PWD/src/PictureGraphic.cpp \
			$$PWD/src/Table.cpp	\
			$$PWD/src/Dialogs/FindDialog.cpp \
			$$PWD/src/Dialogs/FunctionListDialog.cpp \
			$$PWD/src/Dialogs/GraphicPropertiesDialog.cpp \
			$$PWD/src/Dialogs/InsertRCDialog.cpp \
			$$PWD/src/Dialogs/MySpinBox.cpp \
			$$PWD/src/Dialogs/NewFileDialog.cpp \
			$$PWD/src/Dialogs/SortDialog.cpp \
                        $$PWD/src/calculatorpeg.cpp

FORMS += $$PWD/src/Dialogs/FindDialog.ui \
            $$PWD/src/Dialogs/FunctionListDialog.ui  \
            $$PWD/src/Dialogs/GraphicPropertiesDialog.ui \
             $$PWD/src/Dialogs/InsertRCDialog.ui \
             $$PWD/src/Dialogs/NewFileDialog.ui \
            $$PWD/src/Dialogs/SortDialog.ui 
            
INCLUDEPATH += src $$PWD/src/Dialogs

HEADERS = $$PWD/src/testoverflow.h \
                $$PWD/src\evalbase.h \
               $$PWD/src/Cell.h \
               $$PWD/src/Commands.h \
               $$PWD/src/Graphic.h \
               $$PWD/src/ItemDelegate.h \
               $$PWD/src/excelMainWindow.h \
               $$PWD/src/PictureGraphic.h  \
               $$PWD/src/QSimplesheet.h  \
               $$PWD/src/Table.h  \
               $$PWD/src/Dialogs/FindDialog.h  \
               $$PWD/src/Dialogs/FunctionListDialog.h  \
               $$PWD/src/Dialogs/GraphicPropertiesDialog.h  \
               $$PWD/src/Dialogs/InsertRCDialog.h  \
               $$PWD/src/Dialogs/MySpinBox.h  \
               $$PWD/src/Dialogs/NewFileDialog.h  \
               $$PWD/src/Dialogs/SortDialog.h \  
    $$PWD/src/calculatorpeg.h
RESOURCES += \ 
		$$PWD/src/resources.qrc
