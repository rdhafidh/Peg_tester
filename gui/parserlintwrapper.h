#ifndef PARSERLINTWRAPPER_H
#define PARSERLINTWRAPPER_H

#include <QtCore>
#include <chrono>
               
class ParserLintWrapper:public QObject
{
    Q_OBJECT
public:
    ParserLintWrapper(QObject *parent=0);
    ~ParserLintWrapper(){ 
          mp->deleteLater();
    };
     void startParser(const QString &prog, const QStringList &arg, const QString &sat);
    QByteArray datas()const {return m_data;}
    QString lastError()const { return m_error; }
    double selama()const {return m_bench;}
private Q_SLOTS:
    void handleError(QProcess::ProcessError e);
    void handleOutput();
    void onStart();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onSyncGetError(const QString &err){
        m_error=err;
    }
    void saveBench(double b){
        m_bench=b;
    }

Q_SIGNALS:
    void emitError(const QString & err);
    void emitOutput( QString o); //SEND BY VALUE AND MOVE ITERATOR SHOULD BE USED
    void emitFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void emitStart();
    void emitBench(double a);
private: 
    QProcess *mp;
    QByteArray m_data;
    QString m_error;
    double m_bench;
};

#endif // PARSERLINTWRAPPER_H
