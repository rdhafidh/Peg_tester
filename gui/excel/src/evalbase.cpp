#include "evalbase.h"
#ifndef BUILDNOEXCELUI
#include <Table.h>
#endif
#include <calculatorpeg.h>
#include <testoverflow.h>
#include <QtCore>
#include <algorithm>
#include <functional>

EvalBase::EvalBase() {}

EvalBase::~EvalBase() {}

QVariant EvalBase::eval(const std::shared_ptr<astcalculator> ast, Table* t,
                        int row, int col) {
  using namespace peg;
  QVariant temp;

  switch (ast->tag) {
    case "EXPRESSION"_: {
      temp = eval_expression(ast, row, col);
      return temp;
    }
    case "EXPRESSIONCMP"_: {
      temp = evalExpressionCmp(ast, row, col);
      return temp;
    }
    case "FIELDEXCEL"_: {
      temp = evalFieldexcel(ast, t, row, col);
      return temp;
    }
    case "BARISKOLOMEXCEL"_: {
      temp = evalFieldexcel(ast, t, row, col);
      return temp;
    }
    case "EXPIFTAIL"_: {
      temp = evalExpIfTail(ast, row, col);
      return temp;
    }
    case "EXPIFVALUE"_: {
      temp = eval_exprifvalue(ast, row, col);
      return temp;
    }
    case "NUMBERDOUBLE"_: {
      temp = eval_numberdouble(ast);
      return temp;
    }
    case "NUMBER"_: {
      temp = eval_number(ast);
      return temp;
    }
    case "FUNCTION"_: {
      temp = eval_function(ast, row, col);
      return temp;
    }
    case "TERM"_: {
      temp = eval_term(ast, row, col);
      return temp;
    }
    case "STRINGVALUE"_: {
      temp = eval_stringvalue(ast);
      return temp;
    }
    case "FRACTIONALCONSTANT"_: {
      temp = eval_fractionalconstant(ast);
      return temp;
    }
    case "EXPONENTPART"_: {
      temp = eval_exponentpart(ast);
      return temp;
    }
    case "FACTOR"_: {
      temp = eval_factor(ast, row, col);
      return temp;
    }
    case "STRING"_: {
      temp = eval_string(ast);
      return temp;
    }
    case "BENAR"_: {
      temp = true;
      return temp;
    }
    case "SALAH"_: {
      temp = false;
      return temp;
    }
    default: {
      for (auto node : ast->nodes) {
#ifdef ASKDEBUG
        qDebug() << "default executed!, token" << node->token.c_str()
                 << "node size" << ast->nodes.size();
#endif
        temp = eval(node, t, row, col);
      }
      return temp;
    }
  }
}

QVariant EvalBase::evalFieldexcel(const std::shared_ptr<astcalculator> ast,
                                   Table* t, int row, int col) {
  /*
FIELDEXCEL <- BARISKOLOMEXCEL SEPERATORFIELDEXCEL BARISKOLOMEXCEL
                      / BARISKOLOMEXCEL

BARISKOLOMEXCEL <- KOLOMEXCEL NUMBER
KOLOMEXCEL <- [a-zA-Z]+
SEPERATORFIELDEXCEL <- ':'
   */
  const auto& nodes = ast->nodes;
#ifdef ASKDEBUG
  qDebug() << "tagname" << ast->name.c_str();
#endif
  // isexistFieldExcel (ast,Q_FUNC_INFO);
  switch (nodes.size()) {
    case 1:
      return valueOfCell(nodes.at(0), t,
                         QString("%1%2")
                             .arg(nodes.at(0)->nodes.at(0)->token.c_str())
                             .arg(nodes.at(0)->nodes.at(1)->token.c_str()),
                         row, col);
    case 2:
#ifdef ASKDEBUG
      qDebug() << Q_FUNC_INFO << __LINE__ << nodes.size()
               << "child size index 0 " << nodes.at(0)->nodes.size() << "token "
               << nodes.at(0)->token.c_str();
#endif
      return valueOfCell(nodes.at(0), t, QString("%1%2")
                                             .arg(nodes.at(0)->token.c_str())
                                             .arg(nodes.at(1)->token.c_str()),
                         row, col);
    case 3:
      throw_runtime_error(ast, "#VALUE!");
    default:
      throw_runtime_error(ast, "unhandled kalukalsi field excel");
  }
}

bool EvalBase::isFormula(const QString& formula) {
  if (formula.isEmpty())
    return false;
  else if (formula[0] != '=')
    return false;
  else
    return true;
}

double EvalBase::valueOfCell(std::shared_ptr<astcalculator> ast, Table* table,
                             const QString& locationOfOtherCell, int currentRow,
                             int currentColumn) {
    #ifndef BUILDNOEXCELUI
  int otherCellRow, otherCellColumn;
#ifdef ASKDEBUG
  qDebug() << "valueOfCell arg" << locationOfOtherCell;
#endif
  Table::decodePosition(locationOfOtherCell, otherCellRow, otherCellColumn);
  QString otherCellFormula =
      table->getItemText(otherCellRow, otherCellColumn, Qt::EditRole);
  otherCellFormula.append(QChar(QChar::Null));

  // if the formula of cell "A1" is using the value of cell "A2" and
  // the formula of cell "A2" is using the value of cell "A1",
  // an infinite loop will occur... So:

  QString currentCellLocation;
  if (isFormula(otherCellFormula)) {
    Table::encodePosition(currentCellLocation, currentRow, currentColumn);
    int i =
        otherCellFormula.indexOf(currentCellLocation, 0, Qt::CaseInsensitive);
    if (i != -1 && !otherCellFormula[i - 1].isLetter() &&
        !otherCellFormula[i + currentCellLocation.length()].isDigit()) {
#ifdef ASKDEBUG
      qDebug() << "otherCellFormula" << otherCellFormula;
#endif
      throw_runtime_error(ast, "Err:522");
    }
  }

  bool ok = false;
  double tmp = table->getItemText(otherCellRow, otherCellColumn).toDouble(&ok);
  if (!ok) {
    auto text = table->getItemText(otherCellRow, otherCellColumn);
    if (!currentCellLocation.isEmpty() && !text.isEmpty() &&
        currentCellLocation == text) {
      throw_runtime_error(ast, "Err:522");
    }
    if (text == "Err:522") {
      throw_runtime_error(ast, "Err:522");
    }
    if (text == "#VALUE!") {
      throw_runtime_error(ast, "#VALUE!");
    }
    if (text.startsWith("inline")) {
      throw_runtime_error(ast, "#VALUE!");
    }
    if (text == "true") {
      return 1;
    }
    if (text == "false") {
      return 0;
    }
  }
#ifdef ASKDEBUG
  qDebug() << "tmp valueofarg result" << tmp;
#endif
  return tmp;
#endif
  return 0;
}

double EvalBase::toNumberFromVariant(const QVariant &var)
{
    #ifdef ASKDEBUG
    qDebug()<<
               "var"<<var;
#endif
    if(var.canConvert<double> ()){
        double temp=var.value<double>();
        return QString::number (temp,'G',11).toDouble ();
    }
    return 0;
}

QVariant EvalBase::evalExpressionCmp(const std::shared_ptr<astcalculator> ast,
                                      int row, int col) {
  /*
    EXPRESSIONCMP <- EXPRESSION (CMPOP EXPRESSION)*
    */
  const auto& nodes = ast->nodes;
#ifdef ASKDEBUG
  qDebug() << "evalExpressionCmp nodes size" << nodes.size() << "token"
           << ast->token.c_str();
#endif
  QVariant lhs;
  QVariant rhs;
  if (nodes.size() == 2) {
    for (decltype(nodes.size()) x = 0; x < nodes.size(); x++) {
#ifdef ASKDEBUG
      qDebug() << "tagname" << nodes.at(x)->name.c_str();
#endif
    }
  }
  if (nodes.size() == 4) {
    for (decltype(nodes.size()) x = 0; x < nodes.size(); x++) {
#ifdef ASKDEBUG
      qDebug() << "tagname" << nodes.at(x)->name.c_str();
#endif
    }
  }
  using namespace peg;
  bool lastok = false;
  switch (nodes.size()) {
    case 1:
      return eval_expression(nodes.at(0), row, col);
    case 2:
      return eval_expression(ast, row, col);
    default:
      if (nodes.size() > 259) {
        throw_runtime_error(
            ast, "error perbanding jika tidak boleh lebih dari 255x");
      }

      lhs = eval_expression(nodes.at(0), row, col);
#ifdef ASKDEBUG
      qDebug() << __FILE__ << __LINE__ << "called, nodesize" << nodes.size();
#endif
      for (decltype(nodes.size()) x = 1; x < nodes.size(); x += 2) {
        qDebug() << "eval_expresscmp x" << x << "node size" << nodes.size();
        qDebug() << __FILE__ << __LINE__ << "called, nodesize" << nodes.size()
                 << "x" << x << "name op" << nodes.at(x)->name.c_str()
                 << "token" << nodes.at(x)->token.c_str() << "rhs token"
                 << nodes.at(x + 1)->token.c_str();
        rhs = eval_expression(nodes.at(x + 1), row, col);
        // expectTwoSameVariantType(ast,Q_FUNC_INFO,lhs,rhs);
        if (rhs == 2) {
          qDebug() << "rhs ==2 !!";
        }
        if (std::strcmp(nodes.at(x)->token.c_str(), "<>") == 0) {
          lastok = lhs != rhs;
          lhs = lastok;
        } else if (std::strcmp(nodes.at(x)->token.c_str(), "<=") == 0) {
          lastok = lhs <= rhs;
          lhs = lastok;
        } else if (std::strcmp(nodes.at(x)->token.c_str(), ">=") == 0) {
          lastok = lhs >= rhs;
          lhs = lastok;
        } else if (std::strcmp(nodes.at(x)->token.c_str(), "!=") == 0) {
          lastok = lhs != rhs;
          lhs = lastok;
        } else if (std::strcmp(nodes.at(x)->token.c_str(), "<") == 0) {
          lastok = lhs < rhs;
          lhs = lastok;
        } else if (std::strcmp(nodes.at(x)->token.c_str(), ">") == 0) {
          lastok = lhs > rhs;
          lhs = lastok;
        } else if (std::strcmp(nodes.at(x)->token.c_str(), "=") == 0) {
          lastok = lhs == rhs;
          lhs = lastok;
        } else {
          throw_runtime_error(nodes.at(x), "unknown  cmp operator!!");
        }
      }
      break;
  }
#ifdef ASKDEBUG
  qDebug() << "akhir lhs exprcmp" << lhs;
#endif
  return lhs;
}

QVariant EvalBase::evalExpIfTail(const std::shared_ptr<astcalculator> ast,
                                  int row, int col) {
  /*
EXPIFTAIL <- JIKAIF _ EXPIFCMPAND  _ JIKATHEN _ EXPIFVALUE _ JIKAELSE _
EXPIFVALUE
            / JIKAIF _ EXPIFCMPOR _ JIKATHEN _ EXPIFVALUE _  JIKAELSE _
EXPIFVALUE
  */
  const auto& nodes = ast->nodes;
  expectNodeSize(ast, Q_FUNC_INFO, 6);
#ifdef ASKDEBUG
  qDebug() << "iftail node size" << nodes.size() << "tagname"
           << nodes.at(1)->name.c_str();
#endif
  QVariant lval;
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
  if (std::strcmp(nodes.at(1)->name.c_str(), "EXPIFCMPAND") == 0) {
    lval = eval_exprifcmpand(nodes.at(1), row, col);
  } else if (std::strcmp(nodes.at(1)->name.c_str(), "EXPIFCMPOR") == 0) {
    lval = eval_exprifcmpor(nodes.at(1), row, col);
  } else {
    lval =
        eval(nodes.at(1), CalculatorPEG::newInstance()->getTable(), row, col);
  }
  throwIfNotNumberVariant(lval, nodes.at(1),
                          "kondisi jika harus bernilai boolean atau angka");

  lval = evalBooleanFromVariant(lval);
  bool decision = lval.toBool();
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED decision" << decision
           << "at 3 tagname" << nodes.at(3)->name.c_str() << "at 5 tagname"
           << nodes.at(5)->name.c_str()<<"lval"<<lval;
#endif
  if (decision) {
    return eval (nodes.at(3), CalculatorPEG::newInstance()->getTable(),row,
                col);
  } else {
    return eval (nodes.at(5), CalculatorPEG::newInstance()->getTable(),row,
                col);
  }
}

QVariant EvalBase::eval_exprifcmpor(const std::shared_ptr<astcalculator> ast,
                                    int row, int col) {
  /*
                 EXPIFCMPOR	<- EXPRESSIONCMP ( _ OR _ EXPRESSIONCMP)*
   * */
  const auto& nodes = ast->nodes;
  auto lval =
      evalExpressionCmp(nodes.at(0),  row, col);

  throwIfNotNumberVariant(lval, nodes.at(0),
                          "kondisi jika harus bernilai boolean atau angka");

  bool decision = evalBooleanFromVariant(lval);
  if (nodes.size() == 1) {
    return decision;
  }
  for (decltype(nodes.size()) x = 1; x < nodes.size(); x += 2) {
    auto rval = evalExpressionCmp(nodes.at(x + 1), 
                     row, col);
    throwIfNotNumberVariant(rval, nodes.at(x + 1),
                            "kondisi jika harus bernilai boolean atau angka");
    decision = decision || evalBooleanFromVariant(rval);
  }
  return decision;
}

QVariant EvalBase::eval_exprifcmpand(const std::shared_ptr<astcalculator> ast,
                                     int row, int col) {
  /*
                 EXPIFCMPAND <- EXPRESSIONCMP ( _ AND _ EXPRESSIONCMP)*
   */

  const auto& nodes = ast->nodes;
  auto lval =
      evalExpressionCmp (nodes.at(0), row, col);

  throwIfNotNumberVariant(lval, nodes.at(0),
                          "kondisi jika harus bernilai boolean atau angka");

  bool decision = evalBooleanFromVariant(lval);
  if (nodes.size() == 1) {
    return decision;
  }
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"<<"EXPIFCMPAND lval"<<
              lval<<"decision"<<decision;
#endif
  for (decltype(nodes.size()) x = 1; x < nodes.size(); x += 2) {
    auto rval = evalExpressionCmp(nodes.at(x + 1), 
                     row, col);
    throwIfNotNumberVariant(rval, nodes.at(x + 1),
                            "kondisi jika harus bernilai boolean atau angka");
    rval=evalBooleanFromVariant(rval);
    decision = decision && rval.toBool ();
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"<<"EXPIFCMPAND rval"<<
              rval<<"decision"<<decision;
#endif
  }
#ifdef ASKDEBUG
  qDebug()<<"decision"<<decision;
#endif
  return decision;
}

QVariant EvalBase::eval_exprifvalue(const std::shared_ptr<astcalculator> ast,
                                    int row, int col) {
  /*
EXPIFVALUE <- EXPRESSIONCMP
          / EXPIFTAIL
   * */
  const auto& nodes = ast->nodes;
#ifdef ASKDEBUG
  qDebug() << "eval_exprifvalue nodesize" << nodes.size() << "tagname"
           << nodes.at(0)->name.c_str();
#endif
  if (std::strcmp(nodes.at(0)->name.c_str(), "EXPIFTAIL") == 0) {
    return evalExpIfTail (nodes.at(0), row, col);
  }
  return eval (nodes.at(0), CalculatorPEG::newInstance()->getTable(), row, col);
}

QVariant EvalBase::eval_expression(const std::shared_ptr<astcalculator> ast,
                                   int row, int col) {
  /*
  EXPRESSION <- SIGN TERM (TERM_OP TERM)*
  */
  const auto& nodes = ast->nodes;
  QVariant val =
      eval(nodes.at(1), CalculatorPEG::newInstance()->getTable(), row, col);
#ifdef ASKDEBUG
  qDebug() << "in eval_expression val" << val << "nodes size" << nodes.size();
#endif
  bool valid = false;
  if (nodes.size() == 2) {
#ifdef ASKDEBUG
    qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
    auto sign = nodes.at(0)->token;
    auto sign_val = (sign.empty() || sign == "+") ? 1 : -1;
#ifdef ASKDEBUG
    qDebug() << "sign_val" << sign_val << "sign" << sign.c_str();
#endif
    switch (val.type()) {
    case QVariant::ULongLong:
    case QVariant::Int:
    case QVariant::UInt:
      case QVariant::LongLong:
        if (!sign.empty()) val = val.toLongLong() * sign_val;
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                 << "val" << val;
#endif
        return val;
      case QVariant::Double:
        if (!sign.empty()) val =val.toDouble ()* sign_val;
        return val;
      case QVariant::String:
      case QVariant::ByteArray:
        if (!sign.empty())
          throw_runtime_error(
              ast, "error illegal operasi aritmatika dengan tipe data string.");
      case QVariant::Bool:
        if (!sign.empty()) val = val.toInt() * sign_val;
        return val;
      case QVariant::Date:
        if (!sign.empty())
          throw_runtime_error(
              ast, "error illegal operasi aritmatika dengan tipe data date.");
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        return val;
      case QVariant::Time:
        if (!sign.empty())
          throw_runtime_error(
              ast, "error illegal operasi aritmatika dengan tipe data time.");
        return val;
      default:
        if (!sign.empty()) {
          throw_runtime_error(ast, "error illegal operasi aritmatika.");
        } else {
          return val;
        }
    }
  }
#ifdef ASKDEBUG
  qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file"
           << __FILE__ << "lval" << val;
#endif
  throwIfNotNumberVariant(val, nodes.at(1),
                          "aritmatika harus bernilai angka atau boolean");
  switch (val.type()) {
    case QVariant::Bool:
      val = val.toBool() ? 1 : 0;
    default: {
      auto sign = nodes.at(0)->token;
      auto sign_val = (sign.empty() || sign == "+") ? 1 : -1;
      bool computed = false;
#ifdef ASKDEBUG
      qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file"
               << __FILE__ << "val" << val;
#endif
      if (val.type() == QVariant::LongLong || val.type() == QVariant::Int) {
        valid = true;
        computed = true;
        val = val.toLongLong() * sign_val;
#ifdef ASKDEBUG
        qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__
                 << " file" << __FILE__ << "lval" << val << "tolonglong"
                 << val.toLongLong() << "sign_val" << sign_val;
#endif
        for (auto i = 2u; i < nodes.size(); i += 2) {
#ifdef ASKDEBUG
          qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__
                   << " file" << __FILE__;
          qDebug() << "index i " << i << "nodes ope "
                   << nodes.at(i)->token.c_str() << "adv"
                   << nodes.at(i + 1)->token.c_str();
          qDebug() << "after";
#endif
          auto ope = nodes.at(i + 0)->token[0];
#ifdef ASKDEBUG
          qDebug() << "after ope " << ope;
#endif
          auto rval = eval(nodes.at(i + 1),
                           CalculatorPEG::newInstance()->getTable(), row, col);
#ifdef ASKDEBUG
          qDebug() << "in eval_expression rval" << rval << "nodes.size ()"
                   << nodes.size() << "sign_val" << sign_val << "sign"
                   << sign.c_str() << "val" << val;
#endif
          if (rval.type() == QVariant::LongLong) {
            std::pair<bool, qint64> ret;
            switch (ope) {
              case '+':
                // avoid overhead multi arithmetic operation
                ret = TestOverflow::checkPlusOverflow(val.toLongLong(),
                                                      rval.toLongLong());
                if (ret.first) {
                  throw_runtime_error(nodes.at(i + 1),
                                      "error overflow penjumlahan");
                }
#ifdef ASKDEBUG
                qDebug() << "retval" << ret.second;
#endif
                val.setValue(ret.second);
                break;
              case '-':
                ret = TestOverflow::checkMinOverflow(val.toLongLong(),
                                                     rval.toLongLong());
                if (ret.first) {
                  throw_runtime_error(ast, "error overflow pengurangan");
                }
                val.setValue(ret.second);
                break;
            }
          }
          if (rval.type() == QVariant::Double) {
            std::pair<bool, qint64> ret;
            std::feclearexcept(FE_ALL_EXCEPT);
            auto rd =rval.toDouble ();
            auto ld = val.toLongLong();
            double retd = 0.0;
            switch (ope) {
              case '+':
                // avoid overhead multi arithmetic operation
                retd = rd + ld;

                if (isoverflowDoubleValueHappen(ast)) {
                  //      throw_runtime_error(ast, "error overflow
                  //      penjumlahan");
                }
                if (!TestOverflow::validPresisiNumber(retd)) {
                  throw_runtime_error(ast,
                                      "error invalid double hasil penjumlahan");
                }
                //	qDebug() << " rd " << rd << " ld " << ld << " node size
                //" << nodes.size() <<" retd"<<retd	;
                val.setValue(retd);
                break;
              case '-':
                retd = ld - rd;
                if (isoverflowDoubleValueHappen(ast)) {
                  // throw_runtime_error(ast, "error underflow pengurangan");
                }
                if (!TestOverflow::validPresisiNumber(retd)) {
                  throw_runtime_error(ast,
                                      "error invalid double hasil pengurangan");
                }
                val.setValue(retd);
                break;
            }
          }
        }

        return val;
      }
      // gak mungking compute lagi sebelumnya
      if (val.type() == QVariant::Double && !computed) {
        valid = true;
        computed = true;
        val = val.toDouble ()* sign_val;
#ifdef ASKDEBUG
        qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__
                 << " file" << __FILE__ << "val" << val;
#endif
        for (auto i = 2u; i < nodes.size(); i += 2) {
          auto ope = nodes.at(i + 0)->token[0];
          auto rval = eval(nodes.at(i + 1),
                           CalculatorPEG::newInstance()->getTable(), row, col);
#ifdef ASKDEBUG
          qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__
                   << " file" << __FILE__ << rval;
#endif
          if (rval.type() == QVariant::LongLong) {
            std::feclearexcept(FE_ALL_EXCEPT);
            auto ld = val.toDouble ();
            auto rd = rval.toLongLong();
            double retd = 0.0;
            switch (ope) {
              case '+':
                retd = ld + rd;
                if (isoverflowDoubleValueHappen(ast)) {
                  // throw_runtime_error(ast, "error penjumlahan double
                  // overflow");
                }
                if (!TestOverflow::validPresisiNumber(retd)) {
                  throw_runtime_error(ast,
                                      "error invalid double hasil penjumlahan");
                }
                val.setValue(retd);
                // qDebug() << " retd " << retd << " val " << val;
                break;
              case '-':
                retd = ld - rd;
                if (isoverflowDoubleValueHappen(ast)) {
                  throw_runtime_error(ast,
                                      "error penjumlahan double underflow");
                }
                if (!TestOverflow::validPresisiNumber(retd)) {
                  throw_runtime_error(ast,
                                      "error invalid double hasil pengurangan");
                }

                val.setValue(retd);
                break;
            }
          }
          if (rval.type() == QVariant::Double) {
            std::feclearexcept(FE_ALL_EXCEPT);
            auto rd =rval.toDouble ();
            auto ld =val.toDouble ();
            double retd = 0.0;
            switch (ope) {
              case '+':
                // qDebug() << " rd " << rd << " ld " << ld << " node size "
                // << nodes.size();
                retd = ld + rd;
                if (isoverflowDoubleValueHappen(ast)) {
                  // throw_runtime_error(ast, "error penjumlahan double
                  // overflow");
                }
                if (!TestOverflow::validPresisiNumber(retd)) {
                  throw_runtime_error(ast,
                                      "error invalid double hasil penjumlahan");
                }

                val.setValue(retd);
                break;
              case '-':
                retd = ld - rd;
                if (isoverflowDoubleValueHappen(ast)) {
                  throw_runtime_error(ast,
                                      "error penjumlahan double underflow");
                }
                if (!TestOverflow::validPresisiNumber(retd)) {
                  throw_runtime_error(ast,
                                      "error invalid double hasil pengurangan");
                }

                val.setValue(retd);
                break;
            }
          }
        }
#ifdef ASKDEBUG
        qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__
                 << " file" << __FILE__ << val;
#endif
        return val;
      }
      if (!valid) {
        throw_runtime_error(ast, "error unhandled valid expression");
      }
    }
  }
  throw_runtime_error(ast, "error unhandled expression type");
}

QVariant EvalBase::eval_term(const std::shared_ptr<astcalculator> ast, int row,
                             int col) {
  // term <- factor (factor_op factor)*
  const auto& nodes = ast->nodes;
  if (nodes.size() == 0) {
    throw_runtime_error(ast, "error nodes size di eval_term kosong");
  }
#ifdef ASKDEBUG
  qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file"
           << __FILE__;
  qDebug() << "nodesize eval_term" << nodes.size();
#endif
  auto val = eval_factor(nodes.at(0), row, col);
#ifdef ASKDEBUG
  qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file"
           << __FILE__ << val;
  qDebug() << "eval term called";
#endif
  bool valid = false;

  if (nodes.size() == 1) {
    return val;
  }
  if (val.type() == QVariant::Bool) {
    val = val.toBool() ? 1 : 0;
  }
  bool computed = false;
#ifdef ASKDEBUG
  qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file"
           << __FILE__ << val;
#endif
  if (val.type() == QVariant::LongLong || val.type() == QVariant::Int) {
    valid = true;
    computed = true;
    for (auto i = 1u; i < nodes.size(); i += 2) {
      auto ope = nodes.at(i)->token[0];
      auto rval = eval_factor(nodes.at(i + 1), row, col);
      if (rval.type() == QVariant::Bool) {
        rval = rval.toBool() ? 1 : 0;
      }
#ifdef ASKDEBUG
      qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file"
               << __FILE__ << rval<<"wid lval"<<val;
#endif
      std::pair<bool, double> retd;
      std::pair<bool,qint64> ret;
      if (rval.type() == QVariant::LongLong || rval.type() == QVariant::Int ||
          rval.type() == QVariant::ULongLong || rval.type() == QVariant::UInt) {
        switch (ope) {
          case '*':
            ret = TestOverflow::checkMulOverflow(val.toLongLong(),
                                                 rval.toLongLong());
            if (ret.first) {
              throw_runtime_error(nodes.at(i), "error overflow perkalian");
            }
            val.setValue(ret.second);
            break;
          case '/':
            std::feclearexcept(FE_ALL_EXCEPT);
            if (rval == 0 || rval == -0
                    ||rval==0.0 ||rval==-0.0) {
              throw_runtime_error(nodes.at(i), "error divide by 0 error");
            }
            retd = TestOverflow::checkDivOverflow(val.toLongLong(),
                                                 rval.toLongLong());
            if (ret.first) {
              throw_runtime_error(nodes.at(i), "error overflow pembagian");
            }
            if (isoverflowDoubleValueHappen(nodes.at(i))) {
            }
            if (!TestOverflow::validPresisiNumber(retd.second)) {
              throw_runtime_error(nodes.at(i+1),
                                  "error invalid double hasil "
                                  "pembagian");
            }
#ifdef ASKDEBUG
            qDebug()<<"retd "<<retd.second;
#endif
            val.setValue(retd.second);
            break;
        default:
#ifdef ASKDEBUG
            qDebug()<<"unhandled ope "<<__FILE__<<__LINE__;
#endif
            break;
        }
      }
      if (rval.type() == QVariant::Double) {
        std::feclearexcept(FE_ALL_EXCEPT);
        auto rd =rval.toDouble ();
        auto ld = val.toLongLong();
        double retd = 0.0;
        switch (ope) {
          case '*':
            retd =(double) ld * rd;
            if (isoverflowDoubleValueHappen(nodes.at(i))) {
            }
            if (!TestOverflow::validPresisiNumber(retd)) {
              throw_runtime_error(nodes.at(i),
                                  "error invalid double hasil "
                                  "perkalian");
            }
            val.setValue(retd);
            break;
          case '/':
#ifdef ASKDEBUG
            qDebug()<<"pembagian"<<ld<<"rd"<<rd;
            #endif
            if (rval == 0 || rval == -0
                    ||rval==0.0 ||rval==-0.0) {
              throw_runtime_error(nodes.at(i), "error divide by 0 error");
            }
            retd =(double) ld / rd;
            if (isoverflowDoubleValueHappen(nodes.at(i))) {
              // throw_runtime_error(ast, "error overflow pembagian");
            }
            if (!TestOverflow::validPresisiNumber(retd)) {
              throw_runtime_error(nodes.at(i),
                                  "error invalid double hasil pembagian");
            }
            val.setValue(retd);
            break;
        }
      }
    }
    return val;
  }
#ifdef ASKDEBUG
  qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file"
           << __FILE__;
#endif
  if (!computed) {
    valid = true;
    computed = true;
    for (auto i = 1u; i < nodes.size(); i += 2) {
      auto ope = nodes.at(i)->token[0];
#ifdef ASKDEBUG
      qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file"
               << __FILE__;
#endif
      auto rval = eval_factor(nodes.at(i + 1), row, col);
#ifdef ASKDEBUG
      qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file"
               << __FILE__;
#endif
      throwIfNotNumberVariant(rval, nodes.at(i + 1),
                              "operasi aritmatika harus bernilai angka");
      if (rval.type() == QVariant::Bool) {
        rval = rval.toBool() ? 1 : 0;
      }
      if (rval.type() == QVariant::Double) {
        std::feclearexcept(FE_ALL_EXCEPT);
        auto rd =rval.toDouble ();
        auto ld =val.toDouble ();
        double retd = 0.0;
        switch (ope) {
          case '*':
            retd = (double)rd * ld;
            if (isoverflowDoubleValueHappen(nodes.at(i + 1))) {
              // throw_runtime_error(ast, "error overlow double perkalian");
            }
            if (!TestOverflow::validPresisiNumber(retd)) {
              throw_runtime_error(nodes.at(i),
                                  "error invalid double hasil perkalian");
            }

            val.setValue(retd);
            break;
          case '/':
            if (rval == 0 || rval == -0
                    ||rval==0.0 ||rval==-0.0) {
              throw_runtime_error(nodes.at(i), "error divide by 0 error");
            }
            retd =(double) ld / rd;
            if (isoverflowDoubleValueHappen(nodes.at(i))) {
              // throw_runtime_error(ast, "error overlow double perkalian");
            }
            if (!TestOverflow::validPresisiNumber(retd)) {
              throw_runtime_error(nodes.at(i),
                                  "error invalid double hasil pembagian");
            }

            val.setValue(retd);
            break;
        }
      }
      if (rval.type() == QVariant::LongLong || rval.type() == QVariant::Int) {
        std::feclearexcept(FE_ALL_EXCEPT);
        auto rd = rval.toLongLong();
        auto ld =val.toDouble ();
        double retd = 0.0;
        switch (ope) {
          case '*':
            retd =(double) rd * ld;
            if (isoverflowDoubleValueHappen(nodes.at(i))) {
              // throw_runtime_error(ast, "error overlow double perkalian");
            }
            if (!TestOverflow::validPresisiNumber(retd)) {
              throw_runtime_error(nodes.at(i),
                                  "error invalid double hasil perkalian");
            }
            val.setValue(retd);
            break;
          case '/':
            if (rval == 0 || rval == -0
                    ||rval==0.0 ||rval==-0.0) {
              throw_runtime_error(nodes.at(i), "error divide by 0 error");
            }
#ifdef ASKDEBUG
            qDebug()<<"pembagian"<<ld<<"rd"<<rd;
            #endif
            retd =(double) ld / rd;
            if (isoverflowDoubleValueHappen(nodes.at(i))) {
              //  throw_runtime_error(ast, "error underlow double perkalian");
            }
            if (!TestOverflow::validPresisiNumber(retd)) {
              throw_runtime_error(nodes.at(i),
                                  "error invalid double hasil perkalian");
            }
            val.setValue(retd);
            break;
        }
      }
    }
    return val;
  }
  if (val.type() == QVariant::String) {
    valid = true;
    // handle jika node lebih dari 1
    if (nodes.size() > 1) {
      throw_runtime_error(
          nodes.at(0), "operasi aritmatika pada nilai string tidak diizinkan");
    }
    return val.toString();
  }
  if (val.type() == QVariant::Date) {
    valid = true;
    if (nodes.size() > 1) {
      throw_runtime_error(nodes.at(0),
                          "operasi aritmatika pada nilai date tidak diizinkan");
    }
    return val.toDate();
  }
  if (!valid) {
    throw_runtime_error(ast, "error unhandled valid [*/] operation");
  }
  return val;
}

QVariant EvalBase::eval_factor(const std::shared_ptr<astcalculator> ast,
                               int row, int col) {
  /*
FACTOR  <- OPEN _ EXPRESSIONCMP _ CLOSE _
                      / BENAR
                      / SALAH
                      / STRINGVALUE
                      / NUMBERDOUBLE
                      / NUMBER
                      / FUNCTION
                      / BARISKOLOMEXCEL
   * */
  const auto nodes = ast->nodes;
  isexistFieldExcel(ast, Q_FUNC_INFO);
#ifdef ASKDEBUG
  qDebug() << "nodes size eval_factor " << nodes.size() << "tag name "
           << ast->name.c_str();
#endif
  if (std::strcmp(ast->name.c_str(), "BENAR") == 0) {
    return true;
  }
  if (std::strcmp(ast->name.c_str(), "SALAH") == 0) {
    return false;
  }
  if (std::strcmp(ast->name.c_str(), "STRINGVALUE") == 0) {
    return eval_stringvalue(ast);
  }
  if (std::strcmp(ast->name.c_str(), "NUMBERDOUBLE") == 0) {
    return eval_numberdouble(ast);
  }
  if (std::strcmp(ast->name.c_str(), "FRACTIONALCONSTANT") == 0) {
    return eval_fractionalconstant(ast);
  }
  if (std::strcmp(ast->name.c_str(), "NUMBER") == 0) {
    return eval_number(ast);
  }
  if (std::strcmp(ast->name.c_str(), "FUNCTION") == 0) {
    return eval_function(ast, row, col);
  }
  if (std::strcmp(ast->name.c_str(), "BARISKOLOMEXCEL") == 0) {
    return valueOfCell(ast, CalculatorPEG::newInstance()->getTable(),
                       QString("%1%2")
                           .arg(nodes.at(0)->token.c_str())
                           .arg(nodes.at(1)->token.c_str()),
                       row, col);
  }
  bool iscontainexpr = false;
  int idx_expr = -1;
  for (decltype(nodes.size()) x = 0; x < nodes.size(); x++) {
    if (std::strcmp(nodes.at(x)->name.c_str(), "EXPRESSIONCMP") == 0) {
      iscontainexpr = true;
      idx_expr = x;
    }
    if (std::strcmp(nodes.at(x)->name.c_str(), "EXPRESSION") == 0) {
      return eval_expression(nodes.at(x), row, col);
    }
#ifdef ASKDEBUG
    qDebug() << " eval_factor tag name " << nodes.at(x)->name.c_str();
#endif
  }
  if (iscontainexpr) {
    return eval(nodes.at(idx_expr), CalculatorPEG::newInstance()->getTable(),
                row, col);
  }

  throw_runtime_error(ast, "unhandled factor");
}

QVariant EvalBase::eval_stringvalue(const std::shared_ptr<astcalculator> ast) {
  /*
   STRINGVALUE <- PETIK STRING PETIK
    */
  if (ast->nodes.size() != 3) {
    return QVariant("");
  }
  return eval_string(ast->nodes.at(1));
}

QVariant EvalBase::eval_string(const std::shared_ptr<astcalculator> ast) {
  QString str(ast->token.c_str());
  if (str.compare("benar", Qt::CaseInsensitive) == 0 ||
      str.compare("true", Qt::CaseInsensitive) == 0) {
    return true;
  }
  if (str.compare("salah", Qt::CaseInsensitive) == 0 ||
      str.compare("false", Qt::CaseInsensitive) == 0) {
    return false;
  }
  auto isdate = eval_stringdate(str);
  if (isdate.first) {
    return QDate::fromString(str, isdate.second);
  }
  auto istime = eval_stringtime(str);
  if (istime.first) {
    return QTime::fromString(str, istime.second);
  }
  return str;
}

QVariant EvalBase::eval_number(const std::shared_ptr<astcalculator> ast) {
  // qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << " file
  // " << __FILE__ ;
  auto cek = TestOverflow::checkInRangeNumberFromStr(ast->token);
  if (!cek.first && cek.second == -1) {
    throw_runtime_error(ast, "error underflow angka 64bit integer");
  }
  if (!cek.first && cek.second == 1) {
    throw_runtime_error(ast, "error overflow angka 64bit integer");
  }
  return cek.second;
}

QVariant EvalBase::eval_numberdouble(const std::shared_ptr<astcalculator> ast) {
  // NUMBERDOUBLE <- FRACTIONALCONSTANT EXPONENTPART?
  const auto& nodes = ast->nodes;
  auto lhs = eval_fractionalconstant(nodes.at(0));
  std::string double_val;
  double_val.append(lhs.toByteArray().data());
  if (nodes.size() == 2) {
    auto rhs = eval_exponentpart(nodes.at(1));
    double_val.append(rhs.toByteArray().data());
  }
  double ret = 0.0;
  try {
    ret = std::stod(double_val);
  } catch (std::invalid_argument&) {
    throw_runtime_error(ast, "error gagal konversi string ke angka double");
  } catch (std::out_of_range&) {
    throw_runtime_error(ast, "error gagal konversi string ke angka double");
  }
  if (!TestOverflow::validPresisiNumber(ret)) {
    throw_runtime_error(ast,
                        "error gagal konversi string ke angka, invalid "
                        "double (isFinite, isNAN)?");
  }
  return ret;
}

QVariant EvalBase::eval_fractionalconstant(
    const std::shared_ptr<astcalculator> ast) {
  /*
FRACTIONALCONSTANT <- NUMBER DOT NUMBER
  */
  const auto& nodes = ast->nodes;
  expectNodeSize(ast, Q_FUNC_INFO, 3);
  QByteArray fractionalconstant;
  fractionalconstant.append(nodes.at(0)->token.c_str());
  fractionalconstant.append(".");
  fractionalconstant.append(nodes.at(2)->token.c_str());
#ifdef ASKDEBUG
  qDebug() << " fractionalcontat" << fractionalconstant;
#endif
  return fractionalconstant.toDouble ();
}

QVariant EvalBase::eval_exponentpart(const std::shared_ptr<astcalculator> ast) {
  /*
  EXPONENTPART <- SIGNEXPONENT SIGN? NUMBER
  */
  const auto& nodes = ast->nodes;
  QByteArray exponentpart;
  exponentpart.append(nodes.at(0)->token.c_str());
  if (nodes.size() == 3) {
    auto chs = nodes.at(1)->token;
    if (!chs.empty()) {
      exponentpart.append(chs.c_str());
    }
  }
  auto rhs = eval_number(nodes.at(2));
  exponentpart.append(rhs.toByteArray().data());
  return exponentpart.data();
}

std::pair<bool, std::shared_ptr<astcalculator> > EvalBase::isexistFieldExcel(
    const std::shared_ptr<astcalculator> ast, const char* func) {
  int counter_bariskolomexcel = 0;
  bool found_single_seperatorfield_excel = false;

  for (decltype(ast->nodes.size()) x = 0; x < ast->nodes.size(); x++) {
    for (decltype(ast->nodes.size()) col = 0;
         col < ast->nodes.at(x)->nodes.size(); col++) {
#ifdef ASKDEBUG
      qDebug() << func << "tagname"
               << ast->nodes.at(x)->nodes.at(col)->name.c_str();
#endif
      /*
       BARISKOLOMEXCEL
       * */
      if (std::strcmp(ast->nodes.at(x)->nodes.at(col)->name.c_str(),
                      "BARISKOLOMEXCEL") == 0) {
        counter_bariskolomexcel++;
      }
      if (std::strcmp(ast->nodes.at(x)->nodes.at(col)->name.c_str(),
                      "SEPERATORFIELDEXCEL") == 0) {
        found_single_seperatorfield_excel = true;
      }
      if (counter_bariskolomexcel == 2 && found_single_seperatorfield_excel) {
        return std::make_pair(true, ast->nodes.at(x));
      }
      // KOLOMEXCEL
      if (std::strcmp(ast->nodes.at(x)->nodes.at(col)->name.c_str(),
                      "KOLOMEXCEL") == 0) {
        return std::make_pair(true, ast->nodes.at(x));
      }
      if (std::strcmp(ast->nodes.at(x)->nodes.at(col)->name.c_str(),
                      "FIELDEXCEL") == 0) {
#ifdef ASKDEBUG
        qDebug() << "found FIELDEXCEL!!!!!!";
#endif
        return std::make_pair(true, ast->nodes.at(x)->nodes.at(col));
      }
    }
  }

  return std::make_pair(false, nullptr);
}

QVariant EvalBase::eval_power(const std::shared_ptr<astcalculator> ast, int row,
                              int col) {
  /*
   POWER OPEN EXPRESSIONCMP DOTCOMMA EXPRESSIONCMP CLOSE

FIELDEXCEL <- BARISKOLOMEXCEL SEPERATORFIELDEXCEL BARISKOLOMEXCEL
                        / BARISKOLOMEXCEL

BARISKOLOMEXCEL <- KOLOMEXCEL NUMBER
KOLOMEXCEL <- [a-zA-Z]+
SEPERATORFIELDEXCEL <- ':'
   * */
  const auto& nodes = ast->nodes;
  expectNodeSize(ast, Q_FUNC_INFO, 6);
#ifdef ASKDEBUG
  qDebug() << "eval_power called" << nodes.size() << "ast tagname"
           << ast->name.c_str() << "ast 4 tagname" << nodes.at(4)->name.c_str()
           << " token 4" << nodes.at(4)->token.c_str() << " token"
           << ast->name.c_str() << " tag 0 " << nodes.at(0)->name.c_str()
           << "ast 2 tagname" << nodes.at(2)->name.c_str();
#endif

  auto lhs =
      eval(nodes.at(2), CalculatorPEG::newInstance()->getTable(), row, col);
  auto rhs =
      eval(nodes.at(4), CalculatorPEG::newInstance()->getTable(), row, col);
  throwIfNotNumberVariant(
      lhs, nodes.at(2), "error nilai input perpangkatan harus bernilai angka");
  throwIfNotNumberVariant(
      rhs, nodes.at(4), "error nilai input perpangkatan harus bernilai angka");

  std::feclearexcept(FE_ALL_EXCEPT);

  double retd = std::pow(lhs.toDouble (),rhs.toDouble ());
  if (isoverflowDoubleValueHappen(ast)) {
    // throw_runtime_error(ast, "error perpangkatan double overflow");
  }
  if (!TestOverflow::validPresisiNumber(retd)) {
    throw_runtime_error(ast, "error invalid double value ");
  }
#ifdef ASKDEBUG
  qDebug() << "retd" << retd;
#endif
  return QVariant::fromValue(retd);
}
QVariant EvalBase::eval_funcsum(const std::shared_ptr<astcalculator> ast,
                                int row, int col) {
  /*
   SUM OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE

FIELDEXCEL <- BARISKOLOMEXCEL SEPERATORFIELDEXCEL BARISKOLOMEXCEL
                        / BARISKOLOMEXCEL

BARISKOLOMEXCEL <- KOLOMEXCEL NUMBER
KOLOMEXCEL <- [a-zA-Z]+
SEPERATORFIELDEXCEL <- ':'
   */

  const auto& nodes = ast->nodes;
#ifdef ASKDEBUG
  qDebug() << "eval_funcsum called" << ast->nodes.size() << "ast tagname"
           << ast->name.c_str() << "ast 2 tagname" << nodes.at(2)->name.c_str();
#endif
  QVariant lval;

  std::pair<bool, std::shared_ptr<astcalculator> > excel_var =
      isexistFieldExcel(nodes.at(2), Q_FUNC_INFO);
  if (excel_var.first) {
    lval = 0;
#ifdef ASKDEBUG
    qDebug() << "found tagname 2 funcsum" << excel_var.second->name.c_str()
             << "node size" << excel_var.second->nodes.size() << "";
#endif
    QList<double> tmp_lval;
    switch (excel_var.second->nodes.size()) {
      case 1:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        lval = valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str()),
            row, col);
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        break;
      case 2:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                 << "tagname size" << excel_var.second->nodes.size()
                 << "child size" << excel_var.second->nodes.at(0)->nodes.size();
        qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                 << excel_var.second->nodes.at(1)->token.c_str();
#endif
        lval =
            valueOfCell(excel_var.second->nodes.at(0),
                        CalculatorPEG::newInstance()->getTable(),
                        QString("%1%2")
                            .arg(excel_var.second->nodes.at(0)->token.c_str())
                            .arg(excel_var.second->nodes.at(1)->token.c_str()),
                        row, col);
        break;
      case 3:
        tmp_lval = extractDataRangeFromCell(
            excel_var.second->nodes.at(0),
            QString("%1%2:%3%4")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(1)->token.c_str()),
            CalculatorPEG::newInstance()->getTable(), row, col);
        for (int x = 0; x < tmp_lval.size(); x++) {
          lval = lval.toDouble ()+ tmp_lval.at(x);
        }
        break;
      default:
        throw_runtime_error(excel_var.second,
                            "unhandled FIELDEXCEL parsing func_sum");
        break;
    }
  } else {
#ifdef ASKDEBUG
    qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
    lval =
        eval(nodes.at(2), CalculatorPEG::newInstance()->getTable(), row, col);
  }
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
  throwIfNotNumberVariant(lval, nodes.at(2), "input fungsi sum harus angka");
  QVariant rrval = 0;

  switch (nodes.size()) {
    case 3: {
      return lval;
    }
    default:
      // lebih dari 3
      {
        if (nodes.size() > 259) {
          throw_runtime_error(
              ast, "error fungsi sum batasnya hanya 255 input angka");
        }

        for (size_t i = 2; i < nodes.size(); i += 2) {
#ifdef ASKDEBUG
          qDebug() << "i+1" << i + 1 << "nodes size()" << nodes.size();
#endif
          // sisakan 1 byte terminated string.
          auto mx = i + 2;
          if (mx == nodes.size()) {
            break;
          }
          std::pair<bool, std::shared_ptr<astcalculator> > excel_var2 =
              isexistFieldExcel(nodes.at(i + 2), Q_FUNC_INFO);
          if (excel_var2.first) {
#ifdef ASKDEBUG
            qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                     << "tagname size" << excel_var2.second->nodes.size()
                     << "child size"
                     << excel_var2.second->nodes.at(0)->nodes.size();
            qDebug() << "excel_var2.second->nodes.at (1)->token.c_str()"
                     << excel_var2.second->nodes.at(1)->token.c_str();
#endif
            QList<double> tmp_rval;
            switch (excel_var2.second->nodes.size()) {
              case 1:
#ifdef ASKDEBUG
                qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
                rrval = valueOfCell(excel_var2.second->nodes.at(0),
                                    CalculatorPEG::newInstance()->getTable(),
                                    QString("%1%2")
                                        .arg(excel_var2.second->nodes.at(0)
                                                 ->nodes.at(0)
                                                 ->token.c_str())
                                        .arg(excel_var2.second->nodes.at(0)
                                                 ->nodes.at(1)
                                                 ->token.c_str()),
                                    row, col);
#ifdef ASKDEBUG
                qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
                break;
              case 2:
                rrval = valueOfCell(
                    excel_var2.second->nodes.at(0),
                    CalculatorPEG::newInstance()->getTable(),
                    QString("%1%2")
                        .arg(excel_var2.second->nodes.at(0)->token.c_str())
                        .arg(excel_var2.second->nodes.at(1)->token.c_str()),
                    row, col);
                break;
              case 3:
                tmp_rval = extractDataRangeFromCell(
                    excel_var2.second->nodes.at(0),
                    QString("%1%2:%3%4")
                        .arg(excel_var2.second->nodes.at(0)
                                 ->nodes.at(0)
                                 ->token.c_str())
                        .arg(excel_var2.second->nodes.at(0)
                                 ->nodes.at(1)
                                 ->token.c_str())
                        .arg(excel_var2.second->nodes.at(2)
                                 ->nodes.at(0)
                                 ->token.c_str())
                        .arg(excel_var2.second->nodes.at(2)
                                 ->nodes.at(1)
                                 ->token.c_str()),
                    CalculatorPEG::newInstance()->getTable(), row, col);
                for (int x = 0; x < tmp_rval.size(); x++) {
                  rrval = rrval.toDouble ()+ tmp_rval.at(x);
                }
                break;
              default:
                throw_runtime_error(
                    excel_var2.second,
                    "unhandled FIELDEXCEL parsing func_sum in loop");
                break;
            }
          } else {
            rrval = eval(nodes.at(i + 2),
                         CalculatorPEG::newInstance()->getTable(), row, col);
          }
          throwIfNotNumberVariant(rrval, nodes.at(i + 2),
                                  "input fungsi sum harus angka");
#ifdef ASKDEBUG
          qDebug() << "lval" << lval << "rval" << rrval;
#endif
          if (rrval.type() == QVariant::Double ||
              lval.type() == QVariant::Double) {
            std::feclearexcept(FE_ALL_EXCEPT);
            double ret =lval.toDouble ()+ rrval.toDouble ();
            if (isoverflowDoubleValueHappen(ast)) {
              //   throw_runtime_error(ast, "error overflow double
              //   penjumlahan");
            }
            if (!TestOverflow::validPresisiNumber(ret)) {
              throw_runtime_error(
                  ast,
                  "error invalid double penjumlahan (infinite atau NaN) ?");
            }
            lval = ret;
          } else {
            const auto ret = TestOverflow::checkPlusOverflow(
                lval.toLongLong(), rrval.toLongLong());
            if (ret.first) {
              throw_runtime_error(nodes.at(0), "error overflow penjumlahan");
            }
            lval = ret.second;
          }
        }
        break;
      }
  }
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
           << "result  sum" << lval;
#endif
  return lval;
}

QList<double> EvalBase::extractDataRangeFromCell(
    const std::shared_ptr<astcalculator> ast, const QString args, Table* table,
    int row, int col) {
  QList<double> returnList;
  #ifndef BUILDNOEXCELUI
  int iterator = 0;
  QString currentCellLocation;
  Table::encodePosition(currentCellLocation, row, col);
#ifdef ASKDEBUG
  qDebug() << "currentCellLocation" << currentCellLocation
           << "CalculatorPEG::newInstance ()->getRow ()" << row
           << "CalculatorPEG::newInstance ()->getColumn ()" << col;
#endif
  if (isRange(args + QChar(QChar::Null), iterator)) {
    QTableWidgetSelectionRange range;
    Table::decodeRange(args, range);
    QString tmpcell;
    for (int row = range.topRow(); row <= range.bottomRow(); row++)
      for (int column = range.leftColumn(); column <= range.rightColumn();
           column++) {
        // avoid nested
        Table::encodePosition(tmpcell, row, column);
#ifdef ASKDEBUG
        qDebug() << "tmpcelll" << tmpcell;
        qDebug() << "row" << row << "col" << column << "current parent row"
                 << row << "parent col" << column << "split" << args.split(":");
#endif
        if (currentCellLocation == tmpcell) {
          throw_runtime_error(ast, "Err:522");
        }

        auto text = table->getItemText(row, column);
        if (text == "Err:522") {
          throw_runtime_error(ast, "Err:522");
        }
        if (text == "#VALUE!") {
          throw_runtime_error(ast, "#VALUE!");
        }
        if (text.startsWith("inline")) {
          throw_runtime_error(ast, "#VALUE!");
        }
        if (text == "true") {
          returnList.append(1);
        } else if (text == "false") {
          returnList.append(0);
        } else {
          returnList.append(text.toDouble());
        }
      }
  }
#endif
  return returnList;
}

bool EvalBase::isRange(const QString& formula, int& iterator) {
  // A range starts with one variable, then ':', then another variable (e.g.
  // "A1:B1"
  int i = iterator;
  int lastI = iterator;

  while (formula[i].isLetter()) i++;
  if (i == lastI) return false;
  lastI = i;
  while (formula[i].isDigit()) i++;
  if (i == lastI) return false;
  ignoreSpaces(formula, i);
  if (formula[i++] != ':') return false;
  ignoreSpaces(formula, i);

  lastI = i;
  while (formula[i].isLetter()) i++;
  if (i == lastI) return false;
  lastI = i;
  while (formula[i].isDigit()) i++;
  if (i == lastI) return false;

  iterator = i;
  return true;
}

void EvalBase::ignoreSpaces(const QString& formula, int& iterator) {
  while (1) {
    if (formula[iterator].isSpace())
      iterator++;
    else
      break;
  }
}

QVariant EvalBase::eval_funcaverage(const std::shared_ptr<astcalculator> ast,
                                    int row, int col) {
  /*
  AVERAGE OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE

FIELDEXCEL <- BARISKOLOMEXCEL SEPERATORFIELDEXCEL BARISKOLOMEXCEL
                        / BARISKOLOMEXCEL

BARISKOLOMEXCEL <- KOLOMEXCEL NUMBER
KOLOMEXCEL <- [a-zA-Z]+
SEPERATORFIELDEXCEL <- ':'

  always return double
  */
  const auto& nodes = ast->nodes;

  int argcount = 0;
  std::pair<bool, std::shared_ptr<astcalculator> > excel_var =
      isexistFieldExcel(nodes.at(2), Q_FUNC_INFO);
  QVariant lval = 0;
  if (excel_var.first) {
#ifdef ASKDEBUG
    qDebug() << "found tagname 2 funcsum" << excel_var.second->name.c_str()
             << "node size" << excel_var.second->nodes.size() << "";
#endif
    QList<double> tmp_lval;
    switch (excel_var.second->nodes.size()) {
      case 1:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        lval = valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str()),
            row, col);
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        argcount++;
        break;
      case 2:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                 << "tagname size" << excel_var.second->nodes.size()
                 << "child size" << excel_var.second->nodes.at(0)->nodes.size();
        qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                 << excel_var.second->nodes.at(1)->token.c_str();
#endif
        lval =
            valueOfCell(excel_var.second->nodes.at(0),
                        CalculatorPEG::newInstance()->getTable(),
                        QString("%1%2")
                            .arg(excel_var.second->nodes.at(0)->token.c_str())
                            .arg(excel_var.second->nodes.at(1)->token.c_str()),
                        row, col);
        argcount++;
        break;
      case 3:
        tmp_lval = extractDataRangeFromCell(
            excel_var.second->nodes.at(0),
            QString("%1%2:%3%4")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(1)->token.c_str()),
            CalculatorPEG::newInstance()->getTable(), row, col);
        for (int x = 0; x < tmp_lval.size(); x++) {
          lval = lval.toDouble ()+ tmp_lval.at(x);
        }
        argcount += tmp_lval.size();
        break;
      default:
        throw_runtime_error(excel_var.second,
                            "unhandled FIELDEXCEL parsing func_sum");
        break;
    }
  } else {
#ifdef ASKDEBUG
    qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
    lval =
        eval(nodes.at(2), CalculatorPEG::newInstance()->getTable(), row, col);
    argcount++;
  }
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
  throwIfNotNumberVariant(
      lval, nodes.at(2),
      "hasil input fungsi average harus bertipe angka atau boolean");
  if (nodes.size() == 4) {
    return lval;
  }
  int maxargcount = 255;
  for (decltype(nodes.size()) i = 3; i < nodes.size(); i += 2) {
#ifdef ASKDEBUG
    qDebug() << "eval_funcor i+1" << i + 1 << "nodes size()" << nodes.size();
#endif
    auto mx = i + 1;
    if (mx == nodes.size()) {
      break;
    }
    if (argcount == maxargcount) {
      throw_runtime_error(nodes.at(mx), "max input argumen average adalah 255");
    }
    excel_var.second = nullptr;
    QVariant rval = 0;
    excel_var = isexistFieldExcel(nodes.at(mx), Q_FUNC_INFO);
    if (excel_var.first) {
#ifdef ASKDEBUG
      qDebug() << "found tagname 2 funcaverage"
               << excel_var.second->name.c_str() << "node size"
               << excel_var.second->nodes.size() << "";
#endif
      QList<double> tmp_lval;
      switch (excel_var.second->nodes.size()) {
        case 1:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          rval = valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(0)
                           ->nodes.at(1)
                           ->token.c_str()),
              row, col);
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          argcount++;
          break;
        case 2:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                   << "tagname size" << excel_var.second->nodes.size()
                   << "child size"
                   << excel_var.second->nodes.at(0)->nodes.size();
          qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                   << excel_var.second->nodes.at(1)->token.c_str();
#endif
          rval = valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(excel_var.second->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(1)->token.c_str()),
              row, col);
          argcount++;
          break;
        case 3:
          tmp_lval = extractDataRangeFromCell(
              excel_var.second->nodes.at(0),
              QString("%1%2:%3%4")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(2)
                           ->nodes.at(1)
                           ->token.c_str()),
              CalculatorPEG::newInstance()->getTable(), row, col);
          for (int x = 0; x < tmp_lval.size(); x++) {
            rval = rval.toDouble ()+ tmp_lval.at(x);
          }
          argcount += tmp_lval.size();
          break;
        default:
          throw_runtime_error(excel_var.second,
                              "unhandled FIELDEXCEL parsing func_sum");
          break;
      }
    } else {
#ifdef ASKDEBUG
      qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
      rval = eval(nodes.at(mx), CalculatorPEG::newInstance()->getTable(), row,
                  col);
      argcount++;
    }
#ifdef ASKDEBUG
    qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
    throwIfNotNumberVariant(
        rval, nodes.at(mx),
        "input fungsi or harus bernilai angka atau boolean");
#ifdef ASKDEBUG
    qDebug() << "rval funcor " << rval;
#endif
    // selalu treat as double
    std::feclearexcept(FE_ALL_EXCEPT);
    double temp_ret =lval.toDouble ()+ rval.toDouble ();
    isoverflowDoubleValueHappen(nodes.at(mx));
    if (!TestOverflow::validPresisiNumber(temp_ret)) {
      throw_runtime_error(
          nodes.at(mx),
          "error invalid double average (isinfinite  atau isNAN)?");
    }
    lval = temp_ret;
  }
#ifdef ASKDEBUG
  qDebug() << "lval average" << lval << "argcount" << argcount;
#endif
  std::feclearexcept(FE_ALL_EXCEPT);
  double tmp =lval.toDouble ()/ argcount;

  lval = QVariant();
  isoverflowDoubleValueHappen(ast);
  if (!TestOverflow::validPresisiNumber(tmp)) {
    throw_runtime_error(
        ast, "error invalid double average (isinfinite  atau isNAN)?");
  }
  return tmp;
}

QVariant EvalBase::eval_funcmax(const std::shared_ptr<astcalculator> ast,
                                int row, int col) {
  /*
   MAX OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
  */
  const auto& nodes = ast->nodes;

  QList<QVariant> lval;
  std::pair<bool, std::shared_ptr<astcalculator> > excel_var =
      isexistFieldExcel(nodes.at(2), Q_FUNC_INFO);

  if (excel_var.first) {
#ifdef ASKDEBUG
    qDebug() << "found tagname 2 eval_funcmax" << excel_var.second->name.c_str()
             << "node size" << excel_var.second->nodes.size() << "";
#endif
    QList<double> tmp_lval;
    switch (excel_var.second->nodes.size()) {
      case 1:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        lval << valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str()),
            row, col);
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        break;
      case 2:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                 << "tagname size" << excel_var.second->nodes.size()
                 << "child size" << excel_var.second->nodes.at(0)->nodes.size();
        qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                 << excel_var.second->nodes.at(1)->token.c_str();
#endif
        lval << valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(1)->token.c_str()),
            row, col);
        break;
      case 3:
        tmp_lval = extractDataRangeFromCell(
            excel_var.second->nodes.at(0),
            QString("%1%2:%3%4")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(1)->token.c_str()),
            CalculatorPEG::newInstance()->getTable(), row, col);
        for (int x = 0; x < tmp_lval.size(); x++) {
          lval << tmp_lval.at(x);
        }
        break;
      default:
        throw_runtime_error(excel_var.second,
                            "unhandled FIELDEXCEL parsing func_sum");
        break;
    }
  } else {
#ifdef ASKDEBUG
    qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
    lval << eval(nodes.at(2), CalculatorPEG::newInstance()->getTable(), row,
                 col);
  }
  throwIfNotNumberVariant(lval, nodes.at(2),
                          "input fungsi max harus angka atau double");
  auto getmax = [](const QList<QVariant>& d) {
      if(d.size ()==0) return QVariant(0);
      if(d.size ()==1) return d.at (0);
    QVariant max = d.at (0);
    for (auto& l : d) {
        auto lhs=max.toDouble ();
        auto rhs=l.toDouble ();
      if (lhs < rhs) max = l;
    }
    return max;
  };

  if (nodes.size() == 4) {
    return getmax(lval);
  }
  // dianggap lval sebagai nilai max
  for (decltype(nodes.size()) x = 3; x < nodes.size(); x += 2) {
    auto maxi = x + 1;
    if (maxi == nodes.size()) {
      break;
    }
    if (lval.size() == 259) {
      throw_runtime_error(ast, "max input argument max harus 255");
    }
    excel_var.second = nullptr;
    excel_var = isexistFieldExcel(nodes.at(maxi), Q_FUNC_INFO);
    if (excel_var.first) {
#ifdef ASKDEBUG
      qDebug() << "found tagname 2 eval_funcmax"
               << excel_var.second->name.c_str() << "node size"
               << excel_var.second->nodes.size() << "";
#endif
      QList<double> tmp_lval;
      switch (excel_var.second->nodes.size()) {
        case 1:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          lval << valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(0)
                           ->nodes.at(1)
                           ->token.c_str()),
              row, col);
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          break;
        case 2:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                   << "tagname size" << excel_var.second->nodes.size()
                   << "child size"
                   << excel_var.second->nodes.at(0)->nodes.size();
          qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                   << excel_var.second->nodes.at(1)->token.c_str();
#endif
          lval << valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(excel_var.second->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(1)->token.c_str()),
              row, col);
          break;
        case 3:
          tmp_lval = extractDataRangeFromCell(
              excel_var.second->nodes.at(0),
              QString("%1%2:%3%4")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(2)
                           ->nodes.at(1)
                           ->token.c_str()),
              CalculatorPEG::newInstance()->getTable(), row, col);
          for (int x = 0; x < tmp_lval.size(); x++) {
            lval << tmp_lval.at(x);
          }
          break;
        default:
          throw_runtime_error(excel_var.second,
                              "unhandled FIELDEXCEL parsing func_sum");
          break;
      }
    } else {
#ifdef ASKDEBUG
      qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
      lval << eval(nodes.at(maxi), CalculatorPEG::newInstance()->getTable(),
                   row, col);
    }
    throwIfNotNumberVariant(lval, nodes.at(maxi),
                            "input fungsi max harus angka atau double");
  }
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED size" << lval.size();
#endif
  return getmax(lval);
}

QVariant EvalBase::eval_funcmin(const std::shared_ptr<astcalculator> ast,
                                int row, int col) {
  /*
  MIN OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
  */
  const auto& nodes = ast->nodes;

  QList<QVariant> lval;
  std::pair<bool, std::shared_ptr<astcalculator> > excel_var =
      isexistFieldExcel(nodes.at(2), Q_FUNC_INFO);

  if (excel_var.first) {
#ifdef ASKDEBUG
    qDebug() << "found tagname 2 eval_funcmax" << excel_var.second->name.c_str()
             << "node size" << excel_var.second->nodes.size() << "";
#endif
    QList<double> tmp_lval;
    switch (excel_var.second->nodes.size()) {
      case 1:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        lval << valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str()),
            row, col);
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        break;
      case 2:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                 << "tagname size" << excel_var.second->nodes.size()
                 << "child size" << excel_var.second->nodes.at(0)->nodes.size();
        qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                 << excel_var.second->nodes.at(1)->token.c_str();
#endif
        lval << valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(1)->token.c_str()),
            row, col);
        break;
      case 3:
        tmp_lval = extractDataRangeFromCell(
            excel_var.second->nodes.at(0),
            QString("%1%2:%3%4")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(1)->token.c_str()),
            CalculatorPEG::newInstance()->getTable(), row, col);
        for (int x = 0; x < tmp_lval.size(); x++) {
          lval << tmp_lval.at(x);
        }
        break;
      default:
        throw_runtime_error(excel_var.second,
                            "unhandled FIELDEXCEL parsing func_sum");
        break;
    }
  } else {
#ifdef ASKDEBUG
    qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
    lval << eval(nodes.at(2), CalculatorPEG::newInstance()->getTable(), row,
                 col);
  }
  throwIfNotNumberVariant(lval, nodes.at(2),
                          "input fungsi max harus angka atau double");
  auto getmin = [](const QList<QVariant>& d) {
    if(d.size ()==0) return QVariant(0);
    if(d.size ()==1) return d.at (0);
    QVariant min =d.at (0);
    for (auto& l : d) {
        auto lhs =min.toDouble ();
        auto rhs=l.toDouble ();
      if (lhs >rhs) min = l;
    }
    #ifdef ASKDEBUG
    qDebug()<<"final min"<<min;
    #endif
    return min;
  };

  if (nodes.size() == 4) {
    return getmin(lval);
  }
  // dianggap lval sebagai nilai max
  for (decltype(nodes.size()) x = 3; x < nodes.size(); x += 2) {
    auto maxi = x + 1;
    if (maxi == nodes.size()) {
      break;
    }
    if (lval.size() == 259) {
      throw_runtime_error(ast, "max input argument max harus 255");
    }
    excel_var.second = nullptr;
    excel_var = isexistFieldExcel(nodes.at(maxi), Q_FUNC_INFO);
    if (excel_var.first) {
#ifdef ASKDEBUG
      qDebug() << "found tagname 2 eval_funcmax"
               << excel_var.second->name.c_str() << "node size"
               << excel_var.second->nodes.size() << "";
#endif
      QList<double> tmp_lval;
      switch (excel_var.second->nodes.size()) {
        case 1:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          lval << valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(0)
                           ->nodes.at(1)
                           ->token.c_str()),
              row, col);
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          break;
        case 2:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                   << "tagname size" << excel_var.second->nodes.size()
                   << "child size"
                   << excel_var.second->nodes.at(0)->nodes.size();
          qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                   << excel_var.second->nodes.at(1)->token.c_str();
#endif
          lval << valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(excel_var.second->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(1)->token.c_str()),
              row, col);
          break;
        case 3:
          tmp_lval = extractDataRangeFromCell(
              excel_var.second->nodes.at(0),
              QString("%1%2:%3%4")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(2)
                           ->nodes.at(1)
                           ->token.c_str()),
              CalculatorPEG::newInstance()->getTable(), row, col);
          for (int x = 0; x < tmp_lval.size(); x++) {
            lval << tmp_lval.at(x);
          }
          break;
        default:
          throw_runtime_error(excel_var.second,
                              "unhandled FIELDEXCEL parsing func_sum");
          break;
      }
    } else {
#ifdef ASKDEBUG
      qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
      lval << eval(nodes.at(maxi), CalculatorPEG::newInstance()->getTable(),
                   row, col);
    }
    throwIfNotNumberVariant(lval, nodes.at(maxi),
                            "input fungsi max harus angka atau double");
  }
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED size" << lval.size();
#endif
  return getmin(lval);
}

QVariant EvalBase::eval_function(const std::shared_ptr<astcalculator> ast,
                                 int row, int col) {
  /*
  FUNCTION  <- POW OPEN EXPRESSION COMMA EXPRESSION CLOSE
          / SUM OPEN EXPRESSION COMMA EXPRESSION CLOSE

  */
  const auto& nodes = ast->nodes;
  QString le(nodes.at(0)->name.c_str());
  bool valid = false;
// extract single lhs value and then determine its type operation
#ifdef ASKDEBUG
  qDebug() << "funcname" << le << "tagname" << nodes.at(0)->name.c_str()
           << "node size" << nodes.size();
#endif
  if ((le.compare(QLatin1String("power"), Qt::CaseInsensitive) == 0)) {
    valid = true;
    return eval_power(ast, row, col);
  }
  /*
  SUM OPEN EXPRESSION DOTCOMMA EXPRESSION CLOSE
  */
  if ((le.compare(QLatin1String("SUM"), Qt::CaseInsensitive) == 0)) {
    // qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__ << "
    // file " << __FILE__ << " node size :" << nodes.size();
    valid = true;
    return eval_funcsum(ast, row, col);
  }
  if ((le.compare(QLatin1String("average"), Qt::CaseInsensitive) == 0)) {
    // if we found single double value, then we should return it as double and
    // the rest is otherwise...
    return eval_funcaverage(ast, row, col);
  }
  if ((le.compare(QLatin1String("max"), Qt::CaseInsensitive) == 0)) {
    valid = true;
    return eval_funcmax(ast, row, col);
  }
  if ((le.compare(QLatin1String("min"), Qt::CaseInsensitive) == 0)) {
    valid = true;
    return eval_funcmin(ast, row, col);
  }
  if ((le.compare(QLatin1String("IF"), Qt::CaseInsensitive) == 0)) {
    /*
    IF OPEN EXPRESSIONCMP DOTCOMMA EXPRESSIONCMP DOTCOMMA EXPRESSIONCMP CLOSE
    */
    valid = true;
    return eval_funcif(ast, row, col);
  }
  if ((le.compare(QLatin1String("or"), Qt::CaseInsensitive) == 0) ||
      (le.compare(QLatin1String("atau"), Qt::CaseInsensitive) == 0)) {
    return eval_funcor(ast, row, col);
  }
  if ((le.compare(QLatin1String("and"), Qt::CaseInsensitive) == 0) ||
      (le.compare(QLatin1String("dan"), Qt::CaseInsensitive) == 0)) {
    return eval_funcand(ast, row, col);
  }
  if (le.compare(QLatin1String("NOT"), Qt::CaseInsensitive) == 0) {
    return eval_funcnot(ast, row, col);
  }
  if ((le.compare(QLatin1String("date"), Qt::CaseInsensitive) == 0)) {
    return eval_funcdate(ast);
  }
  return QVariant(0);
}

QVariant EvalBase::eval_funcif(const std::shared_ptr<astcalculator> ast,
                               int row, int col) {
  /*
  IF OPEN EXPRESSIONCMP DOTCOMMA EXPRESSIONCMP DOTCOMMA EXPRESSIONCMP CLOSE
  */
  const auto& nodes = ast->nodes;
  expectNodeSize(ast, Q_FUNC_INFO, 8);
#ifdef ASKDEBUG
  qDebug() << " logging " << Q_FUNC_INFO << " baris " << __LINE__
           << "nodes size" << ast->nodes.size() << "first eval nodes sizeif"
           << nodes.at(2)->nodes.size();
#endif
  const auto cmp_val =
      eval(nodes.at(2), CalculatorPEG::newInstance()->getTable(), row, col);
#ifdef ASKDEBUG
  qDebug() << "cmp_val" << cmp_val;
#endif
  if (cmp_val.isValid()) {
    if (cmp_val.toBool()) {
      return eval(nodes.at(4), CalculatorPEG::newInstance()->getTable(), row,
                  col);
    } else {
#ifdef ASKDEBUG
      qDebug() << "executed karena if false nodes size"
               << nodes.at(6)->nodes.size();
#endif
      return eval(nodes.at(6), CalculatorPEG::newInstance()->getTable(), row,
                  col);
    }
  }
#ifdef ASKDEBUG
  qDebug() << "warning got undefined if retval";
#endif
  return false;
}

QVariant EvalBase::eval_funcor(const std::shared_ptr<astcalculator> ast,
                               int row, int col) {
  /*
   OR OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
   */
  const auto nodes = ast->nodes;
  bool lastok = false;
  QList<QVariant> lval;
  std::pair<bool, std::shared_ptr<astcalculator> > excel_var =
      isexistFieldExcel(nodes.at(2), Q_FUNC_INFO);

  if (excel_var.first) {
#ifdef ASKDEBUG
    qDebug() << "found tagname 2 eval_funcmax" << excel_var.second->name.c_str()
             << "node size" << excel_var.second->nodes.size() << "";
#endif
    QList<double> tmp_lval;
    switch (excel_var.second->nodes.size()) {
      case 1:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        lval << valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str()),
            row, col);
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        break;
      case 2:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                 << "tagname size" << excel_var.second->nodes.size()
                 << "child size" << excel_var.second->nodes.at(0)->nodes.size();
        qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                 << excel_var.second->nodes.at(1)->token.c_str();
#endif
        lval << valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(1)->token.c_str()),
            row, col);
        break;
      case 3:
        tmp_lval = extractDataRangeFromCell(
            excel_var.second->nodes.at(0),
            QString("%1%2:%3%4")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(1)->token.c_str()),
            CalculatorPEG::newInstance()->getTable(), row, col);
        for (int x = 0; x < tmp_lval.size(); x++) {
          lval << tmp_lval.at(x);
        }
        break;
      default:
        throw_runtime_error(excel_var.second,
                            "unhandled FIELDEXCEL parsing func_sum");
        break;
    }
  } else {
#ifdef ASKDEBUG
    qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
    lval << eval(nodes.at(2), CalculatorPEG::newInstance()->getTable(), row,
                 col);
  }
  throwIfNotNumberVariant(lval, nodes.at(2),
                          "input fungsi max harus angka atau double");
  auto getor = [&lastok](const QList<QVariant>& d) {
    for (auto& l : d) {
      lastok = lastok || l.toDouble() != 0.0 || l.toDouble() != -0.0 ||
               l.toDouble() != 0 || l.toDouble() != -0;
    }
    return lastok;
  };

  if (nodes.size() == 4) {
    return getor(lval);
  }
  // dianggap lval sebagai nilai max
  for (decltype(nodes.size()) x = 3; x < nodes.size(); x += 2) {
    auto maxi = x + 1;
    if (maxi == nodes.size()) {
      break;
    }
    if (lval.size() == 259) {
      throw_runtime_error(ast, "max input argument max harus 255");
    }
    excel_var.second = nullptr;
    excel_var = isexistFieldExcel(nodes.at(maxi), Q_FUNC_INFO);
    if (excel_var.first) {
#ifdef ASKDEBUG
      qDebug() << "found tagname 2 eval_funcmax"
               << excel_var.second->name.c_str() << "node size"
               << excel_var.second->nodes.size() << "";
#endif
      QList<double> tmp_lval;
      switch (excel_var.second->nodes.size()) {
        case 1:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          lval << valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(0)
                           ->nodes.at(1)
                           ->token.c_str()),
              row, col);
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          break;
        case 2:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                   << "tagname size" << excel_var.second->nodes.size()
                   << "child size"
                   << excel_var.second->nodes.at(0)->nodes.size();
          qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                   << excel_var.second->nodes.at(1)->token.c_str();
#endif
          lval << valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(excel_var.second->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(1)->token.c_str()),
              row, col);
          break;
        case 3:
          tmp_lval = extractDataRangeFromCell(
              excel_var.second->nodes.at(0),
              QString("%1%2:%3%4")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(2)
                           ->nodes.at(1)
                           ->token.c_str()),
              CalculatorPEG::newInstance()->getTable(), row, col);
          for (int x = 0; x < tmp_lval.size(); x++) {
            lval << tmp_lval.at(x);
          }
          break;
        default:
          throw_runtime_error(excel_var.second,
                              "unhandled FIELDEXCEL parsing func_sum");
          break;
      }
    } else {
#ifdef ASKDEBUG
      qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
      lval << eval(nodes.at(maxi), CalculatorPEG::newInstance()->getTable(),
                   row, col);
    }
    throwIfNotNumberVariant(lval, nodes.at(maxi),
                            "input fungsi max harus angka atau double");
  }
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED size" << lval.size();
#endif
  return getor(lval);
}

QVariant EvalBase::eval_funcand(const std::shared_ptr<astcalculator> ast,
                                int row, int col) {
  /*
   AND OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
   * */

  const auto nodes = ast->nodes;
  bool lastok = true;
  QList<QVariant> lval;
  std::pair<bool, std::shared_ptr<astcalculator> > excel_var =
      isexistFieldExcel(nodes.at(2), Q_FUNC_INFO);

  if (excel_var.first) {
#ifdef ASKDEBUG
    qDebug() << "found tagname 2 eval_funcmax" << excel_var.second->name.c_str()
             << "node size" << excel_var.second->nodes.size() << "";
#endif
    QList<double> tmp_lval;
    switch (excel_var.second->nodes.size()) {
      case 1:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        lval << valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str()),
            row, col);
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
        break;
      case 2:
#ifdef ASKDEBUG
        qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                 << "tagname size" << excel_var.second->nodes.size()
                 << "child size" << excel_var.second->nodes.at(0)->nodes.size();
        qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                 << excel_var.second->nodes.at(1)->token.c_str();
#endif
        lval << valueOfCell(
            excel_var.second->nodes.at(0),
            CalculatorPEG::newInstance()->getTable(),
            QString("%1%2")
                .arg(excel_var.second->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(1)->token.c_str()),
            row, col);
        break;
      case 3:
        tmp_lval = extractDataRangeFromCell(
            excel_var.second->nodes.at(0),
            QString("%1%2:%3%4")
                .arg(excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                .arg(excel_var.second->nodes.at(2)->nodes.at(1)->token.c_str()),
            CalculatorPEG::newInstance()->getTable(), row, col);
        for (int x = 0; x < tmp_lval.size(); x++) {
          lval << tmp_lval.at(x);
        }
        break;
      default:
        throw_runtime_error(excel_var.second,
                            "unhandled FIELDEXCEL parsing func_sum");
        break;
    }
  } else {
#ifdef ASKDEBUG
    qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
    lval << eval(nodes.at(2), CalculatorPEG::newInstance()->getTable(), row,
                 col);
  }
  throwIfNotNumberVariant(lval, nodes.at(2),
                          "input fungsi max harus angka atau double");
  auto getand = [&lastok](const QList<QVariant>& d) {
    for (auto& l : d) {
#ifdef ASKDEBUG
      qDebug() << "getand lastok" << lastok << "l" << l;
#endif
      lastok = lastok && l.toDouble() != 0.0 && l.toDouble() != -0.0 &&
               l.toDouble() != 0 && l.toDouble() != -0;
    }
    return lastok;
  };

  if (nodes.size() == 4) {
    return getand(lval);
  }
  // dianggap lval sebagai nilai max
  for (decltype(nodes.size()) x = 3; x < nodes.size(); x += 2) {
    auto maxi = x + 1;
    if (maxi == nodes.size()) {
      break;
    }
    if (lval.size() == 259) {
      throw_runtime_error(ast, "max input argument max harus 255");
    }
    excel_var.second = nullptr;
    excel_var = isexistFieldExcel(nodes.at(maxi), Q_FUNC_INFO);
    if (excel_var.first) {
#ifdef ASKDEBUG
      qDebug() << "found tagname 2 eval_funcmax"
               << excel_var.second->name.c_str() << "node size"
               << excel_var.second->nodes.size() << "";
#endif
      QList<double> tmp_lval;
      switch (excel_var.second->nodes.size()) {
        case 1:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          lval << valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(0)
                           ->nodes.at(1)
                           ->token.c_str()),
              row, col);
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
          break;
        case 2:
#ifdef ASKDEBUG
          qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED"
                   << "tagname size" << excel_var.second->nodes.size()
                   << "child size"
                   << excel_var.second->nodes.at(0)->nodes.size();
          qDebug() << "excel_var.second->nodes.at (1)->token.c_str()"
                   << excel_var.second->nodes.at(1)->token.c_str();
#endif
          lval << valueOfCell(
              excel_var.second->nodes.at(0),
              CalculatorPEG::newInstance()->getTable(),
              QString("%1%2")
                  .arg(excel_var.second->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(1)->token.c_str()),
              row, col);
          break;
        case 3:
          tmp_lval = extractDataRangeFromCell(
              excel_var.second->nodes.at(0),
              QString("%1%2:%3%4")
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(0)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(0)->nodes.at(1)->token.c_str())
                  .arg(
                      excel_var.second->nodes.at(2)->nodes.at(0)->token.c_str())
                  .arg(excel_var.second->nodes.at(2)
                           ->nodes.at(1)
                           ->token.c_str()),
              CalculatorPEG::newInstance()->getTable(), row, col);
          for (int x = 0; x < tmp_lval.size(); x++) {
            lval << tmp_lval.at(x);
          }
          break;
        default:
          throw_runtime_error(excel_var.second,
                              "unhandled FIELDEXCEL parsing func_sum");
          break;
      }
    } else {
#ifdef ASKDEBUG
      qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED";
#endif
      lval << eval(nodes.at(maxi), CalculatorPEG::newInstance()->getTable(),
                   row, col);
    }
    throwIfNotNumberVariant(lval, nodes.at(maxi),
                            "input fungsi max harus angka atau double");
  }
#ifdef ASKDEBUG
  qDebug() << Q_FUNC_INFO << __LINE__ << "CALLED size" << lval.size();
#endif
  return getand(lval);
}

QVariant EvalBase::eval_funcdate(const std::shared_ptr<astcalculator> ast) {
  /*
  DATE OPEN NUMBER DOTCOMMA NUMBER DOTCOMMA NUMBER CLOSE
  */
  const auto& nodes = ast->nodes;
  QVariant y = eval_number(nodes.at(1)), m = eval_number(nodes.at(2)),
           d = eval_number(nodes.at(3));
  QDate date(y.toInt(), m.toInt(), d.toInt());
  if (!date.isValid()) {
    throw_runtime_error(nodes.at(0), "error invalid date terdeteksi");
  }
  return date.toString("d/M/yyyy");
}

std::pair<bool, QString> EvalBase::eval_stringtime(const QString& t) {
  const auto a1 = QTime::fromString(t, "h:m");
  if (!a1.isValid()) {
    // try second format
    const auto a2 = QTime::fromString(t, "hh:mm");
    if (!a2.isValid()) {
      return std::make_pair(false, "");
    }
    return std::make_pair(true, "hh:mm");
  }
  return std::make_pair(true, "h:m");
}

QVariant EvalBase::eval_funcnot(const std::shared_ptr<astcalculator> ast,
                                int row, int col) {
  /*
  NOT OPEN EXPRESSIONCMP CLOSE
  */
  expectNodeSize(ast, Q_FUNC_INFO, 4);
  auto var = eval(ast->nodes.at(2), CalculatorPEG::newInstance()->getTable(),
                  row, col);
  throwIfNotNumberVariant(var, ast,
                          "input fungsi not harus bernilai angka atau boolean");
  switch (var.type()) {
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::ULongLong:
    case QVariant::LongLong:
      return var.toLongLong() == 0;
    default:
      return var.toDouble() == 0.0;
  }
}

std::pair<bool, QString> EvalBase::eval_stringdate(const QString& d) {
  const auto a1 = QDate::fromString(d, "d/M/yyyy");
  if (!a1.isValid()) {
    // try second format
    const auto a2 = QDate::fromString(d, "d-M-yyyy");
    if (!a2.isValid()) {
      // try another one
      const auto a3 = QDate::fromString(d, "d MMMM yyyy");
      if (!a3.isValid()) {
        return std::make_pair(false, "");
      }
      return std::make_pair(true, "d MMMM yyyy");
    }
    return std::make_pair(true, "d-M-yyyy");
  }
  return std::make_pair(true, "d/M/yyyy");
}

void EvalBase::expectNodeSize(const std::shared_ptr<astcalculator> ast,
                              const char* func, size_t n) {
  if (ast->nodes.size() != n) {
    std::stringstream stm;
    stm << func << " expect node size " << n << "\n";
    std::string str;
    stm >> str;
    throw_runtime_error(ast, str.c_str());
  }
}

void EvalBase::expectTwoSameVariantType(
    const std::shared_ptr<astcalculator> ast, const char* func, QVariant& a,
    QVariant& b) {
  if ((a.type() == QVariant::Bool) &&
      (b.type() == QVariant::LongLong || b.type() == QVariant::Int ||
       b.type() == QVariant::Double)) {
    a = a.toBool() ? 1 : 0;
    return;
  }
  if ((a.type() == QVariant::LongLong || a.type() == QVariant::Int ||
       a.type() == QVariant::Double) &&
      (b.type() == QVariant::Bool)) {
    b = b.toBool() ? 1 : 0;
    return;
  }
  if ((a.type() == QVariant::LongLong || a.type() == QVariant::Int) &&
      (b.type() == QVariant::LongLong || b.type() == QVariant::Int)) {
    return;
  }

  if (a.type() == QVariant::Double && b.type() == QVariant::Double) {
    return;
  }
  if (a.type() == QVariant::Double &&
      (b.type() == QVariant::LongLong || b.type() == QVariant::Int)) {
    return;
  }
  if ((a.type() == QVariant::LongLong || a.type() == QVariant::Int) &&
      b.type() == QVariant::Double) {
    return;
  }
  if (a.type() == QVariant::Date && b.type() == QVariant::Date) {
#ifdef ASKDEBUG
    qDebug() << "expected a d date compare" << __FILE__ << __LINE__;
#endif
    return;
  }
  if (a.type() == QVariant::Time && b.type() == QVariant::Time) {
    return;
  }
  std::stringstream stm;
  std::string msg;
  stm << func << " error nilai a dan b harus sama ";
  stm >> msg;
  throw_runtime_error(ast, msg.c_str());
}

bool EvalBase::evalBooleanFromVariant(const QVariant& var) {
  bool decision = false;
  switch (var.type()) {
    case QVariant::Int:
    case QVariant::ULongLong:
    case QVariant::UInt:
    case QVariant::LongLong:
      decision = var.toLongLong() != 0;
      break;
    case QVariant::Bool:
      decision = var.toBool();
      break;
    default:
      decision = var.toDouble() != 0.0;
      break;
  }
  return decision;
}

bool EvalBase::isoverflowDoubleValueHappen(
    const std::shared_ptr<astcalculator> ast) {
  if (std::fetestexcept(FE_DIVBYZERO)) {
    throw_runtime_error(ast, "error divide by zero ");
  }
  if (std::fetestexcept(FE_UNDERFLOW)) {
    throw_runtime_error(ast, "error underflow exception");
  }
  if (std::fetestexcept(FE_INVALID)) {
    throw_runtime_error(ast, "error invalid exception number");
  }
  if (std::fetestexcept(FE_OVERFLOW)) {
    throw_runtime_error(ast, "error overflow exception");
  }
  return false;
}

void EvalBase::throwIfNotNumberVariant(QVariant& var,
                                       const std::shared_ptr<astcalculator> ast,
                                       const char* msg) {
  switch (var.type()) {
    case QVariant::ByteArray:
    case QVariant::String:
    case QVariant::Date:
    case QVariant::Time:
      throw_runtime_error(ast, msg);
    case QVariant::Bool:
      var = var.toBool() ? 1 : 0;
      break;
    default:
      break;
  }
}

void EvalBase::throwIfNotNumberVariant(QList<QVariant>& var,
                                       const std::shared_ptr<astcalculator> ast,
                                       const char* msg) {
  for (auto& x : var) {
    throwIfNotNumberVariant(x, ast, msg);
  }
}

void throw_runtime_error(const std::shared_ptr<astcalculator> node,
                         const std::string& msg) {
  throw std::runtime_error(
      format_error_message(node->path, node->line, node->column, msg));
}

void throw_runtime_error(const std::string& node, const std::string& msg) {
  QString error = QString("\nerror %1  %2").arg(node.c_str()).arg(msg.c_str());
  throw std::runtime_error(error.toStdString());
}

std::string format_error_message(const std::string& path, size_t ln, size_t col,
                                 const std::string& msg) {
  std::stringstream ss;
  if (std::strcmp(msg.c_str(), "#VALUE!") == 0) {
    return msg;
  }
  if (std::strcmp(msg.c_str(), "Err:522") == 0) {
    return msg;
  }
  ss << "\n" << path << ":" << ln << ":" << col << ": " << msg << std::endl;
  return ss.str();
}
