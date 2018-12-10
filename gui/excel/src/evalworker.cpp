#include "evalworker.h"
#include <evalbase.h>

EvalWorker::EvalWorker() {}

bool EvalWorker::tryEval(const char *g, const QByteArray &data,const char*path) {
  peg::parser parser;
  if (!parser.load_grammar(g)) {
    std::cout << "\nload grammar failed";
    return false;
  }
  parser.enable_ast<astcalculator>();
   std::shared_ptr<astcalculator> ast;
   parser.log = [&](size_t ln, size_t col, const std::string& msg) {
           std::cout << format_error_message(path, ln, col, msg) << std::endl;
       };
  std::cout<<"\ndata "<<data.constData ()<<"\n";
  if (!parser.parse_n(data.constData (), data.size(), ast,path)) {
    std::cout << "\nparse failed";
    return false;
  }
  ast = peg::AstOptimizer(true).optimize(ast);
  try { 
      std::cout << "\n" << EvalBase::eval(ast).toString().toStdString ();
    return true;
  } catch (const std::exception &a) {
    std::cout << "\nerror executing: " << a.what() << "\n";
    return false;
  }
  catch(...){
      std::cout << "\nunknown error\n";
      return false;
  }
}
