#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include "ui_NewFileDialog.h"
#include <QDialog>

class NewFileDialog : public QDialog, public Ui::NewFileDialog
{
   Q_OBJECT
   
   public:
      NewFileDialog(QWidget* = 0);
};

#endif
