#include "helpdialog.h"
#include "ui_helpdialog.h"
#include "webviewevent.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    WebViewEvent *w=new WebViewEvent(this);
    connect(w,SIGNAL(emitsavepriview()),SLOT(on_savePriview()));
    ui->webView->installEventFilter(w);
    web_back=ui->webView->pageAction(QWebPage::Back);
    web_forward=ui->webView->pageAction(QWebPage::Forward);
    web_reload=ui->webView->pageAction(QWebPage::Reload);
    //web_stop=ui->webView->pageAction(QWebPage::Stop);
    QUrl r("file:///"+QDir::currentPath()+"/doc/html/index.html");
    bukaHalaman(r);
}

HelpDialog::~HelpDialog()
{
    delete ui;
    web_back->deleteLater();
    web_forward->deleteLater();
    web_reload->deleteLater();
  //  mhistory.clear();
}

void HelpDialog::bukaHalaman(const QUrl &url)
{
    if (url.isEmpty() || !url.isLocalFile() || !url.isValid()){
        return;
    }
    //mhistory.push_back(url);
    QEventLoop evt;  
    ui->webView->setUrl(url); 
    connect(ui->webView,SIGNAL(loadFinished(bool)),&evt,SLOT(quit()));
    evt.exec();
}

void HelpDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void HelpDialog::on_savePriview()
{
    QString format = "png";
    QPixmap originalPixmap=ui->webView->grab();
        QString initialPath = QDir::currentPath() + tr("/untitled.") + format;
    
        QString fileName = QFileDialog::getSaveFileName(this, tr("Simpan screenshot dengan nama"), initialPath,
                                                        tr("%1 Files (*.%2);;All Files (*)")
                                                        .arg(format.toUpper())
                                                        .arg(format));
        if (!fileName.isEmpty())
            originalPixmap.save(fileName, format.toLatin1().constData());
}

void HelpDialog::on_commandLinkButton_kembali_clicked()
{ 
    web_back->trigger();
}

void HelpDialog::on_commandLinkButton_refresh_clicked()
{
 web_reload->trigger();   
}

void HelpDialog::on_commandLinkButton_forward_clicked()
{
    web_forward->trigger();
}

void HelpDialog::on_pushButton_ok_clicked()
{
    accept();
}
