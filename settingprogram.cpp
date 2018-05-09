#include "settingprogram.h"
#include <lastfiledialogfolderdir.h>
#include <QtCore>
#include <QtWidgets>
#include "settinghandler.h"
#include "ui_settingprogram.h"

SettingProgram::SettingProgram(QWidget *parent)
    : QFrame(parent, Qt::Dialog | Qt::Window), ui(new Ui::SettingProgram) {
  ui->setupUi(this);
  // set up everything..
  QFile fl(QDir::currentPath() + "/pengaturan.ini");
  if (!fl.exists()) {
    // awal kali

  } else {
    // ke sekian kali
    this->setSetting();
    checkRequiredFile();
  }
  fl.close();

  connect(ui->pushButton_simpan, SIGNAL(pressed()), SLOT(validsetting()));
  connect(ui->pushButton_ok, SIGNAL(pressed()), SLOT(on_ok()));

  connect(ui->lineEdit_parserlint1, SIGNAL(textEdited(QString)),
          SLOT(checkValidExeFile(QString)));
}

SettingProgram::~SettingProgram() { delete ui; }

void SettingProgram::closeEvent(QCloseEvent *event) {
  this->askreloadsetting();
  event->accept();
}

bool SettingProgram::saveSetting() {
  // Intinya ada 2 test
  // 1. mode benchmark
  // 2. mode test manual
  // 3. mode test otomatis

  auto savemodetestbenchmark = [this]() {
    SettingHandler::setSetting(
        "modebenchmark", "formatdata",
        ui->comboBox_benchmark_pilih_formatdata->currentIndex());
    SettingHandler::setSetting(
        "modebenchmark", "inputdata",
        ui->lineEdit_input_data_export_phpmyadmin->text());
    SettingHandler::setSetting(
        "modebenchmark", "pengaturan_iterasi_test",
        ui->spinBox_pengaturan_benchmark_iterasi_test->value());
    SettingHandler::setSetting(
        "modebenchmark", "pengaturan_export_excel",
        ui->checkBox_pengaturan_benchmark_export_file->isChecked());
    SettingHandler::setSetting(
        "modebenchmark", "pengaturan_satuan_benchmark",
        ui->comboBox_pengaturan_benchmark_satuan_benchmark->currentIndex());
  };
  auto savemodetestmanual = [this]() {
    SettingHandler::setSetting("modetest", "manual_inputgrammar",
                               ui->lineEdit_manual_test_input_grammar->text());
    SettingHandler::setSetting("modetest", "manual_inputdata",
                               ui->lineEdit_manual_test_input_file->text());
    SettingHandler::setSetting(
        "modetest", "manual_tampilast",
        ui->checkBox_manual_test_tampilkan_ast->isChecked());
    SettingHandler::setSetting(
        "modetest", "manual_modeverbose",
        ui->checkBox_manual_test_mode_verbose->isChecked());
  };
  //  auto savemodetestotomatis=[this](){
  //      SettingHandler::setSetting ("modetest",
  //      "otomatis_pengetesan",ui->comboBox_test_otomatis_pilih_pengetesan->currentIndex
  //      ());
  //      SettingHandler::setSetting ("modetest",
  //      "otomatis_inputfolderdata",ui->lineEdit_otomatis_test_input_folder_isi_filetest->text
  //      ());
  //  };

  SettingHandler::setSetting("parserlint", "parserlint1",
                             ui->lineEdit_parserlint1->text());
  switch (ui->comboBox_mode_test_apakah_itu->currentIndex()) {
    case 0:
      savemodetestbenchmark();
      SettingHandler::setSetting("parserlint", "modetesting", 1);
      break;
    default:
      savemodetestmanual();
      SettingHandler::setSetting("parserlint", "modetesting", 2);
      break;
  }
  return true;
}

void SettingProgram::checkValidExeFile(const QString &file) {
  QFile fl(file);
#ifdef Q_OS_WIN
  if (fl.exists() && file.endsWith(".exe")) {
#else
  if (fl.exists()) {
#endif
    valid = true;
    auto wgt = qobject_cast<QLineEdit *>(sender());
    if (wgt && wgt->isWidgetType()) {
      wgt->setStyleSheet(" background-color: rgb(164, 255, 78) ");
      ui->pushButton_simpan->setEnabled(true);
    }
  } else {
    valid = false;
    auto wgt = qobject_cast<QLineEdit *>(sender());
    if (wgt && wgt->isWidgetType()) {
      wgt->setStyleSheet(" background-color: rgb(255, 183, 120) ");
      ui->pushButton_simpan->setEnabled(false);
    }
  }
  fl.close();
}

void SettingProgram::validsetting() {
  saveSetting();
  close();
}

void SettingProgram::on_ok() { close(); }

bool SettingProgram::isTextFile(const QString &file) {
  QFile fl(file);
  if (fl.open(QIODevice::ReadOnly | QIODevice::Text)) {
    fl.close();
    return true;
  }

  return false;
}
void SettingProgram::changeEvent(QEvent *e) {
  QFrame::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void SettingProgram::setSetting() {
  QFileInfo inf;
  auto popuimodebenchmark = [this, &inf]() {
    // check format data
    auto var1 = SettingHandler::getSetting("modebenchmark", "formatdata");
    switch (var1.toInt()) {
      case 0:
      case 1:
        ui->comboBox_benchmark_pilih_formatdata->setCurrentIndex(var1.toInt());
        break;
      default:
        ui->comboBox_benchmark_pilih_formatdata->setCurrentIndex(0);
        break;
    }
    var1 = SettingHandler::getSetting("modebenchmark", "inputdata");
    inf.setFile(var1.toString());
    if (!inf.isFile() || !inf.isReadable()) {
      ui->lineEdit_input_data_export_phpmyadmin->setText("");
    } else {
      ui->lineEdit_input_data_export_phpmyadmin->setText(var1.toString());
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
        ui->spinBox_pengaturan_benchmark_iterasi_test->setValue(var1.toInt());
        break;
      default:
        ui->spinBox_pengaturan_benchmark_iterasi_test->setValue(10);
        break;
    }
    var1 =
        SettingHandler::getSetting("modebenchmark", "pengaturan_export_excel");
    ui->checkBox_pengaturan_benchmark_export_file->setChecked(var1.toBool());
    var1 = SettingHandler::getSetting("modebenchmark",
                                      "pengaturan_satuan_benchmark");
    switch (var1.toInt()) {
      case 0:
      case 1:
      case 2:
        ui->comboBox_pengaturan_benchmark_satuan_benchmark->setCurrentIndex(
            var1.toInt());
        break;
      default:
        ui->comboBox_pengaturan_benchmark_satuan_benchmark->setCurrentIndex(2);
        break;
    }

  };
  auto popuimodetestmanual = [this, &inf]() {

    auto var1 = SettingHandler::getSetting("modetest", "manual_inputgrammar");
    inf.setFile(var1.toString());
    if (!inf.isFile() || !inf.isReadable()) {
      ui->lineEdit_manual_test_input_grammar->setText("");
    } else {
      ui->lineEdit_manual_test_input_grammar->setText(var1.toString());
    }
    var1 = SettingHandler::getSetting("modetest", "manual_inputdata");
    inf.setFile(var1.toString());
    if (!inf.isFile() || !inf.isReadable()) {
      ui->lineEdit_manual_test_input_file->setText("");
    } else {
      ui->lineEdit_manual_test_input_file->setText(var1.toString());
    }
    var1 = SettingHandler::getSetting("modetest", "manual_tampilast");
    ui->checkBox_manual_test_tampilkan_ast->setChecked(var1.toBool());
    var1 = SettingHandler::getSetting("modetest", "manual_modeverbose");
    ui->checkBox_manual_test_mode_verbose->setChecked(var1.toBool());
  };

  // Intinya ada 2 test
  // 1. mode benchmark
  // 2. mode test manual

  auto var1 = SettingHandler::getSetting("parserlint", "parserlint1");
  inf.setFile(var1.toString());
  if (var1.toString().isEmpty() || !inf.isFile() || !inf.isExecutable()) {
    ui->lineEdit_parserlint1->setText("");
  } else {
    ui->lineEdit_parserlint1->setText(var1.toString());
  }
  var1 = SettingHandler::getSetting("parserlint", "modetesting");

  switch (var1.toInt()) {
    case 1:
      ui->comboBox_mode_test_apakah_itu->setCurrentIndex(0);
      on_comboBox_mode_test_apakah_itu_currentIndexChanged(0);
      popuimodebenchmark();
      break;
    case 2:
      ui->comboBox_mode_test_apakah_itu->setCurrentIndex(1);
      on_comboBox_mode_test_apakah_itu_currentIndexChanged(1);
      popuimodetestmanual();
      break;
    default:
      ui->comboBox_mode_test_apakah_itu->setCurrentIndex(0);
      popuimodebenchmark();
      on_comboBox_mode_test_apakah_itu_currentIndexChanged(0);
      break;
  }
}

bool SettingProgram::isExistFile(const QString &file) {
  QFile fl(file);
  if (!fl.exists()) {
    return false;
  }

  return true;
}

void SettingProgram::checkRequiredFile() {
  if (!ui->lineEdit_parserlint1->text().isEmpty()) {
    if (!isExistFile(ui->lineEdit_parserlint1->text())) {
      ui->lineEdit_parserlint1->setStyleSheet(
          "background-color: rgb(255, 183, 120)");
    } else {
      ui->lineEdit_parserlint1->setStyleSheet(
          "background-color: rgb(164, 255, 78)");
    }
  }
}

void SettingProgram::on_toolButton_parserlint1_clicked() {
  QString dir = ".";
  bool seemsokexe = false;
  if (!ui->lineEdit_parserlint1->text().isEmpty()) {
    if (this->isExistFile(ui->lineEdit_parserlint1->text())) {
      QFileInfo inf(ui->lineEdit_parserlint1->text());
      dir = inf.absolutePath();
      seemsokexe = true;
      LastFileDialogFolderDir::instance()->setLastdir(dir);
    }
  }
#ifdef Q_OS_WIN
  QString exefile = QFileDialog::getOpenFileName(
      this, tr("pilih parserlint exe file"),
      LastFileDialogFolderDir::instance()->getLastDirUsed(), "*.exe");
#else
  QString exefile = QFileDialog::getOpenFileName(
      this, tr("pilih parserlint program file"),
      LastFileDialogFolderDir::instance()->getLastDirUsed(), "parserlint*");
#endif
  QFile fl(exefile);
  if (exefile.isEmpty() || !fl.exists()) {
    if (!seemsokexe) {
      ui->lineEdit_parserlint1->setStyleSheet(
          "background-color: rgb(255, 183, 120)");
    }
    fl.close();
    return;  // should emmit error here;
  }
  LastFileDialogFolderDir::instance()->setLastdir(exefile);
  ui->lineEdit_parserlint1->setText(exefile);
  ui->lineEdit_parserlint1->setStyleSheet(
      "background-color: rgb(164, 255, 78)");

  fl.close();
}

void SettingProgram::on_toolButton_mode_benchmark_pilih_export_data_clicked() {
  QString exefile;
  if (ui->comboBox_benchmark_pilih_formatdata->currentIndex() == 0) {
    exefile = QFileDialog::getOpenFileName(
        this, tr("pilih input file test CSV file dari export data phpmyadmin"),
        LastFileDialogFolderDir::instance()->getLastDirUsed(), "*.csv");
  }
  if (ui->comboBox_benchmark_pilih_formatdata->currentIndex() == 1) {
    exefile = QFileDialog::getOpenFileName(
        this, tr("pilih input file test JSON file dari export data phpmyadmin"),
        LastFileDialogFolderDir::instance()->getLastDirUsed(), "*.json");
  }
  if (exefile.isEmpty()) return;

  QFile fl(exefile);
  if (!fl.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Galat"),
                         tr("Input file %1 tidak bisa dibuka").arg(exefile));
    return;
  }
  fl.close();
  LastFileDialogFolderDir::instance()->setLastdir(exefile);
  ui->lineEdit_input_data_export_phpmyadmin->setText(exefile);
}

void SettingProgram::on_toolButton_manual_test_pilih_file_grammar_clicked() {
  QString exefile;
  exefile = QFileDialog::getOpenFileName(
      this, tr("pilih input  test file peg grammar"),
      LastFileDialogFolderDir::instance()->getLastDirUsed(), "*.peg");
  if (exefile.isEmpty()) return;

  QFile fl(exefile);
  if (!fl.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Galat"),
                         tr("Input file %1 tidak bisa dibuka").arg(exefile));
    return;
  }
  fl.close();
  LastFileDialogFolderDir::instance()->setLastdir(exefile);
  ui->lineEdit_manual_test_input_grammar->setText(exefile);
}

void SettingProgram::on_toolButton_manual_test_input_file_test_clicked() {
  QString exefile;
  exefile = QFileDialog::getOpenFileName(
      this, tr("pilih input  test file peg grammar"),
      LastFileDialogFolderDir::instance()->getLastDirUsed());
  if (exefile.isEmpty()) return;

  QFile fl(exefile);
  if (!fl.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Galat"),
                         tr("Input file %1 tidak bisa dibuka").arg(exefile));
    return;
  }
  fl.close();
  LastFileDialogFolderDir::instance()->setLastdir(exefile);
  ui->lineEdit_manual_test_input_file->setText(exefile);
}

void SettingProgram::on_comboBox_mode_test_apakah_itu_currentIndexChanged(
    int index) {
  auto enablebench = [this]() {
    ui->groupBox_aktif_benchmark->setEnabled(true);
    ui->groupBox_aktif_test->setEnabled(false);
  };
  auto enablemanualtest = [this]() {
    ui->groupBox_aktif_benchmark->setEnabled(false);
    ui->groupBox_aktif_test->setEnabled(true);
  };

  switch (index) {
    case 0:
      enablebench();
      break;
    case 1:
      enablemanualtest();
      break;
    default:
      enablebench();
      break;
  }
}
