#ifndef CALCULATORPEG_H
#define CALCULATORPEG_H

#include <QVariant>


#ifdef BUILDNOEXCELUI
struct Table;
#else
class Table;
#endif


class CalculatorPEG
{
public:
    CalculatorPEG();
    ~CalculatorPEG();
    #ifndef BUILDNOEXCELUI
     QVariant hitung(Table *t,int row,int col);
     
#endif
     Table *getTable();
     QVariant hitung(const QString &formulas);
     
     static  CalculatorPEG *newInstance();
     static bool isformula(const QString &data);
    
private:
#ifndef BUILDNOEXCELUI
     Table *table;
     QString lastformula;
     QVariant lastvariant;
#endif
};

#endif // CALCULATORPEG_H