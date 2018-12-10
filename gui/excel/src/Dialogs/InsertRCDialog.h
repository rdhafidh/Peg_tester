#ifndef INSERTRCDIALOG_H
#define INSERTRCDIALOG_H

#include <QDialog>
#include "ui_InsertRCDialog.h"

class InsertRCDialog : public QDialog, public Ui::InsertRCDialog
{
   Q_OBJECT
   
   public:
      InsertRCDialog(int, QWidget* = 0);
      enum {InsertRows, InsertColumns};
};


#endif
