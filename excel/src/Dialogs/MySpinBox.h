#ifndef MYSPINBOX_H
#define MYSPINBOX_H

#include <QSpinBox>
class QRegExpValidator;

class MySpinBox : public QDoubleSpinBox
{
   Q_OBJECT
   
   public:
      MySpinBox(QWidget* = 0);
      void stepBy(int);
      QValidator::State validate(QString&, int&) const;
      
      void setAnotherSpinBox(MySpinBox *asb)
      { anotherSpinBox = asb; }
      
      int getInitialValueOfAspectRatio() const
      { return initialValueOfAspectRatio; }
      void setInitialValueOfAspectRatio(int i)
      { initialValueOfAspectRatio = i; }
      
      int getOriginalValue() const
      { return originalValue; }
      void setOriginalValue(int i)
      { originalValue = initialValueOfAspectRatio = i; }
      
      void setKeepAspectRatio (bool kar)
      { keepAspectRatio = kar; }
      
      void setUnit(int u)
      { unit = u; setValue(value()); /* just to refresh the sping box */}
      
      enum {Pixel, Percent};
      
   protected:
      double valueFromText(const QString&) const;
      QString textFromValue(double) const;
      
   private slots:
      void syncronizeSpinBoxes();
      
   private:
      MySpinBox *anotherSpinBox;
      int initialValueOfAspectRatio;
      int originalValue;
      int unit;
      bool keepAspectRatio;
      
      QRegExpValidator *validator;
};

#endif

