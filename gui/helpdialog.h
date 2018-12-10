#ifndef HELPDIALOG_H
#define HELPDIALOG_H
 
#include <QtWebKitWidgets/QtWebKitWidgets>
#include <QtWidgets>
#include <QtCore>
#include <deque>
namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit HelpDialog(QWidget *parent = 0);
    ~HelpDialog();
    
public Q_SLOTS:
    void bukaHalaman(const QUrl &url=QUrl());
    
    
protected:
    void changeEvent(QEvent *e);
    
private Q_SLOTS:
    void on_savePriview();
    void on_commandLinkButton_kembali_clicked();
    
    void on_commandLinkButton_refresh_clicked();
    
    void on_commandLinkButton_forward_clicked();
    
    void on_pushButton_ok_clicked();
    
private:
    
    Ui::HelpDialog *ui;
    std::deque<QUrl> mhistory;
    QAction *web_back,*web_forward,*web_reload;
};

#endif // HELPDIALOG_H
