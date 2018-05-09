#ifndef BENCHMARKHANDLER_H
#define BENCHMARKHANDLER_H

#include <QByteArray>
#include <QLocalSocket>
#include <QObject>

class BenchmarkHandler : public QObject {
  Q_OBJECT
 public:
  explicit BenchmarkHandler(QObject *parent = 0);
  ~BenchmarkHandler();

  static bool testkoneksiAsync(const QString &socket);

  static bool killprocessBenchmark(const QString &socket);

  void handleBaru(const QByteArray &data, const QString &servername, int jumlahiterasihelper);
  
 signals:
  void writeFailed();
  void invalidReadResult(const QByteArray &data);
  void emitBenchmarkProgress(const QByteArray &msg, bool isdone, int progress);
  void emitStartprocess();

 private Q_SLOTS:
  void prepareWrite();
  void determineRead();

 private:
  QLocalSocket client;
  QByteArray mdata;
  int currentjumlahiterasi=0;
};

#endif  // BENCHMARKHANDLER_H