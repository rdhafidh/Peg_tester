#ifndef EVALBASE_H
#define EVALBASE_H
#include <QString>
#include <QVariant>
#include <sstream>
#include <utility>
#include <cstdlib>
#include <peglib.h>

#ifdef BUILDNOEXCELUI
struct Table{
    
};
#else
class Table;
#endif

struct SymbolScope {
  SymbolScope(std::shared_ptr<SymbolScope> outer) : outer(outer) {}

 private:
  std::shared_ptr<SymbolScope> outer;
};
struct Annotation {
  std::shared_ptr<SymbolScope> scope;
};

typedef peg::AstBase<Annotation> astcalculator;
std::string format_error_message(const std::string& path, size_t ln, size_t col,
                                 const std::string& msg);
void throw_runtime_error(const std::shared_ptr<astcalculator> node,
                         const std::string& msg);

void throw_runtime_error(const std::string& node, const std::string& msg);
class EvalBase {
 public:
  EvalBase();
  ~EvalBase();
  static QVariant eval(const std::shared_ptr<astcalculator> ast, Table *t,int row, int col);
  
  static bool isFormula(const QString &formula);
  
  static double toNumberFromVariant(const QVariant & var);

 private:
  static QVariant evalFieldexcel(const std::shared_ptr<astcalculator> ast, Table *t, int row, int col);
  static double valueOfCell(std::shared_ptr<astcalculator> ast, Table*table, const QString &locationOfOtherCell
                            ,int currentRow,int currentColumn);
  
  static QVariant evalExpressionCmp(const std::shared_ptr<astcalculator> ast,int row,int col);
 
  static QVariant evalExpIfTail(const std::shared_ptr<astcalculator> ast, int row, int col);
  
   static QVariant eval_exprifcmpor(const std::shared_ptr<astcalculator> ast, int row, int col);
  
   static QVariant eval_exprifcmpand(const std::shared_ptr<astcalculator> ast, int row, int col);
  
  static QVariant eval_exprifvalue(const std::shared_ptr<astcalculator> ast, int row, int col);

  static QVariant eval_expression(const std::shared_ptr<astcalculator> ast,int row,int col);

  static QVariant eval_term(const std::shared_ptr<astcalculator> ast, int row, int col);
  
  static QVariant eval_factor(const std::shared_ptr<astcalculator> ast, int row, int col);
  
  static QVariant eval_stringvalue(const std::shared_ptr<astcalculator> ast);

  static QVariant eval_string(const std::shared_ptr<astcalculator> ast);

  static QVariant eval_number(const std::shared_ptr<astcalculator> ast);

  static QVariant eval_numberdouble(const std::shared_ptr<astcalculator> ast);

  static QVariant eval_fractionalconstant(
      const std::shared_ptr<astcalculator> ast);

  static QVariant eval_exponentpart(const std::shared_ptr<astcalculator> ast);
  
  static std::pair<bool, std::shared_ptr<astcalculator> > isexistFieldExcel(const std::shared_ptr<astcalculator> ast,const char* func);

  static QVariant eval_power(const std::shared_ptr<astcalculator> ast, int row, int col);

  static QVariant eval_funcaverage(const std::shared_ptr<astcalculator> ast, int row, int col);

  static QVariant eval_funcmax(const std::shared_ptr<astcalculator> ast, int row, int col);

  static QVariant eval_funcmin(const std::shared_ptr<astcalculator> ast, int row, int col);

  static QVariant eval_function(const std::shared_ptr<astcalculator> ast, int row, int col);
  
  static QVariant eval_funcif(const std::shared_ptr<astcalculator> ast, int row, int col);
  
  static QVariant eval_funcor(const std::shared_ptr<astcalculator> ast, int row, int col);
  
  static QVariant eval_funcand(const std::shared_ptr<astcalculator> ast, int row, int col);

  static QVariant eval_funcdate(const std::shared_ptr<astcalculator> ast);

  static std::pair<bool, QString> eval_stringtime(const QString& t);
  
  static QVariant eval_funcnot(const std::shared_ptr<astcalculator> ast, int row, int col);

  static std::pair<bool, QString> eval_stringdate(const QString& d);

  static QVariant eval_funcsum(const std::shared_ptr<astcalculator> ast, int row, int col);
  
  static QList<double> extractDataRangeFromCell(const std::shared_ptr<astcalculator> ast, const QString args, Table *table, int row, int col);
  
  static bool isRange(const QString &formula, int &iterator);
  static void ignoreSpaces(const QString &formula, int &iterator);

  static QVariant eval_eq(const std::shared_ptr<astcalculator> ast, int row, int col);
  
  static void expectNodeSize(const std::shared_ptr<astcalculator> ast,const char *func,std::size_t n);
  
  static void expectTwoSameVariantType(const std::shared_ptr<astcalculator> ast, const char*func, QVariant &a,
                                       QVariant &b);
  static bool evalBooleanFromVariant(const QVariant &var);
  
  
  inline static bool isoverflowDoubleValueHappen(const std::shared_ptr<astcalculator> ast);
  inline static void throwIfNotNumberVariant(QVariant &var, const std::shared_ptr<astcalculator> ast, const char *msg);
  inline static void throwIfNotNumberVariant(QList<QVariant> &var, const std::shared_ptr<astcalculator> ast, const char *msg);
};

#endif  // EVALBASE_H