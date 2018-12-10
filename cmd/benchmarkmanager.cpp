#include "benchmarkmanager.h"
#include <parserlint.h>
#include <xlsx/xlsxdocument.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QtNetwork>
#include <chrono>
#include <iostream>

BenchmarkManager::BenchmarkManager(QObject *parent) : QObject(parent) {
  server = new QLocalServer;
  connect(server, SIGNAL(newConnection()), SLOT(handelClientBaru()));
  server->setMaxPendingConnections(100);
  if (!server->listen("skripsi_peg_parser_benchmarkmanager")) {
    this->serverListenFailed();
  }
}

BenchmarkManager::~BenchmarkManager() {
  if (server->isListening()) {
    server->close();
  }
  server->disconnect();
  delete server;
}

void BenchmarkManager::handelClientBaru() {
  auto client = server->nextPendingConnection ();
  auto maxmbread = 1024 * 1024;
  maxmbread *= 3;
  if (client) {
    connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
    QByteArray buffer;
    client->setReadBufferSize(maxmbread);
    bool ok = false;
    do {
      ok = client->waitForReadyRead(5000);
      buffer += client->readLine();
    } while (ok);
    if(!buffer.isEmpty ()){
        qDebug()<<"got buffer from client "<<buffer;
        processCmd(client, &buffer);
    }
  }
}

void BenchmarkManager::processCmd(QLocalSocket *lc, QByteArray *buf) {
  /*
   format json incoming pesan
   {
          "inputtest":"str",
           "formatdata":int,
           "iterasi":int (1-10),
           "saveexcel":bool,
           "satuanbenchmark":int,
           "stop":bool
   }
   * */
  auto json = QJsonDocument::fromJson(*buf);
  std::function<QByteArray(const QByteArray &)> answerfailed =
      [](const QByteArray &msg) -> QByteArray {
    QJsonObject obj;
    obj["answer"] = msg.constData();
    obj["status"] = false;
    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Compact);
    ;
  };
  std::function<void(int, const QByteArray &, bool, bool)>
      answerok = [this, lc](int val, const QByteArray &msg, bool isdone,
                            bool stat) -> void {
    QJsonObject obj;
    obj["progress"] = val;
    obj["answer"] = msg.constData();
    obj["status"] = stat;
    obj["isdone"] = isdone;
    QJsonDocument doc(obj);
    auto buffer = doc.toJson(QJsonDocument::Compact);
    if (!this->tryWrite(lc, &buffer)) {
      std::cout << "\nreply failed when answering..";
    }
  };

  if (json.isNull() || !json.isObject()) {
    auto rep = answerfailed("perintah tidak sah");
    if (!tryWrite(lc, &rep)) {
      std::cout << "\nfailed tryWrite.";
    }
    lc->close();
    this->benchmarkdone();
    return;
  }
  if (!json.object()["stop"].isNull() && json.object()["stop"].isBool() &&
      json.object()["stop"].toBool()) {
    // ask to stop server
    lc->close();
    server->close();
    this->serverstopped();
    return;
  }
  if (json.object()["inputtest"].isNull() ||
      !json.object()["inputtest"].isString()) {
    auto rep = answerfailed("input test file harus diisi");
    if (!tryWrite(lc, &rep)) {
      std::cout << "\nfailed tryWrite.";
    }
    lc->close();
    this->benchmarkdone();
    return;
  }
  QFileInfo inf(json.object ()["inputtest"].toString ());
  if(!inf.isFile ()
          || !inf.isReadable ()){
      auto rep = answerfailed("input test file tidak bisa dibaca");
      if (!tryWrite(lc, &rep)) {
        std::cout << "\nfailed tryWrite.";
      }
      std::cout<<"\ninput was: "<<json.object ()["inputtest"].toString ().toStdString ();
      lc->close();
      this->benchmarkdone();
      return;
  }
  if (json.object()["formatdata"].isNull() ||
      !json.object()["formatdata"].isDouble()) {
    auto idx = json.object()["formatdata"].toInt();
    bool ok = false;
    if (idx == 0 || idx == 1) {
      ok = true;
    }
    if (!ok) {
      auto rep = answerfailed("formatdata harus diisi");
      if (!tryWrite(lc, &rep)) {
        std::cout << "\nfailed tryWrite.";
      }
      lc->close();
      this->benchmarkdone();
      return;
    }
  }
  if (json.object()["iterasi"].isNull() ||
      !json.object()["iterasi"].isDouble()) {
    auto idx = json.object()["iterasi"].toInt();
    bool ok = true;
    if (idx < 1 || idx > 10) {
      ok = false;
    }
    if (!ok) {
      auto rep = answerfailed("jumlah iterasi harus diisi 1-10");
      if (!tryWrite(lc, &rep)) {
        std::cout << "\nfailed tryWrite.";
      }
      lc->close();
      this->benchmarkdone();
      return;
    }
  }
  if (json.object()["saveexcel"].isNull() ||
      !json.object()["saveexcel"].isBool()) {
    auto rep = answerfailed("saveexcel boolean harus diisi");
    if (!tryWrite(lc, &rep)) {
      std::cout << "\nfailed tryWrite.";
    }
    lc->close();
    this->benchmarkdone();
    return;
  }
  if (json.object()["satuanbenchmark"].isNull() ||
      !json.object()["satuanbenchmark"].isDouble()) {
    int idx = json.object()["satuanbenchmark"].toInt();
    bool ok = false;
    if (idx == 1 || idx == 0 || idx == 2) {
      ok = true;
    }
    if (!ok) {
      auto rep = answerfailed("satuanbenchmark  harus diisi 0-2");
      if (!tryWrite(lc, &rep)) {
        std::cout << "\nfailed tryWrite.";
      }
      lc->close();
      this->benchmarkdone();
      return;
    }
  }
  if (isbusy) {
    auto rep =
        answerfailed("benchmark server sedang sibuk melakukan benchmark");
    if (!tryWrite(lc, &rep)) {
      std::cout << "\nfailed tryWrite.";
    }
    lc->close();
    this->benchmarkdone();
    return;
  }
  /*
   format json incoming pesan
   {
          "inputtest":"str",
           "formatdata":int(0-1),
           "iterasi":int (1-10),
           "saveexcel":bool,
           "satuanbenchmark":int(0-2)
   }
   */
  isbusy = true;
  auto inputtest = json.object()["inputtest"].toString();
  auto formatdata = json.object()["formatdata"].toInt();
  auto iterasi = json.object()["iterasi"].toInt();
  auto saveexcel = json.object()["saveexcel"].toBool();
  auto satuanbenchmark = json.object()["satuanbenchmark"].toInt();
  switch (formatdata) {
    case 0:
      benchmarkCsv(lc, inputtest, iterasi, saveexcel, satuanbenchmark,
                   answerok);
      break;
    default:
      benchmarkJSON(lc, inputtest, iterasi, saveexcel, satuanbenchmark,
                    answerok);
      break;
  }
  isbusy = false;
  this->benchmarkdone();
}

bool BenchmarkManager::tryWrite(QLocalSocket *lc, QByteArray *buf) {
  if(!lc->isOpen () || 
          !lc->isWritable ()){
      qDebug()<<"handle is not writable ?";
      return false;
  }
  auto stat = lc->write(*buf);
  return lc->waitForBytesWritten() && stat != -1;
}

void BenchmarkManager::benchmarkCsv(
    QLocalSocket *lc, const QString &inputtest, int iterasi, int saveexcel,
    int satuanbenchmark,
    const std::function<void(int, const QByteArray &, bool, bool)> &ansok) {
    QList<double> resultcsv1;
    QList<double> resultcsv2;
    auto total = iterasi * 2;
    // pertama rekam parser rapidjson parser 1 .
    bool isok = true;
    for (int x = 0; x < iterasi; x++) {
      auto start1 = std::chrono::steady_clock::now();
      auto stat = ParserLint::parsecsv (inputtest.toStdString().c_str(), false);
      if (stat != ParserDef::SUCCESS) {
        isok = false;
        break;
      }
      auto end1 = std::chrono::steady_clock::now();
      auto diff1 = end1 - start1;
      double elapsed1 = 0;  
      switch (satuanbenchmark) {
        case 0:
          elapsed1 = std::chrono::duration<double, std::nano>(diff1).count();
          break;
        case 1:
          elapsed1 = std::chrono::duration<double, std::milli>(diff1).count();
          break;
        default:
          elapsed1 = std::chrono::duration<double>(diff1).count();
          break;
      }
      resultcsv1 << elapsed1;
      ansok(x + 1, "processsing done", false, true);
    }
  
    if (!isok) {
      QString msg(
          "benchmark galat ketika mencoba dengan qtcsv parser backend dengan "
          "input file %1");
      ansok(0, msg.arg(inputtest).toUtf8(), false, false);
      lc->close();
      return;
    }
    isok = true;
    for (int x = iterasi; x < total; x++) {
      auto start2 = std::chrono::steady_clock::now();
      auto stat =
          ParserLint::parseselfcsv (inputtest.toStdString().c_str(), false);
      if (stat != ParserDef::SUCCESS) {
        isok = false;
        break;
      }
      auto end2 = std::chrono::steady_clock::now();
      auto diff2 = end2 - start2;
      double elapsed2 = 0;
      switch (satuanbenchmark) {
        case 0:
          elapsed2 = std::chrono::duration<double, std::nano>(diff2).count();
          break;
        case 1:
          elapsed2 = std::chrono::duration<double, std::milli>(diff2).count();
          break;
        default:
          elapsed2 = std::chrono::duration<double>(diff2).count();
          break;
      }
      resultcsv2 << elapsed2;
      ansok(x + 1, "processsing done", false, true);
    }
  
    if (!isok) {
      QString msg(
          "benchmark galat ketika mencoba dengan self peg parser csv backend "
          "dengan input file %1");
      ansok(0, msg.arg(inputtest).toUtf8(), false, false);
      lc->close();
      return;
    }
    
  if (saveexcel == 1) {
      this->reportWriter (std::make_pair("QtCSV parser",resultcsv1)
                          ,std::make_pair("PEG CSV parser",resultcsv2)
                          ,tr("CSV Benchmark"),true,satuanbenchmark);
  }
  ansok(total, "proses benchmark csv selesai", true, true);
  lc->close();
}

void BenchmarkManager::benchmarkJSON(
    QLocalSocket *lc, const QString &inputtest, int iterasi, int saveexcel,
    int satuanbenchmark,
    const std::function<void(int, const QByteArray &, bool, bool)> &ansok) {
  QList<double> resultjson1;
  QList<double> resultjson2;
  auto total = iterasi * 2;
  // pertama rekam parser rapidjson parser 1 .
  bool isok = true;
  for (int x = 0; x < iterasi; x++) {
    auto start1 = std::chrono::steady_clock::now();
    auto stat = ParserLint::parsejson(inputtest.toStdString().c_str(), false);
    if (stat != ParserDef::SUCCESS) {
      isok = false;
      break;
    }
    auto end1 = std::chrono::steady_clock::now();
    auto diff1 = end1 - start1;
    double elapsed1 = 0;
    switch (satuanbenchmark) {
      case 0:
        elapsed1 = std::chrono::duration<double, std::nano>(diff1).count();
        break;
      case 1:
        elapsed1 = std::chrono::duration<double, std::milli>(diff1).count();
        break;
      default:
        elapsed1 = std::chrono::duration<double>(diff1).count();
        break;
    }
    resultjson1 << elapsed1;
 //       std::cout<<"\nelapsed1: "<<elapsed1<<" satuanbenchmark "<<satuanbenchmark;
    ansok(x + 1, "processsing done", false, true);
  }

  if (!isok) {
    QString msg(
        "benchmark galat ketika mencoba dengan rapidjson parser backend dengan "
        "input file %1");
    ansok(0, msg.arg(inputtest).toUtf8(), false, false);
    lc->close();
    return;
  }
  isok = true;
  for (int x = iterasi; x < total; x++) {
    auto start2 = std::chrono::steady_clock::now();
    auto stat =
        ParserLint::parseselfjson(inputtest.toStdString().c_str(), false);
    if (stat != ParserDef::SUCCESS) {
      isok = false;
      break;
    }
    auto end2 = std::chrono::steady_clock::now();
    auto diff2 = end2 - start2;
    double elapsed2 = 0;
    switch (satuanbenchmark) {
      case 0:
        elapsed2 = std::chrono::duration<double, std::nano>(diff2).count();
        break;
      case 1:
        elapsed2 = std::chrono::duration<double, std::milli>(diff2).count();
        break;
      default:
        elapsed2 = std::chrono::duration<double>(diff2).count();
        break;
    }
 //   std::cout<<"\nelapsed2: "<<elapsed2<<" satuanbenchmark "<<satuanbenchmark;
    resultjson2 << elapsed2;
    ansok(x + 1, "processsing done", false, true);
  }

  if (!isok) {
    QString msg(
        "benchmark galat ketika mencoba dengan self peg parser json backend "
        "dengan input file %1");
    ansok(0, msg.arg(inputtest).toUtf8(), false, false);
    lc->close();
    return;
  }
  if (saveexcel == 1) {
      this->reportWriter (std::make_pair("Rapidjson",resultjson1)
                          ,std::make_pair("PEG json parser",resultjson2)
                          ,tr("JSON Benchmark"),false,satuanbenchmark);
  }
  
  ansok(total, "proses benchmark json selesai", true, true);
  lc->close();
}

void BenchmarkManager::reportWriter(
    const std::pair<QString, QList<double> > &result1,
    const std::pair<QString, QList<double> > &result2, const QString &title,
    bool iscsv,int satuanbenchmark) {
  QTXLSX_USE_NAMESPACE
  QDateTime tmf = QDateTime::currentDateTime();
  QString fname = QString("laporan_benchmark_%1_%2_.xlsx")
                      .arg(iscsv ? "csv" : "json")
                      .arg(tmf.toString("dddd_MM_yy_hh_HH_mm_ss_zzz"));

  Document doc(fname);
  Format fmtjudul, fmtvalue;
  fmtvalue.setBorderStyle (Format::BorderMedium);
  fmtjudul.setBorderStyle(Format::BorderMedium);
  fmtjudul.setHorizontalAlignment(Format::AlignHCenter);
  fmtjudul.setVerticalAlignment(Format::AlignVCenter);
  doc.write("B3", title);
  doc.mergeCells("B3:D3", fmtjudul);
  doc.write("B4", "No",fmtjudul);
  doc.write("C4", result1.first, fmtjudul);
  doc.write("D4", result2.first, fmtjudul);

  auto countmean = [](const QList<double> &d) -> double {
    double h = 0;
    for (auto &a : d) {
      h += a;
    }
    if (d.size() != 0) {
      h /= d.size();
    }
    return h;
  };
  
  for (decltype(result1.second.size ()) x = 0; x < result1.second.size(); x++) {
      QString num("B%1");
      QString isi("C%1");
      doc.write (num.arg (QString::number (x+5.0)),QString::number  (x+1),fmtjudul);
      doc.write (isi.arg (QString::number (x+5.0)),QString::number  (result1.second.at (x)),fmtvalue);
  }
  for (decltype(result2.second.size ()) x = 0; x < result2.second.size(); x++) {
      QString isi("D%1");
      doc.write (isi.arg (QString::number (x+5.0)),QString::number (result2.second.at (x)),fmtvalue);
  }
  QString rata2str("B%1");
  doc.write (rata2str.arg (4+result2.second.size ()+1),"Rata-rata",fmtvalue);
  auto nanostr="nano second";
  auto milistr="milisecond";
  auto secondstr="second";
  QString rata2left("C%1");
  QString rata2right("D%1");
  QString value("%1 %2");
  switch (satuanbenchmark) {
  case 0:
      doc.write (rata2left.arg (4+result2.second.size ()+1)
                 ,value.arg (countmean(result1.second))
                 .arg (nanostr),fmtvalue);
      doc.write (rata2right.arg (4+result2.second.size ()+1)
                 ,value.arg (countmean(result2.second))
                 .arg (nanostr),fmtvalue);
      break;
  case 1:
      doc.write (rata2left.arg (4+result2.second.size ()+1)
                 ,value.arg (countmean(result1.second))
                 .arg (milistr),fmtvalue);
      doc.write (rata2right.arg (4+result2.second.size ()+1)
                 ,value.arg (countmean(result2.second))
                 .arg (milistr),fmtvalue);
      break;
  default:
      doc.write (rata2left.arg (4+result2.second.size ()+1)
                 ,value.arg (countmean(result1.second))
                 .arg (secondstr),fmtvalue);
      doc.write (rata2right.arg (4+result2.second.size ()+1)
                 ,value.arg (countmean(result2.second))
                 .arg (secondstr),fmtvalue);
      break;
  }
  doc.save ();
}
