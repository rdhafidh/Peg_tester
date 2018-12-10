#include "mainwindow.h"
#if defined(Q_OS_WIN) && !defined(MINGWBUILD)
#include "crash_handler.h"
#endif
#include <QApplication>
#include <QDir>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	#if defined(Q_OS_WIN) && !defined(MINGWBUILD)
    Breakpad::CrashHandler::instance()->Init(QDir::currentPath());
	#endif
    MainWindow w;
    w.show();
    
    return a.exec();
}
