#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <deque>
namespace Ui {
class MainWindow;
}
namespace excel {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void closeEvent(QCloseEvent *e);
  
  void determineValidConfigtoStartTest();

 protected:
  void changeEvent(QEvent *e);

 private Q_SLOTS:
  void showsettings();
  void readyParse();
  void onError(const QString &e);
  void onOutput(QString e);  // by value
  void onStart();
  void onFinish(int e, QProcess::ExitStatus w);
  void runner();
  void tampilPenggunaan();
  void versi();

  void on_textEdit_output_customContextMenuRequested(const QPoint &pos);

  void onClearOutput();
  void on_tabWidget_tabBarClicked(int index);

  void on_toolButton_pengaturantesting_clicked();

  void on_toolButton_prosestesting_clicked();

  // file
  void excelnew();
  void excelopen();
  void excelsave();
  void excelsaveas();

  // edit
  void excelundo();
  void excelredo();
  void copy();
  void cut();
  void paste();

  // insert
  void excelinsertbaris();
  void excelinsertkolom();
  void excelinsertgambar();

  // format
  void excelformatfont();
  void excelformatalignleft();
  void excelformataligncenter();
  void excelformatalignright();
  void excelformatbackgroundcolor();
  void excelformattextcolor();
  void excelformatsortir();

 private:
  bool checkValidSettings();
  void diagnosaError();

  void spawnChildProcess(const QString &prog, const QStringList &args);
 
  /*
   * generate valid custom progressbar incremental job
   * */
  template <class ForwardIterator, class T>
  void genvalidprogress(ForwardIterator first, ForwardIterator last, T value,
                        T step) {
    while (first != last) {
      *first++ = value;
      value += step;
    }
  }
  Ui::MainWindow *ui;
  QLabel *msglabel;
  QProgressBar *mbar;
  std::deque<float> mbarvalue;
  std::deque<std::pair<int, double> > mnilai;
  std::deque<QPair<int, QString> > m_batcherrorlog;

  excel::MainWindow *excelui;

  // file
  QAction *act_newfile;
  QAction *act_open;
  QAction *act_save;
  QAction *act_saveas;

  // edit
  QAction *act_undo;
  QAction *act_redo;
  QAction *act_copy;
  QAction *act_cut;
  QAction *act_paste;

  // insert
  QAction *act_insertbaris;
  QAction *act_insertkolom;
  QAction *act_insertgambar;

  // format
  QAction *act_formatfont;
  QAction *act_formatalignleft;
  QAction *act_formataligncenter;
  QAction *act_formatalignright;
  QAction *act_formatbackground;
  QAction *act_formattextcolor;
  QAction *act_formatsortir;
  
  QMenu *menuexcelfile;
  QMenu *menuexceledit;
  QMenu *menuexcelinsert;
  QMenu *menuexcelformat;
  bool isvalidconfig=true;
  bool ismodebenchmark=false;
};

#endif  // MAINWINDOW_H
