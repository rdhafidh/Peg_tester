#include "webviewevent.h"

WebViewEvent::WebViewEvent(QObject *parent) : QObject(parent)
{ 
}

bool WebViewEvent::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type()==QEvent::ContextMenu){
        QWebView *web=qobject_cast<QWebView*>(obj); 
        QMenu menu(web);  
        menu.addAction(tr("save priview"),this,SLOT(savepriview()));
        QContextMenuEvent *evt=static_cast<QContextMenuEvent*>(event);
        menu.exec(evt->globalPos());
        return true;
    }
    else {
            // standard event processing
            return QObject::eventFilter(obj, event);
    }
}

void WebViewEvent::savepriview()
{
    Q_EMIT emitsavepriview();
}

