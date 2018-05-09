#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include "ui_FindDialog.h"

class FindDialog : public QDialog, public Ui::FindDialog
{
   Q_OBJECT
   
   public:
      FindDialog(QWidget* = 0);
      Qt::CaseSensitivity caseSensitivity() const;
      int inFormula() const;
      QString text() const;
      
   signals:
      void findNextSignal();
      void findPreviousSignal();
      
   private slots:
      void updateFindButton();
      void findButtonClicked();
};

#endif
