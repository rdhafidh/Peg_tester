#include "benchmarkhandler.h"
#include <jsonsocketparserworkaround.h>
#include <qfuture.h>
#include <qjsonobject.h>
#include <QJsonDocument>
#include <QtConcurrent/QtConcurrent>

BenchmarkHandler::BenchmarkHandler(QObject *parent) : QObject(parent) {
  connect(&client, SIGNAL(connected()), SLOT(prepareWrite()));
  connect(&client, SIGNAL(readyRead()), SLOT(determineRead()));
}

BenchmarkHandler::~BenchmarkHandler() {
  if (client.isOpen()) {
    client.close();
  }
}

bool testkoneksi(const QString &socket) {
  QLocalSocket mc;
  mc.connectToServer(socket);
  bool re = mc.waitForConnected();
  if (re) {
    mc.close();
  }
  return re;
}

bool BenchmarkHandler::testkoneksiAsync(const QString &socket) {
  QFuture<bool> ret = QtConcurrent::run(testkoneksi, socket);
  ret.waitForFinished();
  return ret.result();
}

bool killingBenchmarkServer(const QString &socket) {
  QLocalSocket mc;
  mc.connectToServer(socket);
  bool re = mc.waitForConnected();
  if (!re) {
	qDebug()<<"warning waitForConnected when killing false";
    return false;
  }
  QJsonObject obj;
  obj["stop"] = true;
  QJsonDocument doc(obj);
  mc.write(doc.toJson(QJsonDocument::Compact));
  if (!mc.waitForBytesWritten()) {
	  qDebug()<<"warning waitForBytesWritten when killing false";
    return false;
  } 
  mc.close();
  return true;
}

bool BenchmarkHandler::killprocessBenchmark(const QString &socket) {
  QFuture<bool> ret = QtConcurrent::run(killingBenchmarkServer, socket);
  ret.waitForFinished();
  bool retval=ret.result();
  if(!retval){
	  qDebug()<<"warning killprocessBenchmark when killing false";
  }else{
	  qDebug()<<"warning killprocessBenchmark when killing ok";
  }
  return retval;
}


void BenchmarkHandler::handleBaru(const QByteArray &data,
                                  const QString &servername,
                                  int jumlahiterasihelper) {
  mdata = data;
  if (client.isOpen()) {
    client.close();
  }
  currentjumlahiterasi = jumlahiterasihelper;
  client.connectToServer(servername);
}

void BenchmarkHandler::prepareWrite() {
  this->emitStartprocess();
  qDebug() << "write to server";
  client.write(mdata);
  currentjumlahiterasi *= 2;
  for (int x = 0; x < currentjumlahiterasi; x++) {
    client.waitForReadyRead();
  }
}

void BenchmarkHandler::determineRead() {
  qDebug() << "determine read called";

  auto res = client.readLine();
  qDebug() << "raw res return" << res;
  auto datas = JsonSocketParserWorkaround::fixmultiLinejson(res);
  if(datas.size ()>1){
      for (auto &data : datas) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject()) {
          this->invalidReadResult(data);
          return;
        }
        if (doc.object()["status"].isNull() || !doc.object()["status"].isBool() ||
            !doc.object()["status"].toBool()) {
          QString ans = doc.object()["answer"].isNull()
                            ? "Galat tidak diketahui"
                            : doc.object()["answer"].toString();
          this->invalidReadResult(ans.toUtf8());
          return;
        }
        if (doc.object()["answer"].isNull() || doc.object()["isdone"].isNull() ||
            doc.object()["progress"].isNull() || doc.object()["status"].isNull() ||
            !doc.object()["answer"].isString() ||
            !doc.object()["isdone"].isBool() ||
            !doc.object()["progress"].isDouble() ||
            !doc.object()["status"].isBool()) {
          QString ans = doc.object()["answer"].isNull()
                            ? "Galat tidak diketahui"
                            : doc.object()["answer"].toString();
          this->invalidReadResult(ans.toUtf8());
          return;
        }
        this->emitBenchmarkProgress(doc.object()["answer"].toString().toUtf8(),
                                    doc.object()["isdone"].toBool(),
                                    doc.object()["progress"].toInt());
      }
  }else{
      QJsonDocument doc = QJsonDocument::fromJson(res);
      if (doc.isNull() || !doc.isObject()) {
        this->invalidReadResult(res);
        return;
      }
      if (doc.object()["status"].isNull() || !doc.object()["status"].isBool() ||
          !doc.object()["status"].toBool()) {
        QString ans = doc.object()["answer"].isNull()
                          ? "Galat tidak diketahui"
                          : doc.object()["answer"].toString();
        this->invalidReadResult(ans.toUtf8());
        return;
      }
      if (doc.object()["answer"].isNull() || doc.object()["isdone"].isNull() ||
          doc.object()["progress"].isNull() || doc.object()["status"].isNull() ||
          !doc.object()["answer"].isString() ||
          !doc.object()["isdone"].isBool() ||
          !doc.object()["progress"].isDouble() ||
          !doc.object()["status"].isBool()) {
        QString ans = doc.object()["answer"].isNull()
                          ? "Galat tidak diketahui"
                          : doc.object()["answer"].toString();
        this->invalidReadResult(ans.toUtf8());
        return;
      }
      this->emitBenchmarkProgress(doc.object()["answer"].toString().toUtf8(),
                                  doc.object()["isdone"].toBool(),
                                  doc.object()["progress"].toInt());
  }
  
}
