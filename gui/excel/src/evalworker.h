#ifndef EVALWORKER_H
#define EVALWORKER_H
#include <qstring.h>

class EvalWorker
{
public:
    EvalWorker();
    bool tryEval(const char*g, const QByteArray &data,const char*path);
};

#endif // EVALWORKER_H