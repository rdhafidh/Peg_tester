#include "mainwindow.h"
#ifdef Q_OS_WIN
#include "crash_handler.h"
#endif
#include <QApplication>
#include <QDir>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	#ifdef Q_OS_WIN
 //   Breakpad::CrashHandler::instance()->Init(QDir::currentPath());
	#endif
    MainWindow w;
    w.show();
    
    return a.exec();
}
