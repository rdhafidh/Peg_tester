#include <QtCore>
#include <vector>
#include "parserdef.h"
#include "parserlint.h"
#include <benchmarkmanager.h>
#include <subsetexceltestsuite.h>
#include <qtest.h>

void checkFailedMsg(ParserDef::errcode m) {
  if (m == ParserDef::SUCCESS) {
    std::cout << "sukses";
  }
  if (m == ParserDef::GRAMMAR_NOTFOUND) {
    std::cout << "[~] grammar file tidak ditemukan...";
  }
  if (m == ParserDef::GRAMMAR_ERROR) {
    std::cout << "[~] terdapat kesalahan grammar...";
  }
  if (m == ParserDef::INPUTTEST_NOTFOUND) {
    std::cout << "[~] input test file tidak ditemukan...";
  }
  if (m == ParserDef::PARSE_ERROR) {
    std::cout << "[~] proses parsing gagal...";
  }
  if(m==ParserDef::SERVER_LISTEN_ERROR){
      std::cout << "[~] server benchmark listen galat...";
  }
}

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QCommandLineParser parser;
  parser.setApplicationDescription("PEG Parser check grammar dan AST analisis");
  parser.addHelpOption();
  QCommandLineOption astp(
      QStringList() << "a"
                    << "ast",
      QCoreApplication::translate(
          "main", "Tampilkan analisis AST, dengan kombinasi opsi -v"));
  parser.addOption(astp);

  QCommandLineOption gm(
      QStringList() << "g"
                    << "grammar",
      QCoreApplication::translate("main", "spesifikasikan peg grammar file"),
      "file");
  parser.addOption(gm);

  QCommandLineOption fl(
      QStringList() << "i"
                    << "input",
      QCoreApplication::translate("main", "spesifikasikan input test file"),
      "file");
  parser.addOption(fl);

  QCommandLineOption vb(
      QStringList() << "v"
                    << "verbose",
      QCoreApplication::translate("main", "tampilkan pesan2"));
  parser.addOption(vb);

  QCommandLineOption testdir(
      QStringList() << "d"
                    << "directory",
      QCoreApplication::translate("main",
                                  "test file directory bereksetensi *.t"),
      "directory");
  QCommandLineOption testrapidjson(
      QStringList() << "tj"
                    << "tj",
      QCoreApplication::translate("main",
                                  "mode test file parsing json dengan rapidjson parser"));
  parser.addOption(testrapidjson);
  
  QCommandLineOption testexcel(
      QStringList() << "rtx"
                    << "rtx",
      QCoreApplication::translate("main",
                                  "mode test file penerjemah sintaks excel"));
  parser.addOption(testexcel);
  
  QCommandLineOption testbenchmarkcsvandjson(
      QStringList() << "b"
                    << "bench",
      QCoreApplication::translate("main",
                                  "mode test benchmark server csv dan json"));
  parser.addOption(testbenchmarkcsvandjson);
  
  QCommandLineOption testpegparsingjson(
      QStringList() << "tjp"
                    << "tjp",
      QCoreApplication::translate("main",
                                  "mode test parsing json dengan peg parsing"));
  parser.addOption(testpegparsingjson);
  
  QCommandLineOption testcsv(
      QStringList() << "tcq"
                    << "tcq",
      QCoreApplication::translate("main",
                                  "mode test file parsing csv dengan Qtcsv parser"));
  parser.addOption(testcsv);
  
  QCommandLineOption testcsvpeg(
      QStringList() << "tcp"
                    << "tcp",
      QCoreApplication::translate("main",
                                  "mode test file parsing csv dengan peg parser"));
  parser.addOption(testcsvpeg);
  
  QCommandLineOption testcsvpeguseheader(
      QStringList() << "csvuseheader"
                    << "csvuseheader",
      QCoreApplication::translate("main",
                                  "mode csv menggunakan header"));
  parser.addOption(testcsvpeguseheader);

  QCommandLineOption th(
      QStringList() << "t"
                    << "thread",
      QCoreApplication::translate(
          "main", "menggunakan konkurensi multi proses parsing"));
  parser.addOption(th);

  parser.process(a);
  
  if(parser.isSet (testexcel)){
      SubsetExcelTestsuite se;
      return QTest::qExec(&se);
  }
  
  if(parser.isSet (testbenchmarkcsvandjson) ){
      BenchmarkManager mgr;
      QObject::connect (&mgr,&BenchmarkManager::serverstopped,[&a](){
          a.quit ();
      });
      QObject::connect (&mgr,&BenchmarkManager::serverListenFailed,[&a](){
          a.quit ();
      });
      return a.exec ();
  }
  if(parser.isSet (testrapidjson) 
          && parser.isSet (fl)){
      return ParserLint::parsejson (parser.value (fl).toStdString ().c_str (),
                                    parser.isSet (vb));
  }
  if(parser.isSet (testpegparsingjson)
          && parser.isSet (fl)){
      return ParserLint::parseselfjson (parser.value (fl).toStdString ().c_str (),parser.isSet (vb));
  }
  if(parser.isSet (testcsv) 
          && parser.isSet (fl)){
      return ParserLint::parsecsv (parser.value (fl).toStdString ().c_str (),parser.isSet (vb));
  }
  if(parser.isSet (testcsvpeg)
          && parser.isSet (fl)){
      return ParserLint::parseselfcsv (parser.value (fl).toStdString ().c_str (),parser.isSet (vb)
                                       ,parser.isSet (testcsvpeguseheader));
  }
  
  if (parser.isSet(gm) && parser.isSet(fl)) {
    if (parser.isSet(vb)) {
      qDebug() << "[*] grammar file:" << parser.value(gm)
               << " input test file:" << parser.value(fl);
    }
    ParserDef::errcode retval = ParserLint::parsetest(
        parser.value(gm).toUtf8().constData(), parser.value(fl).toUtf8().data(),
        parser.isSet(astp), parser.isSet(th), parser.isSet(vb));
    checkFailedMsg(retval);
    return retval;

  } else {
    if (parser.isSet(gm) && parser.isSet(testdir)) {
      QDir targetdir = parser.value(testdir);
      QDirIterator it(QDir::toNativeSeparators(targetdir.path()));
      if (parser.isSet(vb)) {
        std::cout << "\nusing from pathdir "
                  << QDir::toNativeSeparators(targetdir.path()).toStdString()
                  << "\n";
      }
      std::vector<std::string> fname;
      while (it.hasNext()) {
        auto file = it.next();
        QFileInfo isfile(file);
        if (isfile.isFile()) {
          if (parser.isSet(vb)) {
            std::cout << "\nmencoba input test " << file.toUtf8().constData()
                      << "\n";
          }
          auto retval = ParserLint::parsetest(
              parser.value(gm).toUtf8().constData(), file.toUtf8().data(),
              parser.isSet(astp), parser.isSet(th), parser.isSet(vb));
          checkFailedMsg(retval);
          if (retval != ParserDef::SUCCESS) {
            fname.push_back(file.toStdString());
          }
        }
      }
      if (fname.size() > 0) {
        std::cout << "\nterdapat " << fname.size()
                  << " file gagal parsing diantaranya:\n";
        for (decltype(fname.size()) x = 0; x < fname.size(); x++) {
          std::cout << "\n" << x + 1 << ". " << fname.at(x) << "\n";
        }
        return ParserDef::PARSE_ERROR;
      } else {
        std::cout << "\ndone.";
      }
    }
  }
  if (argc == 1) {
    qDebug() << parser.helpText().toStdString().c_str();
  }
  return 1;
}
