#include "parserlint.h"
#include <qtcsv/reader.h>
#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/schema.h>
#include <QFileInfo>
#include <fstream>
#include <sstream>
#include <utility>

bool read_file(const char *path, std::vector<char> &buff) {
  std::ifstream ifs(path, std::ios::in | std::ios::binary);
  if (ifs.fail()) {
    return false;
  }

  buff.resize(static_cast<unsigned int>(ifs.seekg(0, std::ios::end).tellg()));
  if (!buff.empty()) {
    ifs.seekg(0, std::ios::beg)
        .read(&buff[0], static_cast<std::streamsize>(buff.size()));
  }
  return true;
}
ParserLint::ParserLint() {}

ParserDef::errcode ParserLint::parsetest(const char *gram, char *inputtestfile,
                                         bool enable_ast, bool use_thread,
                                         bool verbose) {
  peg::parser peg;
  // cek grammar
  if (verbose) {
    peg.log = [&](auto ln, auto col, const auto &msg) {
      std::cerr << gram << ":" << ln << ":" << col << ": " << msg << std::endl;
    };
  }

  if (!isExistFile(QLatin1String(gram))) {
    return ParserDef::GRAMMAR_NOTFOUND;
  }
  std::vector<char> syntax;
  if (!read_file(gram, syntax)) {
    return ParserDef::GRAMMAR_ERROR;
  }
  if (!peg.load_grammar(syntax.data(), syntax.size())) {
    std::cout << "load_grammar error\n";
    return ParserDef::GRAMMAR_ERROR;
  }
  // cek input source test file
  if (!isExistFile(QLatin1String(inputtestfile))) {
    return ParserDef::INPUTTEST_NOTFOUND;
  }
  const char *source_path = inputtestfile;

  std::vector<char> source;
  if (!read_file(source_path, source)) {
    if (verbose) {
      std::cout << "input test file tidak ditemukan atau error\n";
    }

    return ParserDef::INPUTTEST_NOTFOUND;
  }
  if (verbose) {
    peg.log = [&](auto ln, auto col, const auto &msg) {
      std::cerr << source_path << ":" << ln << ":" << col << ": " << msg
                << std::endl;
    };
  }

  std::shared_ptr<peg::Ast> ast;
  if (enable_ast) {
    peg.enable_ast();
    if (!use_thread) {
      if (!peg.parse_n(source.data(), source.size(), ast)) {
        return ParserDef::PARSE_ERROR;
      }
    }
    if (use_thread) {
      std::future<int> m_p =
          std::async(std::launch::async, [&peg, &source, &ast]() {
            if (!peg.parse_n(source.data(), source.size(), ast)) {
              return 0;
            }
            return 1;
          });
      m_p.wait();
      if (m_p.get() == 0) {
        return ParserDef::PARSE_ERROR;
      }
    }
    ast = peg::AstOptimizer(true).optimize(ast);
    std::cout << "\n" << peg::ast_to_s(ast);
  } else {
    if (!use_thread) {
      if (!peg.parse_n(source.data(), source.size())) {
        return ParserDef::PARSE_ERROR;
      }
    }
    if (use_thread) {
      std::future<int> m_p =
          std::async(std::launch::async, [&peg, &source, &ast]() {
            if (!peg.parse_n(source.data(), source.size(), ast)) {
              return 0;
            }
            return 1;
          });
      m_p.wait();
      if (m_p.get() == 0) {
        return ParserDef::PARSE_ERROR;
      }
    }
  }
  return ParserDef::SUCCESS;
}

ParserDef::errcode ParserLint::parsecsv(const char *fname, bool verbose) {
  if (!isExistFile(fname)) {
    return ParserDef::INPUTTEST_NOTFOUND;
  }
  QFileInfo fl(fname);
  if (verbose)
    std::cout << "\n using input csv " << fl.absoluteFilePath().toStdString()
              << "\n";
  QList<QStringList> st = QtCSV::Reader::readToList(fl.absoluteFilePath());
  switch (st.size()) {
    case 0:
      if (verbose) std::cout << "\nparsing csv failed?\n";
      return ParserDef::PARSE_ERROR;
    default:
      break;
  }
  if (verbose) {std::cout << "\nparsing csv ok\n";
      for (auto &v : st) {
        for (auto &c : v) {
          std::cout << "\n data "
                    << c.toStdString();
        }
      }
  }
  return ParserDef::SUCCESS;
}

ParserDef::errcode ParserLint::parsejson(const char *fname, bool verbose) {
  if (!isExistFile(fname)) {
    return ParserDef::INPUTTEST_NOTFOUND;
  }
  std::ifstream ifs(fname);
  rapidjson::IStreamWrapper isw(ifs);
  rapidjson::Document d;
  auto &&ret = d.ParseStream(isw);
  if (ret.HasParseError()) { 
    if (verbose) {
      std::cout << "\n" << fname << " parse file json gagal\n";
    }
    return ParserDef::PARSE_ERROR;
  }
  if (!ret.IsArray()) {
    if (verbose) {
      std::cout << "\n" << fname << " bukan file json array\n";
    }
    return ParserDef::PARSE_ERROR;
  }
  if (ret.Size() == 0) {
    if (verbose) {
      std::cout << "\n" << fname << " file json array kosong\n";
    }
    return ParserDef::PARSE_ERROR;
  }
  if (!ret[0].IsObject()) {
    if (verbose) {
      std::cout << "\n"
                << fname << " file json expect setiap item json object\n";
    }
    return ParserDef::PARSE_ERROR;
  }
  auto getvalueasstring =
      [](const rapidjson::GenericValue<
          rapidjson::UTF8<char>,
          rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> &val)
      -> QString {
    QString retval;
    switch (val.GetType()) {
      case rapidjson::kNullType:
        break;
      case rapidjson::kNumberType:
        if (val.IsInt64()) {
          retval = retval.number(val.GetInt64());
        } else if (val.IsInt()) {
          retval = retval.number(val.GetInt());
        } else if (val.IsFloat()) {
          retval = retval.number(val.GetFloat());
        } else if (val.IsDouble()) {
          retval = retval.number(val.GetDouble());
        }
        break;
      case rapidjson::kFalseType:
        retval = "0";
        break;
      case rapidjson::kTrueType:
        retval = "1";
        break;
      case rapidjson::kStringType:
        retval = QString::fromLocal8Bit(val.GetString());
        break;
      default:
        break;
    }
    return retval;
  };

  QList<JSONROW> json;
  for (decltype(ret.Size()) x = 0; x < ret.MemberCount(); x++) {
    JSONROW item;
    for (auto &m : ret[x].GetObject()) {
      item << std::make_pair(QString(m.name.GetString()),
                             getvalueasstring(m.value));
    }
    json << item;
  }
  if (verbose) {
    std::cout << "\nparsing json ok\n";
    for (auto &v : json) {
      for (auto &c : v) {
        std::cout << "\nkolom " << c.first.toStdString() << " data "
                  << c.second.toStdString();
      }
    }
  }
  return ParserDef::SUCCESS;
}

ParserDef::errcode ParserLint::parseselfjson(const char *fname, bool verbose) {
  peg::parser peg;
  auto gram = R"(
              json <- KOMENTAR? array (ESC? array)* ESC? / KOMENTAR? object ESC?
              object <- '{' ESC? pair  ESC? '}' /   '{' ESC?  '}'  
              pair <-  STRING ESC? ':' ESC? value ESC? ( ',' ESC? STRING ESC? ':' ESC? value)*  / STRING ESC? ':' ESC? value 
              array <- '[' ESC? value ESC? (',' ESC? value)* ESC? ']' / '[' ESC? ']'
              value <-   STRING /   NUMBER /   object  /   array   /   TRUE   /   FALSE /   NULL
              STRING <- PETIK ESC* TEXTDATA* ESC* PETIK
              TEXTDATA    <- (!["] .)
              ~ESC <-   [ \n\r\t]+ 
              ~KOMENTAR <- KOMENOPENCLOSE  NLKOMEN* KOMENDOUBLESLASH*
              KOMENOPENCLOSE <- KOMENOPEN  NLKOMEN* SPACEKOMEN* TEXTKOMEN* NLKOMEN* SPACEKOMEN* KOMENCLOSE
              KOMENDOUBLESLASH <- '/' '/' SPACEKOMEN* TEXTKOMENDOUBLESLASH* ESC*
              KOMENOPEN <- '/' '*' 
              KOMENCLOSE <- '*' '/' 
              NLKOMEN <- [\n]+
              SPACEKOMEN <- [ ]+
              TEXTKOMEN <- (!KOMENCLOSE .)
              TEXTKOMENDOUBLESLASH <- (![\n] .)
              PETIK <- '"'
              TRUE <- 'true'
              FALSE <- 'false'
              NULL <- 'null'
              HEX <- [0-9a-fA-F] 
              NUMBER <-   '-'? INT '.' [0-9]+ EXP? # // 1.35, 1.35E-9, 0.3, -4.5
                  /   '-'? INT EXP             #  1e10 -3e4
                  /   '-'? INT                 # -3, 45
              INT <-   '0' / [1-9] [0-9]*  
              EXP <-   [Ee] ('+' / '-')? INT               
              )";
  if (!peg.load_grammar(gram)) {
    if (verbose) {
      std::cout << "\ninternal error load json grammar error..";
    }
    return ParserDef::GRAMMAR_ERROR;
  }
  QList<JSONROW> json;

  peg["STRING"] = [](const peg::SemanticValues &sv) { return sv.str(); };
  peg["value"] = [](const peg::SemanticValues &sv) { return sv.str(); };
  peg["pair"] = [&json](const peg::SemanticValues &sv) {
    JSONROW item;
    for (decltype(sv.size()) x = 0; x < sv.size(); x += 2) {
      QString col(sv[x].get<std::string>().c_str());
      QString value(sv[x + 1].get<std::string>().c_str());
      item.push_back(std::make_pair(col.remove("\""), value.remove("\"")));
    }
    json << item;
  };

  if (verbose) {
    peg.log = [&](auto ln, auto col, const auto &msg) {
      std::cerr << "\n"
                << fname << ":" << ln << ":" << col << ": " << msg << std::endl;
    };
  }
  std::vector<char> buffer;
  if (!read_file(fname, buffer)) {
    if (verbose) {
      std::cout << "\nerror load json " << fname << " galat tidak bisa dibuka";
    }
    return ParserDef::INPUTTEST_NOTFOUND;
  }
  if (!peg.parse_n(static_cast<const char *>(buffer.data()), buffer.size())) {
    if (verbose) {
      std::cout << "\nerror load json " << fname << " galat sintak error";
    }
    return ParserDef::PARSE_ERROR;
  }

  if (verbose) {
    std::cout << "\nok load json " << fname << "\n";
    for (auto &v : json) {
      for (auto &c : v) {
        std::cout << "\nkolom " << c.first.toStdString() << " data "
                  << c.second.toStdString();
      }
    }
  }
  return ParserDef::SUCCESS;
}

ParserDef::errcode ParserLint::parseselfcsv(const char *fname, bool verbose,
                                            bool useheader) {
  peg::parser peg;
  auto gramnoheader = R"(
              file        <- record (NL record)* NL?
              record      <- field (COMMA field)*
              name        <- field
              field       <- escaped / non_escaped
              escaped     <- DQUOTE (TEXTDATA / COMMA / CR / LF / D_DQUOTE)* DQUOTE
              non_escaped <- TEXTDATA*
              COMMA       <- ','
              ~CR          <- '\r'
              DQUOTE      <- '"'
             ~LF          <- '\n'
              ~NL          <- CR LF / CR / LF
              TEXTDATA    <- ![",] .
              D_DQUOTE    <- '"' '"'     
              )";
  auto gram = R"(
                      file        <- (header NL)? record (NL record)* NL?
                      header      <- name (COMMA name)*
                      record      <- field (COMMA field)*
                      name        <- field
                      field       <- escaped / non_escaped
                      escaped     <- DQUOTE (TEXTDATA / COMMA / CR / LF / D_DQUOTE)* DQUOTE
                      non_escaped <- TEXTDATA*
                      COMMA       <- ','
                      ~CR          <- '\r'
                      DQUOTE      <- '"'
                      ~LF          <- '\n'
                      ~NL          <- CR LF / CR / LF
                      TEXTDATA    <- ![",] .
                      D_DQUOTE    <- '"' '"'              
              )";
  if (useheader) {
    if (!peg.load_grammar(gramnoheader)) {
      if (verbose) {
        std::cout << "\ninternal error load csv grammar error..";
      }
      return ParserDef::GRAMMAR_ERROR;
    }
  } else {
      if (!peg.load_grammar(gram)) {
        if (verbose) {
          std::cout << "\ninternal error load csv grammar error..";
        }
        return ParserDef::GRAMMAR_ERROR;
      }
  }
  QList<QStringList> csvitem;

  peg["field"] = [](const peg::SemanticValues &sv) { return sv.str(); };
  peg["record"] = [&csvitem](const peg::SemanticValues &sv) {
    QStringList record;
    if(sv.str ().empty ()){
        return;
    }
    if(sv.size()==1){
        record<<QString::fromStdString (sv.str ());
    }
    auto lval=QString::fromStdString(sv[0].get<std::string>());
    record << lval.remove ("\"");
    for (decltype(sv.size()) x = 1; x < sv.size(); x +=2) {
      QString value(sv[x+1].get<std::string>().c_str());
      record <<value.remove("\"");
    }
    csvitem << record;
  };

  if (verbose) {
    peg.log = [&](auto ln, auto col, const auto &msg) {
      std::cerr << "\n"
                << fname << ":" << ln << ":" << col << ": " << msg << std::endl;
    };
  }
  std::vector<char> buffer;
  if (!read_file(fname, buffer)) {
    if (verbose) {
      std::cout << "\nerror load csv " << fname << " galat tidak bisa dibuka";
    }
    return ParserDef::INPUTTEST_NOTFOUND;
  }
  if (!peg.parse_n(static_cast<const char *>(buffer.data()), buffer.size())) {
    if (verbose) {
      std::cout << "\nerror load csv " << fname << " galat sintak error";
    }
    return ParserDef::PARSE_ERROR;
  }

  if (verbose) {
    std::cout << "\nok load csv " << fname << "\n";
    for (auto &v : csvitem) {
      for (auto &c : v) {
        std::cout << "\n data "
                  << c.toStdString();
      }
    }
  }
  return ParserDef::SUCCESS;
}
