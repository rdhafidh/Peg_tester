#include "parserlintwrapper.h"
#include "parserdef.h"
ParserLintWrapper::ParserLintWrapper(QObject *parent):QObject(parent),m_bench(0)
{
    mp = new QProcess;
    mp->setProcessChannelMode(QProcess::MergedChannels);   
    connect(mp,SIGNAL(readyRead()),SLOT(handleOutput()));
    connect(mp,SIGNAL(error(QProcess::ProcessError)),SLOT(handleError(QProcess::ProcessError)));
    connect(mp,SIGNAL(started()),SLOT(onStart()));
    connect(mp,SIGNAL(finished(int,QProcess::ExitStatus)),SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(this,SIGNAL(emitError(QString)),SLOT(onSyncGetError(QString)));
    connect(this,SIGNAL(emitBench(double)),SLOT(saveBench(double)));
}

void ParserLintWrapper::startParser(const QString &prog, const QStringList &arg,const QString & sat)
{ 
            auto start = std::chrono::steady_clock::now();
             mp->start(prog, arg);
             if(!mp->waitForFinished()){
                 Q_EMIT emitError(mp->errorString());
             } 
             auto end = std::chrono::steady_clock::now();
             auto diff= end-start;
             if (sat=="nano"){
                 auto elapsed=std::chrono::duration <double, std::nano> (diff).count() ;       
                  Q_EMIT emitBench(elapsed);
             }
             if (sat=="mili"){
                 auto elapsed=std::chrono::duration <double, std::milli> (diff).count() ; 
                  Q_EMIT emitBench(elapsed);
             }
             if (sat=="detik"){
                 auto elapsed=std::chrono::duration <double> (diff).count() ;                 
                  Q_EMIT emitBench(elapsed);
             } 
}

void ParserLintWrapper::handleError(QProcess::ProcessError e)
{//system error
    if (e==QProcess::FailedToStart){
        Q_EMIT emitError(tr("error proses %1 gagal dijalankan").arg(mp->program()));
        return;
     }
    if (e==QProcess::Crashed){
        Q_EMIT emitError(tr("error proses %1 mengalami crash ").arg(mp->program()));
        return;
    }
    if (e==QProcess::Timedout){
        Q_EMIT emitError(tr("error proses %1 mengalami waitfor... timeout ").arg(mp->program()));
        return;
    }
    if (e==QProcess::WriteError){
        Q_EMIT emitError(tr("error program %1 gagal menulis proses  ").arg(mp->program()));
        return;
    }
    if (e==QProcess::ReadError){
        Q_EMIT emitError(tr("error program %1 gagal membaca proses, mungkin program belum dijalankan").arg(mp->program()));
        return;
    }
    if (e==QProcess::UnknownError){
        Q_EMIT emitError(tr("error program %1 mengalamai kesalahan yang tidak diketahui").arg(mp->program()));
        return;
    }
    return; 
}

void ParserLintWrapper::handleOutput()
{
    m_data= mp->readAllStandardOutput();
    Q_EMIT emitOutput(QLatin1String(m_data)); 
}

void ParserLintWrapper::onStart()
{
    Q_EMIT emitStart();
}

void ParserLintWrapper::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    //handle parsing eror here
    if (exitCode==ParserDef::PARSE_ERROR){
        m_error.append(" input test file parsing terdapat kesalahan");
    }
    if (exitCode==ParserDef::INPUTTEST_NOTFOUND){
        m_error.append(" input test file parsing gagal ditemukan");
    }
    if (exitCode==ParserDef::GRAMMAR_NOTFOUND){
        m_error.append(" input test file grammar gagal ditemukan");
    }
    if (exitCode==ParserDef::GRAMMAR_ERROR){
        m_error.append(" input file grammar terdapat kesalahan");
    }
    Q_EMIT emitFinish(exitCode,exitStatus);
} 
