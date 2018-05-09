#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QWidget>
#include <QSize>
#include <QPaintEvent>
#include <QPoint>
#include <QDataStream>
#include <QPoint>
class QPainter;
class QImage;

class Graphic: public QWidget
{
   Q_OBJECT
   
   public:
      Graphic(QWidget* = 0);
      
      enum BoundaryPosition{TopLeft, TopRight, BottomLeft, BottomRight};

      void out(QDataStream&) const;
      void in(QDataStream&);
      virtual QSize getOriginalSize() const = 0;
      QSize getDisplaySize() const;
      void moveManual(int, int);
      void moveManual(const QPoint&);
      void scale(int, int, Qt::AspectRatioMode, BoundaryPosition);
      void scale(const QSize&);
      
      QPoint getPosisiDiTable()const ;
      
   public slots:
      void execPropertiesDialog();
      
   signals:
      void graphicDraggedSignal(Graphic*, QPoint, QPoint);
      void graphicResizedSignal(Graphic*, QSize, QSize, Graphic::BoundaryPosition);
      void graphicSelectedSignal(Graphic*);
      void graphicContextMenuSignal(QPoint);
      void somethingChangedSignal();
      
   protected:
      void mousePressEvent(QMouseEvent*);
      void mouseMoveEvent(QMouseEvent*);
      void mouseReleaseEvent(QMouseEvent*);
      void mouseDoubleClickEvent(QMouseEvent*)
           {execPropertiesDialog();}
      void contextMenuEvent(QContextMenuEvent*);
      void paintEvent(QPaintEvent*);
      virtual void drawInImage(QImage&) = 0;
      virtual void writeOutYourSelf(QDataStream&) const = 0;
      virtual void readInYourSelf(QDataStream&) = 0;
      
      
   private:
      void drawBoundary(QPainter&);
      //returns the BoundaryPosition (enum) of the mouse in graphic
      //returns -1 for unknown position
      int whereIsMouse(const QPoint&) const;
      
      QSize scaleSize;

      QPoint pressedPosition;
      QPoint initialGraphicPosition;
      BoundaryPosition centreOfScale;
      QSize initialGraphicSize;
      bool scaling;
      bool dragging;
};



#endif

