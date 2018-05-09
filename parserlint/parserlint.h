#ifndef PARSERLINT_H
#define PARSERLINT_H

#include <peglib.h>
#include <memory>
#include <thread>
#include <vector>
#include <fstream>
#include <iostream> 
#include <future>
#include <QFile>
#include <QString>
#include <QLatin1String>

#include "parserdef.h" 

typedef QList<std::pair<QString,QString>> JSONROW;

class ParserLint
{
public:
    ParserLint();
    static ParserDef::errcode parsetest(const char *gram, char * inputtestfile,bool enable_ast,bool use_thread,bool verbose);
    
    static ParserDef::errcode parsecsv(const char*fname, bool verbose);
    
    static ParserDef::errcode parsejson(const char*fname, bool verbose);
    
    static ParserDef::errcode parseselfjson(const char*fname, bool verbose);
    
    static ParserDef::errcode parseselfcsv(const char*fname, bool verbose, bool useheader=false);
    
    
    static bool isExistFile(const QString &file)
    {
        QFile fl(file);
        if (!fl.open (QIODevice::ReadOnly|QIODevice::Text)){
            return false;
        }
        fl.close ();
        return true;
    }
    ~ParserLint(){}
     
private:
    
};

#endif // PARSERLINT_H
