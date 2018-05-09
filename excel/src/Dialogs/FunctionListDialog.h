
#include <QDialog>
#include "ui_FunctionListDialog.h"

class FunctionListDialog : public QDialog, public Ui::FunctionListDialog
{
   Q_OBJECT
   
   public:
      FunctionListDialog(QWidget* = 0);
};

