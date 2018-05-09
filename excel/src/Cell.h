

#ifndef CELL_H
#define CELL_H

#include <QTableWidgetItem>
#include <QDataStream>

class Cell : public QTableWidgetItem
{
   friend QDataStream& operator<<(QDataStream&, const Cell&);
   friend QDataStream& operator>>(QDataStream&, Cell&);
   
   public:
      Cell();
      QTableWidgetItem* clone() const;
      QVariant data(int) const;
      void setData(int, const QVariant&);
   
   private:
      int editor;
};

#endif
