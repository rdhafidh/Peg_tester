#include "subsetexceltestsuite.h"
#include <QtTest>
#include <calculatorpeg.h>

SubsetExcelTestsuite::SubsetExcelTestsuite(QObject *parent) : QObject(parent)
{
    
}

SubsetExcelTestsuite::~SubsetExcelTestsuite()
{
    
}

void SubsetExcelTestsuite::testSimplemath()
{
    QVERIFY( 4==CalculatorPEG::newInstance ()->hitung ("=6-2"));    
}

void SubsetExcelTestsuite::testMulPower()
{
    QVERIFY( 365.238672==CalculatorPEG::newInstance ()->hitung ("=3,2+2*powER(2;5,5+2)"));    
}

void SubsetExcelTestsuite::testComplexArith1()
{
    QVERIFY(177.46==CalculatorPEG::newInstance ()->hitung ("=20,3/2+(-10,4+20,3)*3,9+2-3/10+(13*12)-(12+(3*4)+5)"));
}
