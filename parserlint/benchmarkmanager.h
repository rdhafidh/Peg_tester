#ifndef BENCHMARKMANAGER_H
#define BENCHMARKMANAGER_H

#include <QObject>
#include <functional>
#include <utility>
class QLocalServer;
class QLocalSocket;
class QByteArray;

class BenchmarkManager : public QObject
{
    Q_OBJECT
public:
    explicit BenchmarkManager(QObject *parent = 0);
    ~BenchmarkManager();
    
Q_SIGNALS:
     void benchmarkdone();
     void serverstopped();
     void asktostopsever();
     
     void serverListenFailed();
     
private slots:
    void handelClientBaru();
    
private:
    void processCmd(QLocalSocket *lc, QByteArray *buf);
    bool tryWrite(QLocalSocket *lc, QByteArray *buf);
    void benchmarkCsv(QLocalSocket *lc, const QString &inputtest, int  iterasi, int saveexcel, int  satuanbenchmark 
                     , const std::function<void(int, const QByteArray &, bool, bool)> &ansok );
    void benchmarkJSON(QLocalSocket *lc, const QString &inputtest, int  iterasi, int saveexcel, int  satuanbenchmark 
                     , const std::function<void(int,const QByteArray &,bool,bool)>&ansok );
    
    void reportWriter(const std::pair<QString, QList<double> > &result1, const std::pair<QString, QList<double> > &result2, const QString &title
                      , bool iscsv, int satuanbenchmark);
    QLocalServer* server;
    bool isbusy=false;
};


#endif // BENCHMARKMANAGER_H