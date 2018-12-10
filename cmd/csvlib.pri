QTCSVDIR=
win32{
QTCSVDIR=D:\masteraplikasi\transferh11nov\qtcsv-master
HEADERS += $$QTCSVDIR/include/qtcsv/abstractdata.h \
            $$QTCSVDIR/include/qtcsv/qtcsv_global.h \
            $$QTCSVDIR/include/qtcsv/reader.h \
            $$QTCSVDIR/include/qtcsv/stringdata.h \
        $$QTCSVDIR/include/qtcsv/variantdata.h \
            $$QTCSVDIR/include/qtcsv/writer.h \
             $$QTCSVDIR/sources/contentiterator.h \
        $$QTCSVDIR/sources/filechecker.h \
        $$QTCSVDIR/sources/symbols.h

DEFINES += QTCSV_LIBRARY
INCLUDEPATH +=  $$QTCSVDIR $$QTCSVDIR/include $$QTCSVDIR/qtcsv $$QTCSVDIR/sources

SOURCES += $$QTCSVDIR/sources/writer.cpp \
    $$QTCSVDIR/sources/variantdata.cpp \
    $$QTCSVDIR/sources/stringdata.cpp \
    $$QTCSVDIR/sources/reader.cpp $$QTCSVDIR/sources/contentiterator.cpp

}