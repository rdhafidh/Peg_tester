#ifndef SORTDIALOG_H
#define SORTDIALOG_H

#include <QDialog>
#include <QTableWidgetSelectionRange>
#include "ui_SortDialog.h"
class TableSort;

class SortDialog : public QDialog, public Ui::SortDialog
{
   Q_OBJECT
   
   public:
      SortDialog(const QTableWidgetSelectionRange&, QWidget* = 0);
      TableSort getTableSort();
      
   private slots:
      void setSortKeys();

   private:
      QTableWidgetSelectionRange range;
};

#endif
