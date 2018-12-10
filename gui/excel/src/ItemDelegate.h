#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QItemDelegate>
class QWidget;
class QStyleOptionViewItem;
class QModelIndex;
class QAbstractItemModel;

class ItemDelegate : public QItemDelegate
{
   Q_OBJECT
   
   public:
      ItemDelegate(QAbstractItemDelegate*, QObject* = 0);
      QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const;
      void setEditorData(QWidget*, const QModelIndex&) const;
      void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const;
      
   signals:
      void dataChangedSignal(int, int, QString&, QString&) const;
      
   private:
      QAbstractItemDelegate *lastItemDelegate;
};

#endif
