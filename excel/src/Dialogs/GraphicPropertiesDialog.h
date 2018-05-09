#ifndef GRAPHICPROPERTIESDIALOG_H
#define GRAPHICPROPERTIESDIALOG_H

#include <QDialog>
#include <QSize>
#include "MySpinBox.h"
#include "ui_GraphicPropertiesDialog.h"

class GraphicPropertiesDialog : public QDialog, public Ui::GraphicPropertiesDialog
{
   Q_OBJECT
   
   public:
      GraphicPropertiesDialog(const QSize&, const QSize&, QWidget* = 0);
      QSize getModifiedSizeProperty() const;
      
   private slots:
      void keepAspectRatioSlot(int);
      void originalSizeSlot(int);
      void unitChangedSlot(int);
      
   private:
      QSize originalSize;
};

#endif
