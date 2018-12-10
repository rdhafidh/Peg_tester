#ifndef XLSWRITER_H
#define XLSWRITER_H
#include <xlsxdocument.h>
#include <xlsxformat.h>
#include <QtCore>
#include <utility> 
#include <deque>
#include <iterator>
#include  <algorithm>
class XlsWriter
{
public:
    XlsWriter();
    ~XlsWriter(){}
    template<class ForwardIterator, class T>
    static void genNumber(ForwardIterator first, ForwardIterator last, T value,T step)
    { 
            while(first != last) {
            *first++ = value;
            value +=step;
            } 
    }
    static bool writeReport(const QString &filename, std::deque<std::pair<int, double> > d, const std::pair<QString,QString> & compiler, const QString &satuan);
	static bool writeReportSingleBenchmark(const QString &filename, std::deque< std::pair<int,double> > d, const QString & compiler, const QString &satuan);
private: 
    /*
     * total data index =mdata.size()*4, urutanya
     * 1. index 1 -> non thread vs
     * 2. index 2 -> thread vs
     * 3. index 3 -> non thread mingw
     * 4. index 4 -> thread mingw
     * 
     * */
};

#endif // XLSWRITER_H
