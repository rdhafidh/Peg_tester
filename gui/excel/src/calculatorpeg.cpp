#include "calculatorpeg.h"
#include <Table.h>
#include <evalbase.h>

CalculatorPEG::CalculatorPEG() {
  lastvariant = 0;
  lastformula = "";
}

CalculatorPEG::~CalculatorPEG() {}

QVariant CalculatorPEG::hitung(Table *t, int row, int col) {
  table = t;
  QString formula = table->getItemText(row, col, Qt::EditRole);

  if (!EvalBase::isFormula(formula)) return formula;
  formula = formula.toUpper();
  peg::parser parser;
  const char *g = R"(
                 STATEMENT <- EXPRESSIONCMP _ 
                             / EXPIFTAIL _
                 
                 EXPIFTAIL <- JIKAIF _ EXPIFCMPAND  _ JIKATHEN _ EXPIFVALUE _ JIKAELSE _ EXPIFVALUE
                             / JIKAIF _ EXPIFCMPOR _ JIKATHEN _ EXPIFVALUE _  JIKAELSE _ EXPIFVALUE
                 
                 EXPIFCMPAND <- EXPRESSIONCMP ( _ AND _ EXPRESSIONCMP)*
                 EXPIFCMPOR	<- EXPRESSIONCMP ( _ OR _ EXPRESSIONCMP)*
                 
                 EXPIFVALUE <- EXPIFTAIL
                         / EXPRESSIONCMP
                 
                 ~_ <- [ \t\r\n]*
                 
                 NUMBERDOUBLE <- FRACTIONALCONSTANT EXPONENTPART?
                 
                 FRACTIONALCONSTANT <- NUMBER DOT NUMBER
                 
                 EXPONENTPART <- SIGNEXPONENT SIGN? NUMBER
                 
                 FUNCTION  <- POWER OPEN EXPRESSIONCMP DOTCOMMA EXPRESSIONCMP CLOSE
                         / SUM OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
                         / IF OPEN EXPRESSIONCMP DOTCOMMA EXPRESSIONCMP DOTCOMMA EXPRESSIONCMP CLOSE
                         / OR OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
                         / AND OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
                         / NOT OPEN EXPRESSIONCMP CLOSE
                         / AVERAGE OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
                         / MAX OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
                         / MIN OPEN EXPRESSIONCMP (DOTCOMMA EXPRESSIONCMP)* CLOSE
                 
                 FACTOR  <- OPEN _ EXPRESSIONCMP _ CLOSE _
                             / BENAR
                             / SALAH
                             / FIELDEXCEL
                             / STRINGVALUE
                             / NUMBERDOUBLE 
                             / NUMBER 
                             / FUNCTION
                             
                 TERM <- FACTOR (FACTOR_OP FACTOR)*
                 EXPRESSION <- SIGN TERM (TERM_OP TERM)*
                     
                 EXPRESSIONCMP <- EXPRESSION (CMPOP EXPRESSION)*
                 
                 STRINGVALUE <- PETIK STRING PETIK
                 FACTOR_OP  <- < [*/] > _
                 SIGN       <- < [-+]? > _
                 TERM_OP    <- < [-+] > _
                 CMPOP <-  '<>'
                         / '<='
                         / '>=' 
                         / '!='
                         / '<' 
                         / '>' 
                         / '=' 
                 
                 SIGNEXPONENT <- [Ee]
                 POWER  <- [Pp] [Oo] [Ww] [Ee] [Rr]
                 SUM <- [Ss] [Uu] [Mm]
                 DATE <- [Dd] [Aa] [Tt] [Ee]
                 OPEN <-  '('
                 CLOSE <- ')'
                 DOTCOMMA <- ';'
                 SALAH <- [Ss] [Aa] [Ll] [Aa] [Hh]
                                 / [Ff] [Aa] [Ll] [Ss] [Ee]
                 BENAR <- [Tt] [Rr] [Uu] [Ee]
                                 / [bB] [Ee] [Nn] [Aa] [Rr]
                 NOT <- [Nn] [Oo] [Tt]
                         / [Bb] [Uu] [Kk] [Aa] [Nn]
                 
                 AVERAGE	<- [Aa] [Vv] [Ee] [Rr] [Aa] [Gg] [eE]
                 
                 MAX <- [Mm] [Aa] [Xx]
                 
                 MIN <- [Mm] [Ii] [nN]
                                
                 AND <- [Aa] [Nn] [Dd]
                         / DAN	
                         
                 DAN <- [Dd] [Aa] [Nn]			   
                 OR <- [Oo] [Rr]
                        / ATAU
                 ATAU <- [Aa] [Tt] [Aa] [Uu]
                 JIKAELSE <- [Ss] [Ee] [Bb] [Aa] [Ll] [Ii] [Kk] [Nn] [Yy] [Aa]
                     / ELSE
                 JIKATHEN <- [Mm] [Aa] [Kk] [Aa]
                 JIKAIF <- [Jj] [Ii] [Kk] [Aa] 
                         / IF
                 IF <- [Ii] [fF]
                 ELSE <- [Ee] [Ll] [Ss] [Ee]
                 DOT <-   ','
                 PETIK <- '"'
                 STRING <-  (!["] .)*
                 
                 FIELDEXCEL <- BARISKOLOMEXCEL SEPERATORFIELDEXCEL BARISKOLOMEXCEL
                             / BARISKOLOMEXCEL
                             
                 BARISKOLOMEXCEL <- KOLOMEXCEL NUMBER
                 KOLOMEXCEL <- [a-zA-Z]+
                 SEPERATORFIELDEXCEL <- ':'
                 
                 NUMBER <-  [0-9]+
             )";
  if (!parser.load_grammar(g)) {
    return QString("load grammar failed");
  }
  parser.enable_ast<astcalculator>();
  std::shared_ptr<astcalculator> ast;
  QString errormsg;
  parser.log = [&errormsg](size_t ln, size_t col, const std::string &msg) {
    errormsg =
        QString::fromStdString(format_error_message("inline", ln, col, msg));
  };
  formula = formula.remove(0, 1);
  #ifdef ASKDEBUG
  std::cout << "\ndata " << formula.toUtf8().constData() << "\n";
#endif
  if (!parser.parse_n(formula.toUtf8().constData(), formula.size(), ast,
                      "inline")) {
    return errormsg;
  }
  ast = peg::AstOptimizer(true).optimize(ast);
  try {
    QVariant retval= EvalBase::eval(ast, table, row, col);
    switch (retval.type ()){ 
    case QVariant::Bool:
        switch (retval.toBool ()){
        case 0:
                return QVariant("salah");
        default:
                return QVariant("benar");
        }
    default:
        return retval;
    }
  } catch (const std::exception &a) {
    return QString(a.what());
  } catch (...) {
    return QString("unknown error");
  }
}

Table *CalculatorPEG::getTable() { return table; }

CalculatorPEG *CalculatorPEG::newInstance() {
  static CalculatorPEG calc;
  return &calc;
}

bool CalculatorPEG::isformula(const QString &data)
{
    return EvalBase::isFormula(data);
}
