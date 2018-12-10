#include "mainwindow.h"
#include "parserlintwrapper.h"
#include "settinghandler.h"
#include "settingprogram.h"
#include "xlswriter.h"
#ifdef BUILD_WEBKIT
#include "helpdialog.h"
#endif
#include <benchmarkhandler.h>
#include <excelMainWindow.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtCore>
#include <algorithm>
#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include <utility>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  excelui = new excel::MainWindow(this);
  ui->gridLayout_excel->addWidget(excelui);

  act_newfile = new QAction(tr("Lembar kerja baru"), this);
  act_open = new QAction(tr("Buka lembar kerja"), this);
  act_save = new QAction(tr("Simpan lembar kerja"), this);
  act_saveas = new QAction(tr("Simpan lembar kerja sebagai lainya"), this);

  connect(act_newfile, SIGNAL(triggered(bool)), SLOT(excelnew()));
  connect(act_open, SIGNAL(triggered(bool)), SLOT(excelopen()));
  connect(act_save, SIGNAL(triggered(bool)), SLOT(excelsave()));
  connect(act_saveas, SIGNAL(triggered(bool)), SLOT(excelsaveas()));

  ui->toolButton_excelfile->addAction(act_newfile);
  ui->toolButton_excelfile->addAction(act_open);
  ui->toolButton_excelfile->addAction(act_save);
  ui->toolButton_excelfile->addAction(act_saveas);

  act_copy = new QAction(tr("Kopi"), this);
  act_cut = new QAction(tr("Cut"), this);
  act_paste = new QAction(tr("Paste"), this);
  connect(act_copy, SIGNAL(triggered(bool)), SLOT(copy()));
  connect(act_cut, SIGNAL(triggered(bool)), SLOT(cut()));
  connect(act_paste, SIGNAL(triggered(bool)), SLOT(paste()));
  ui->toolButton_exceledit->addAction(excelui->getUndo());
  ui->toolButton_exceledit->addAction(excelui->getRedo());
  ui->toolButton_exceledit->addAction(act_copy);
  ui->toolButton_exceledit->addAction(act_cut);
  ui->toolButton_exceledit->addAction(act_paste);

  act_insertbaris = new QAction(tr("Insert baris"), this);
  act_insertgambar = new QAction(tr("Insert gambar"), this);
  act_insertkolom = new QAction(tr("Insert kolom"), this);
  connect(act_insertbaris, SIGNAL(triggered(bool)), SLOT(excelinsertbaris()));
  connect(act_insertgambar, SIGNAL(triggered(bool)), SLOT(excelinsertgambar()));
  connect(act_insertkolom, SIGNAL(triggered(bool)), SLOT(excelinsertkolom()));
  ui->toolButton_excelinsert->addAction(act_insertbaris);
  ui->toolButton_excelinsert->addAction(act_insertkolom);
  ui->toolButton_excelinsert->addAction(act_insertgambar);

  act_formatfont = new QAction(tr("Font"), this);
  act_formatbackground = new QAction(tr("Warna Background"), this);
  act_formattextcolor = new QAction(tr("Warna Tulisan"), this);
  act_formatsortir = new QAction(tr("Sortir"), this);
  connect(act_formatfont, SIGNAL(triggered(bool)), SLOT(excelformatfont()));
  connect(act_formatbackground, SIGNAL(triggered(bool)),
          SLOT(excelformatbackgroundcolor()));
  connect(act_formattextcolor, SIGNAL(triggered(bool)),
          SLOT(excelformattextcolor()));
  connect(act_formatsortir, SIGNAL(triggered(bool)), SLOT(excelformatsortir()));
  ui->toolButton_excelformat->addAction(excelui->getAlignLeft());
  ui->toolButton_excelformat->addAction(excelui->getAlignCenter());
  ui->toolButton_excelformat->addAction(excelui->getAlignRight());
  ui->toolButton_excelformat->addAction(act_formatfont);
  ui->toolButton_excelformat->addAction(act_formatbackground);
  ui->toolButton_excelformat->addAction(act_formattextcolor);
  ui->toolButton_excelformat->addAction(act_formatsortir);

  msglabel = new QLabel;
  msglabel->setTextFormat(Qt::RichText);
  msglabel->setAttribute(Qt::WA_DeleteOnClose);

  ui->statusBar->addWidget(msglabel);

  mbar = new QProgressBar;
  mbar->setRange(0, 100);
  mbar->setValue(0);
  mbar->setFormat(tr("memroses %p% ..."));
  mbar->setAttribute(Qt::WA_DeleteOnClose);
  mbar->hide();
  ui->statusBar->addPermanentWidget(mbar);

  connect(ui->actionKonfigurasi_program, SIGNAL(triggered()),
          SLOT(showsettings()));
  connect(ui->actionJalankan_test, SIGNAL(triggered()), SLOT(runner()));
  connect(ui->actionPenggunaan, SIGNAL(triggered()), SLOT(tampilPenggunaan()));
  connect(ui->actionVersi, SIGNAL(triggered()), SLOT(versi()));
  determineValidConfigtoStartTest();
}

MainWindow::~MainWindow() {
  delete excelui;
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e) {
  if (BenchmarkHandler::testkoneksiAsync(
          "skripsi_peg_parser_benchmarkmanager")) {
    BenchmarkHandler::killprocessBenchmark(
        "skripsi_peg_parser_benchmarkmanager");
  }
  e->accept();
}

void MainWindow::determineValidConfigtoStartTest() {
  // only determine apakah pakai valid config tidak
  QVariant var1;
  QFileInfo inf;

  auto checkbenchmarksetting = [this, &var1, &inf]() {
    // be strict catat semua kemungkinan opsi yang muncul
    var1 = SettingHandler::getSetting("modebenchmark", "formatdata");
    switch (var1.toInt()) {
      case 0:
        isvalidconfig = true;
        break;
      case 1:
        isvalidconfig = true;
        break;
      default:
        isvalidconfig = false;
        QMessageBox::warning(
            this, tr("Galat"),
            tr("Pengaturan mode test format data tidak sah dalam mode test "
               "benchmark testing, silahkan anda memilih pengaturan mode test "
               "format data dalam benchmark testing di menu pengaturan "
               "terlebih dahulu."));
        break;
    }
    if (!isvalidconfig) {
      ui->toolButton_prosestesting->setEnabled(isvalidconfig);
      ui->actionJalankan_test->setEnabled(isvalidconfig);
      return;
    }
    var1 = SettingHandler::getSetting("modebenchmark", "inputdata");
    inf.setFile(var1.toString());
    if (!inf.isFile() || !inf.isReadable()) {
      isvalidconfig = false;
      QMessageBox::warning(
          this, tr("Galat"),
          tr("Pengaturan mode test benchmark input datanya tidak bisa dibaca "
             "oleh sistem aplikasi, silahkan anda memilih  input data yang "
             "diinginkan pada menu pengaturan."));
    } else {
      isvalidconfig = true;
    }
    if (!isvalidconfig) {
      ui->toolButton_prosestesting->setEnabled(isvalidconfig);
      ui->actionJalankan_test->setEnabled(isvalidconfig);
      return;
    }
    var1 =
        SettingHandler::getSetting("modebenchmark", "pengaturan_iterasi_test");
    switch (var1.toInt()) {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
        isvalidconfig = true;
        break;
      default:
        QMessageBox::warning(
            this, tr("Galat"),
            tr("Pengaturan mode validasi test benchmark tidak bisa dijalankan "
               "karena jumlah iterasi test tidak bisa dibaca oleh sistem "
               "aplikasi, silahkan anda memilih kembali jumlah iterasi proses "
               "benchmark pada menu pengaturan ."));
        isvalidconfig = false;
        break;
    }
    if (!isvalidconfig) {
      ui->toolButton_prosestesting->setEnabled(isvalidconfig);
      ui->actionJalankan_test->setEnabled(isvalidconfig);
      return;
    }
    var1 =
        SettingHandler::getSetting("modebenchmark", "pengaturan_export_excel");
    // skip

    var1 = SettingHandler::getSetting("modebenchmark",
                                      "pengaturan_satuan_benchmark");
    switch (var1.toInt()) {
      case 0:
      case 1:
      case 2:
        isvalidconfig = true;
        break;
      default:
        isvalidconfig = false;
        QMessageBox::warning(
            this, tr("Galat"),
            tr("Pengaturan konfigurasi satuan benchmark tidak bisa dibaca oleh "
               "sistem aplikasi, silahkan anda memilih kembali pengaturan "
               "satuan benchmark pada menu pengaturan ."));
        break;
    }
  };
  auto checkmanualtesting = [this, &var1, &inf]() {
    var1 = SettingHandler::getSetting("parserlint", "parserlint1");
    inf.setFile(var1.toString());
    if (var1.toString().isEmpty() || !inf.isExecutable()) {
      isvalidconfig = false;
      QMessageBox::warning(
          this, tr("Galat"),
          tr("Program parser validator test belum diset dalam menu pengaturan, "
             "anda bisa melakukan hal tersebut terlebih "
             "dahulu."));
    } else {
      isvalidconfig = true;
    }
    if (!isvalidconfig) {
      ui->toolButton_prosestesting->setEnabled(isvalidconfig);
      ui->actionJalankan_test->setEnabled(isvalidconfig);
      return;
    }
    var1 = SettingHandler::getSetting("modetest", "manual_inputgrammar");
    inf.setFile(var1.toString());
    if (!inf.isFile() || !inf.isReadable()) {
      isvalidconfig = false;
      QMessageBox::warning(
          this, tr("Galat"),
          tr("Pengaturan mode validasi test tidak bisa dijalankan karena input "
             "file grammar tidak bisa dibaca oleh sistem aplikasi silahkan "
             "anda memilih kembali input file di menu pengaturan."));
    } else {
      isvalidconfig = true;
    }
    if (!isvalidconfig) {
      ui->toolButton_prosestesting->setEnabled(isvalidconfig);
      ui->actionJalankan_test->setEnabled(isvalidconfig);
      return;
    }
    var1 = SettingHandler::getSetting("modetest", "manual_inputdata");
    inf.setFile(var1.toString());
    if (!inf.isFile() || !inf.isReadable()) {
      isvalidconfig = false;
      QMessageBox::warning(
          this, tr("Galat"),
          tr("Pengaturan mode validasi test tidak bisa dijalankan karena input "
             "file data tidak bisa dibaca oleh sistem aplikasi, silahkan anda "
             "memilih kembali input file di menu pengaturan."));
    } else {
      isvalidconfig = true;
    }
    if (!isvalidconfig) {
      ui->toolButton_prosestesting->setEnabled(isvalidconfig);
      ui->actionJalankan_test->setEnabled(isvalidconfig);
      return;
    }
    var1 = SettingHandler::getSetting("modetest", "manual_tampilast");
    // skip karena cuman bisa true atau false

    var1 = SettingHandler::getSetting("modetest", "manual_modeverbose");
  };

  auto checkotomatistesting = [this, &var1, &inf]() {
    var1 = SettingHandler::getSetting("modetest", "otomatis_pengetesan");
    switch (var1.toInt()) {
      case 0:
      case 1:
      case 2:
        isvalidconfig = true;
        break;
      default:
        isvalidconfig = false;
        QMessageBox::warning(
            this, tr("Galat"),
            tr("Pengaturan mode validasi test otomatis tidak bisa dijalankan "
               "karena mode validasi tidak bisa dikenali oleh sistem aplikasi, "
               "silahkan anda memilih kembali mode validasi parsing otomatis "
               "oleh sistem."));
        break;
    }
    if (!isvalidconfig) {
      ui->toolButton_prosestesting->setEnabled(isvalidconfig);
      ui->actionJalankan_test->setEnabled(isvalidconfig);
      return;
    }
    var1 = SettingHandler::getSetting("modetest", "otomatis_inputfolderdata");
    inf.setFile(var1.toString());
    if (!inf.isDir()) {
      isvalidconfig = false;
      QMessageBox::warning(
          this, tr("Galat"),
          tr("Pengaturan mode validasi test otomatis tidak bisa dijalankan "
             "karena folder input test data tidak bisa dibaca oleh sistem "
             "aplikasi, silahkan anda memilih kembali folder input test data "
             "pada menu pengaturan."));
    } else {
      isvalidconfig = true;
    }
    if (!isvalidconfig) {
      ui->toolButton_prosestesting->setEnabled(isvalidconfig);
      ui->actionJalankan_test->setEnabled(isvalidconfig);
      return;
    }
  };

  // Intinya ada 2 test
  // 1. mode benchmark
  // 2. mode test manual
  // 3. mode test otomatis

  var1 = SettingHandler::getSetting("parserlint", "modetesting");
  switch (var1.toInt()) {
    case 1:
      isvalidconfig = true;
      checkbenchmarksetting();
      break;
    case 2:
      isvalidconfig = true;
      checkmanualtesting();
      break;
    case 3:
      isvalidconfig = true;
      checkotomatistesting();
      break;
    default:
      isvalidconfig = false;
      QMessageBox::warning(
          this, tr("Galat"),
          tr("Pengaturan mode test tidak sah silahkan anda memilih pengaturan "
             "terlebih dahulu di menu pengaturan."));
      break;
  }

  ui->toolButton_prosestesting->setEnabled(isvalidconfig);
  ui->actionJalankan_test->setEnabled(isvalidconfig);
}

void MainWindow::changeEvent(QEvent *e) {
  QMainWindow::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void MainWindow::showsettings() {
  SettingProgram *stp = new SettingProgram(this);

  stp->setAttribute(Qt::WA_DeleteOnClose);
  connect(stp, &SettingProgram::askreloadsetting,
          [this]() { this->determineValidConfigtoStartTest(); });
  QPoint p = this->pos();
  p.ry() = p.ry() + ((p.ry() / 2) * 1);
  stp->move(p);
  stp->show();
}

void MainWindow::readyParse() {
  //    disconnect(msglabel, 0, 0, 0);

  //    QTimer::singleShot(15000, msglabel, SLOT(hide()));
  //    if (e) {
  //        msglabel->setText(tr("siap dijalankan test"));
  //        msglabel->setStyleSheet(
  //                    "background-color: rgb(168, 255, 97); \
//                    color: rgb(0, 0, 0);");
  //                msglabel->show();
  //    } else {
  //        msglabel->setText(
  //                    tr("belum siap dijalankan test, karena terdapat
  //                    kesalahan pengaturan"));
  //        msglabel->setStyleSheet(
  //                    "background-color: rgb(255, 88, 32);\
//                    color: rgb(0, 0, 0);");
  //                msglabel->show();
  //    }
}

void MainWindow::onError(const QString &e) {
  disconnect(msglabel, 0, 0, 0);

  msglabel->setText(e);
  msglabel->setStyleSheet(
      "background-color: rgb(255, 88, 32);\
                            color: rgb(0, 0, 0);");
  QTimer::singleShot(16000, Qt::PreciseTimer, msglabel, SLOT(hide()));
  msglabel->show();
  mbar->setValue(0);
  mbar->hide();
  ui->actionJalankan_test->setEnabled(false);
}

void MainWindow::onOutput(QString e) {
  QString v;

  int last = ui->textEdit_output->verticalScrollBar()->maximum();
  std::move(e.begin(), e.end(), std::back_inserter(v));
  ui->textEdit_output->append(v);
  // adjust textedit vertical scrollbar value
  ui->textEdit_output->verticalScrollBar()->setValue(last);
  v.clear();
}

void MainWindow::onStart() {
  disconnect(msglabel, 0, 0, 0);

  msglabel->setText(tr("sedang menjalankan test..."));
  msglabel->setStyleSheet(
      "background-color: rgb(168, 255, 97); \
                            color: rgb(0, 0, 0);");
  QTimer::singleShot(16000, Qt::PreciseTimer, msglabel, SLOT(hide()));
  QDateTime tm = QDateTime::currentDateTime();
  ui->textEdit_output->append(
      "\n===================================================");
  ui->textEdit_output->append(tr("\nMemulai proses testing pada %1 \n")
                                  .arg(tm.toString(Qt::RFC2822Date)));

  msglabel->show();
  mbar->setValue(0);
  mbar->show();
  ui->actionJalankan_test->setEnabled(false);
}

void MainWindow::onFinish(int e, QProcess::ExitStatus w) {
  Q_UNUSED(e)
  disconnect(msglabel, 0, 0, 0);
  if (w == QProcess::NormalExit && e == 0) {
    msglabel->setText(tr("proses selesai dengan baik..."));
    QTimer::singleShot(30000, Qt::PreciseTimer, msglabel, SLOT(hide()));
    msglabel->setStyleSheet(
        "background-color: rgb(168, 255, 97); \
                                color: rgb(0, 0, 0);");
    msglabel->show();
  }
  if (w == QProcess::NormalExit && e == -1) {  // gramar not found
    msglabel->setText(
        tr("proses selesai dengan gagal input grammar tidak ditemukan..."));
    QTimer::singleShot(16000, Qt::PreciseTimer, msglabel, SLOT(hide()));
    msglabel->setStyleSheet(
        "background-color: rgb(255, 88, 32);\
                                color: rgb(0, 0, 0);");
    msglabel->show();
  }
  if (w == QProcess::NormalExit && e == -2) {  // grammar error
    msglabel->setText(tr("proses selesai dengan gagal input grammar error..."));
    QTimer::singleShot(16000, Qt::PreciseTimer, msglabel, SLOT(hide()));
    msglabel->setStyleSheet(
        "background-color: rgb(255, 88, 32);\
                                color: rgb(0, 0, 0);");
    msglabel->show();
  }
  if (w == QProcess::NormalExit && e == -3) {
    msglabel->setText(
        tr("proses selesai dengan gagal input testfile tidak ditemukan..."));
    QTimer::singleShot(16000, Qt::PreciseTimer, msglabel, SLOT(hide()));
    msglabel->setStyleSheet(
        "background-color: rgb(255, 88, 32);\
                                color: rgb(0, 0, 0);");
    msglabel->show();
  }
  if (w == QProcess::NormalExit && e == -4) {
    msglabel->setText(
        tr("proses selesai dengan adanya kesalahan sintaks dalam testfile buat "
           "parsing..."));
    QTimer::singleShot(16000, Qt::PreciseTimer, msglabel, SLOT(hide()));
    msglabel->setStyleSheet(
        "background-color: rgb(255, 88, 32);\
                                    color: rgb(0, 0, 0);");
    msglabel->show();
  }
  if (w == QProcess::NormalExit && e == -5) {
    msglabel->setText(
        tr("proses selesai dengan gagal testfile buat parsing tidak "
           "ditemukan..."));
    QTimer::singleShot(16000, Qt::PreciseTimer, msglabel, SLOT(hide()));
    msglabel->setStyleSheet(
        "background-color: rgb(255, 88, 32);\
                                    color: rgb(0, 0, 0);");
    msglabel->show();
  }
  if (w == QProcess::CrashExit) {
    msglabel->setText(
        tr("proses selesai dengan gagal karena child proses parselint.exe "
           "mengalami crash ..."));
    QTimer::singleShot(16000, Qt::PreciseTimer, msglabel, SLOT(hide()));
    msglabel->setStyleSheet(
        "background-color: rgb(255, 88, 32);\
                                color: rgb(0, 0, 0);");
    msglabel->show();
  }
  ui->actionJalankan_test->setEnabled(true);
  mbar->setValue(0);
  mbar->hide();
}

bool MainWindow::checkValidSettings() {
  QFileInfo file(QDir::currentPath() + "/pengaturan.ini");
  if (!file.exists() || !file.isReadable()) {
    return false;
  }
  QVariant var1;
  // Intinya ada 2 test
  // 1. mode benchmark
  // 2. mode test manual
  // 3. mode test otomatis
  var1 = SettingHandler::getSetting("parserlint", "modetesting");
  bool ok = true;
  int modesetting = 0;
  switch (var1.toInt()) {
    case 1:
    case 2:
    case 3:
      break;
    default:
      ok = false;
      break;
  }
  if (!ok) return ok;
  modesetting = var1.toInt();

  auto isokbenchmarksetting = [this]() -> bool {
    auto variant = SettingHandler::getSetting("modebenchmark", "formatdata");
    // 0 csv, 1 json
    bool localok = true;
    switch (variant.toInt()) {
      case 0:
      case 1:
        break;
      default:
        return false;
    }
    variant = SettingHandler::getSetting("modebenchmark", "inputdata");
    if (variant.toString().isEmpty()) return false;

    QFileInfo inf(variant.toString());
    localok = inf.isFile() && inf.isReadable();
    if (!localok) return false;

    variant =
        SettingHandler::getSetting("modebenchmark", "pengaturan_iterasi_test");
    switch (variant.toInt()) {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
        break;
      default:
        return false;
    }
    variant =
        SettingHandler::getSetting("modebenchmark", "pengaturan_export_excel");
    if (!variant.isValid()) return false;

    variant = SettingHandler::getSetting("modebenchmark",
                                         "pengaturan_satuan_benchmark");
    switch (variant.toInt()) {
      case 1:
      case 2:
      case 3:
        break;
      default:
        return false;
    }

    return true;
  };

  return ok;
}

void MainWindow::diagnosaError() {
  if (m_batcherrorlog.size() > 0) {
    ui->textEdit_output->append(
        "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    ui->textEdit_output->append(tr("\ndiagnosa error: \n"));
    foreach (auto err, m_batcherrorlog) {
      ui->textEdit_output->append(
          tr("[~] Index %1 gagal, karena %2\n").arg(err.first).arg(err.second));
    }
  }
}

void MainWindow::runner() {
  qDebug() << "isvalidconfig" << isvalidconfig;

  determineValidConfigtoStartTest();
  qDebug() << "isvalidconfig" << isvalidconfig;

  if (!isvalidconfig) return;

  QVariant parserlint1 =
      SettingHandler::getSetting("parserlint", "parserlint1");

  // Intinya ada 2 test
  // 1. mode benchmark
  // 2. mode test manual
  // 3. mode test otomatis
  ui->toolButton_prosestesting->setEnabled(false);
  qApp->processEvents();

  QVariant modetesting =
      SettingHandler::getSetting("parserlint", "modetesting");

  // be strict catat semua kemungkinan opsi yang muncul
  QVariant formatdata =
      SettingHandler::getSetting("modebenchmark", "formatdata");

  QVariant inputdata = SettingHandler::getSetting("modebenchmark", "inputdata");

  QVariant manual_inputgrammar =
      SettingHandler::getSetting("modetest", "manual_inputgrammar");

  QVariant manual_inputdata =
      SettingHandler::getSetting("modetest", "manual_inputdata");

  QVariant manual_tampilast =
      SettingHandler::getSetting("modetest", "manual_tampilast");
  // skip karena cuman bisa true atau false

  QVariant manual_modeverbose =
      SettingHandler::getSetting("modetest", "manual_modeverbose");

  QVariant otomatis_pengetesan =
      SettingHandler::getSetting("modetest", "otomatis_pengetesan");

  QVariant otomatis_inputfolderdata =
      SettingHandler::getSetting("modetest", "otomatis_inputfolderdata");

  QVariant pengaturan_iterasi_test =
      SettingHandler::getSetting("modebenchmark", "pengaturan_iterasi_test");

  QVariant pengaturan_export_excel =
      SettingHandler::getSetting("modebenchmark", "pengaturan_export_excel");
  // skip

  QVariant pengaturan_satuan_benchmark = SettingHandler::getSetting(
      "modebenchmark", "pengaturan_satuan_benchmark");

  parserlint1 = SettingHandler::getSetting("parserlint", "parserlint1");

  // yang paling menentukan pengaturan modesetting
  if (modetesting.toInt() == 1) {
    // mode benchmark
    if (!BenchmarkHandler::testkoneksiAsync(
            "skripsi_peg_parser_benchmarkmanager")) {
      QFileInfo inf(parserlint1.toString());
      if (!inf.isFile() || !inf.isReadable ()) {
        QMessageBox::warning(
            this, tr("Galat"),
            tr("Program tcp pipe server benchmark manager tidak bisa "
               "ditemukan, anda bisa melakukan setting pengaturan "
               "path program parserlint_vs2015.exe di menu pengaturan. Kode "
               "error 400"));
        ui->toolButton_prosestesting->setEnabled(true);
        qApp->processEvents();
        return;
      }
      
      QStringList list;
      QDir dir;
      
      list << "cmd.exe"
           << "/C" << "cd"<< dir.toNativeSeparators (QCoreApplication::applicationDirPath ())
           << "&&"<< parserlint1.toString() << "-b";
      QString cmd;
      foreach( auto &mlist, list){
          if(!cmd.isEmpty ()){
              cmd.append (" ");  
          }
          cmd.append (mlist);
      }
      bool proc = QProcess::startDetached(cmd);
      if (!proc) {
        QMessageBox::warning(
            this, tr("Galat"),
            tr("Program tcp pipe server benchmark manager tidak bisa "
               "dijalankan, re-instalasi aplikasi ini "
               "bisa membantu dari file installer. Kode error 40C"));
        ui->toolButton_prosestesting->setEnabled(true);
        qApp->processEvents();
        return;
      }
      if (proc) {
        QThread::msleep(900);
        if (!BenchmarkHandler::testkoneksiAsync(
                "skripsi_peg_parser_benchmarkmanager")) {
          QMessageBox::warning(
              this, tr("Galat"),
              tr("Program tcp pipe server benchmark manager tidak bisa "
                 "ditemukan secara otomatis, re-instalasi aplikasi ini "
                 "bisa membantu dari file installer. Kode error 40D"));
          ui->toolButton_prosestesting->setEnabled(true);
          qApp->processEvents();
          return;
        }
      }
    }
    ismodebenchmark = true;
    mbar->setMaximum(pengaturan_iterasi_test.toInt() * 2);
    BenchmarkHandler bench;
    connect(&bench, &BenchmarkHandler::emitStartprocess,
            [this]() { this->mbar->show(); });
    connect(&bench, &BenchmarkHandler::writeFailed, [this]() {
      this->mbar->hide();
      this->msglabel->setText(
          tr("Galat write socket gagal. Anda bisa mencoba ulangi klik tombol "
             "jalankan test."));
      this->msglabel->setStyleSheet(
          "background-color: rgb(255, 88, 32);\
                      color: rgb(0, 0, 0);");
    });
    connect(&bench, &BenchmarkHandler::emitBenchmarkProgress,
            [this](const QByteArray &msg, bool isdone, int progress) {
              if (isdone) {
                this->mbar->hide();
                this->msglabel->setText(tr("Proses benchmark selesai..."));
                return;
              }
              this->mbar->show();
              this->mbar->setValue(progress);
              this->msglabel->setText(msg.constData());
              this->msglabel->setStyleSheet(
                  "background-color: rgb(168, 255, 97); \
              color: rgb(0, 0, 0);");
            });
    connect(&bench, &BenchmarkHandler::invalidReadResult,
            [this](const QString &data) {
              this->mbar->hide();
              this->msglabel->setText(data);
              this->msglabel->setStyleSheet(
                  "background-color: rgb(255, 88, 32);\
                      color: rgb(0, 0, 0);");
            });
    auto returinput = [&](const QString &input, int formatdata, int iterasi,
                          bool saveexcel, int satuanbenchmark) {
      QJsonObject obj;
      obj["inputtest"] = input;
      obj["formatdata"] = formatdata;
      obj["iterasi"] = iterasi;
      obj["saveexcel"] = saveexcel;
      obj["satuanbenchmark"] = satuanbenchmark;
      QJsonDocument doc(obj);
      return doc.toJson(QJsonDocument::Compact);
    };
    bench.handleBaru(returinput(inputdata.toString(), formatdata.toInt(),
                                pengaturan_iterasi_test.toInt(),
                                pengaturan_export_excel.toBool(),
                                pengaturan_satuan_benchmark.toInt()),
                     "skripsi_peg_parser_benchmarkmanager",
                     pengaturan_iterasi_test.toInt());

  } else if (modetesting.toInt() == 2) {
    ismodebenchmark = false;
    QStringList args;
    if (manual_modeverbose.toBool()) {
      args << "-v";
    }
    if (manual_tampilast.toBool()) {
      args << "-a";
    }
    args << "-g" << manual_inputgrammar.toString();
    args << "-i" << manual_inputdata.toString();
    spawnChildProcess(parserlint1.toString(), args);
  } else {
    // mode otomatis test
    ismodebenchmark = false;
    QStringList args;
    args << "--rtx";
    spawnChildProcess(parserlint1.toString(), args);
  }
  if (isvalidconfig) {
    ui->toolButton_prosestesting->setEnabled(true);
    qApp->processEvents();
  }
}

void MainWindow::tampilPenggunaan() {
#ifdef BUILD_WEBKIT
  HelpDialog dlg(this);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }
#endif
 QMessageBox::information(this,tr("Mohon maaf"),tr("Fitur bantuan telah di-disabled karena urgensi kompatibilitas aplikasi dengan windows xp"),QMessageBox::Ok);
}

void MainWindow::versi() {
  QMessageBox::about(
      this, tr("Tentang program"),
      tr("Program unit-testing PEG Parsing Ini versi 1.0,\n"
         "by Ridwan Abdul Hafidh ( ridwanabdulhafidh et gmail dot com )"));
}

void MainWindow::spawnChildProcess(const QString &prog,
                                   const QStringList &args) {
  ParserLintWrapper mp;
  connect(&mp, SIGNAL(emitError(QString)), this, SLOT(onError(QString)));
  connect(&mp, SIGNAL(emitOutput(QString)), this, SLOT(onOutput(QString)));
  connect(&mp, SIGNAL(emitStart()), this, SLOT(onStart()));
  connect(&mp, SIGNAL(emitFinish(int, QProcess::ExitStatus)), this,
          SLOT(onFinish(int, QProcess::ExitStatus)));
  QString dummy("");
  mp.startParser(prog, args, dummy);
}

void MainWindow::on_textEdit_output_customContextMenuRequested(
    const QPoint &pos) {
  QMenu *menu = ui->textEdit_output->createStandardContextMenu();
  menu->addAction("Hapus teks", this, SLOT(onClearOutput()));
  menu->exec(ui->textEdit_output->mapToGlobal(pos));
  menu->deleteLater();
}

void MainWindow::onClearOutput() { ui->textEdit_output->clear(); }

void MainWindow::on_tabWidget_tabBarClicked(int index) {
  ui->stackedWidget->setCurrentIndex(index);
}

void MainWindow::on_toolButton_pengaturantesting_clicked() {
  if (ui->actionKonfigurasi_program->isEnabled()) {
    ui->actionKonfigurasi_program->trigger();
  }
}

void MainWindow::on_toolButton_prosestesting_clicked() {
  qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
           << "ui->actionJalankan_test->isEnabled()"
           << ui->actionJalankan_test->isEnabled();
  if (ui->actionJalankan_test->isEnabled()) {
    ui->actionJalankan_test->trigger();
  } else {
    determineValidConfigtoStartTest();
  }
}

void MainWindow::excelnew() { excelui->newFile(); }

void MainWindow::excelopen() { excelui->open(); }

void MainWindow::excelsave() { excelui->save(); }

void MainWindow::excelsaveas() { excelui->saveAs(); }

void MainWindow::excelundo() { excelui->undo(); }

void MainWindow::excelredo() { excelui->redo(); }

void MainWindow::copy() { excelui->copy(); }

void MainWindow::cut() { excelui->cut(); }

void MainWindow::paste() { excelui->paste(); }

void MainWindow::excelinsertbaris() { excelui->insertRows(); }

void MainWindow::excelinsertkolom() { excelui->insertColumns(); }

void MainWindow::excelinsertgambar() { excelui->insertImage(); }

void MainWindow::excelformatfont() { excelui->font(); }

void MainWindow::excelformatalignleft() { excelui->alignleft(); }

void MainWindow::excelformataligncenter() { excelui->aligncenter(); }

void MainWindow::excelformatalignright() { excelui->alignright(); }

void MainWindow::excelformatbackgroundcolor() { excelui->background(); }

void MainWindow::excelformattextcolor() { excelui->foreground(); }

void MainWindow::excelformatsortir() { excelui->sort(); }
