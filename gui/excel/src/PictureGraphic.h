#ifndef PICTUREGRAPHIC_H
#define PICTUREGRAPHIC_H

#include <QImage>
#include <QSize>
#include "Graphic.h"
class QWidget;
class QPaintEvent;

class PictureGraphic : public Graphic
{
   Q_OBJECT
   
   public:
      PictureGraphic(QWidget*, const QImage& = QImage());
      QSize sizeHint() const;
      virtual void writeOutYourSelf(QDataStream&) const;
      virtual void readInYourSelf(QDataStream&);
      virtual QSize getOriginalSize() const;
      
   protected:
      virtual void drawInImage(QImage&);
      
   private:
      QImage image;
};


#endif
