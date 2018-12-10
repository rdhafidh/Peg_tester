#ifndef SETTINGPROGRAM_H
#define SETTINGPROGRAM_H

#include <QFrame>

namespace Ui {
class SettingProgram;
}

class SettingProgram : public QFrame
{
    Q_OBJECT
    
public:
    explicit SettingProgram(QWidget *parent = 0);
    ~SettingProgram();
    void closeEvent(QCloseEvent * event);
    
public Q_SLOTS:
    bool saveSetting();
    void checkValidExeFile(const QString & file);
    void validsetting();
    void on_ok();
    bool isTextFile(const QString &file);
    
Q_SIGNALS:
    void askreloadsetting();
    
protected:
    void changeEvent(QEvent *e);
    
private slots:
    void on_toolButton_parserlint1_clicked();
    
    void on_toolButton_mode_benchmark_pilih_export_data_clicked();
    
    void on_toolButton_manual_test_pilih_file_grammar_clicked();
    
    void on_toolButton_manual_test_input_file_test_clicked();
       
    void on_comboBox_mode_test_apakah_itu_currentIndexChanged(int index);
    
private:
    void setSetting();
    bool isExistFile(const QString & file);
    void checkRequiredFile();
    Ui::SettingProgram *ui;
    bool valid;
};

#endif // SETTINGPROGRAM_H
