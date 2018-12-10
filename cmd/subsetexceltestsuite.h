#ifndef SUBSETEXCELTESTSUITE_H
#define SUBSETEXCELTESTSUITE_H

#include <QObject>

class SubsetExcelTestsuite : public QObject
{
    Q_OBJECT
public:
    explicit SubsetExcelTestsuite(QObject *parent = 0);
    ~SubsetExcelTestsuite();
    
public slots:
    
private Q_SLOTS:
    void testSimplemath();
    void testMulPower();
    void testComplexArith1();
    
    
};

#endif // SUBSETEXCELTESTSUITE_H