#ifndef JSONSOCKETPARSERWORKAROUND_H
#define JSONSOCKETPARSERWORKAROUND_H

#include <QList>
#include <QByteArray>

class JsonSocketParserWorkaround
{
public:
    JsonSocketParserWorkaround();
  static   QList<QByteArray> fixmultiLinejson(const QByteArray &rawdata);
    
};

#endif // JSONSOCKETPARSERWORKAROUND_H