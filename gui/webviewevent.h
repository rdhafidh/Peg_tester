#ifndef WEBVIEWEVENT_H
#define WEBVIEWEVENT_H

#include <QtCore>
#include <QtGui>
#include <QMenu>
#include <QWebView>
class WebViewEvent : public QObject
{
    Q_OBJECT
public:
    explicit WebViewEvent(QObject *parent = 0);
    bool eventFilter(QObject *obj, QEvent *event);
Q_SIGNALS:
     void emitsavepriview();
public Q_SLOTS:
    void savepriview();
private:
    
};

#endif // WEBVIEWEVENT_H
