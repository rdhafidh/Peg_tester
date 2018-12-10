#include "jsonsocketparserworkaround.h"
#include <peglib.h>
#include <QDebug>

JsonSocketParserWorkaround::JsonSocketParserWorkaround()
{
    
}

QList<QByteArray> JsonSocketParserWorkaround::fixmultiLinejson(const QByteArray &rawdata)
{
    auto raw= R"(
              data <- json+
              json <- array (ESC? array)* ESC? / object ESC?
              object <- '{' ESC? pair  ESC? '}' /   '{' ESC?  '}'  
              pair <-  STRING ESC? ':' ESC? value ESC? ( ',' ESC? STRING ESC? ':' ESC? value)*  / STRING ESC? ':' ESC? value 
              array <- '[' ESC? value ESC? (',' ESC? value)* ESC? ']' / '[' ESC? ']'
              value <-   STRING /   NUMBER /   object  /   array   /   TRUE   /   FALSE /   NULL
              STRING <- PETIK ESC* TEXTDATA* ESC* PETIK 
              TEXTDATA    <- (!["] .)
              ~ESC <-   [ \n\r\t]+ 
              ~PETIK <- '"' 
              TRUE <- 'true'
              FALSE <- 'false'
              NULL <- 'null'
              NUMBER <-   '-'? INT '.' [0-9]+ EXP? # // 1.35, 1.35E-9, 0.3, -4.5
                  /   '-'? INT EXP             #  1e10 -3e4
                  /   '-'? INT                 # -3, 45
              INT <-   '0' / [1-9] [0-9]*  
              EXP <-   [Ee] ('+' / '-')? INT  
)";
      peg::parser parser;
      bool e=parser.load_grammar (raw);
      if(!e){
          qDebug()<<"warning load json replay grammar failed..";
      }
      QList<QByteArray> data;
      parser["json"]=[&data](const peg::SemanticValues &sv){
          data.push_back (QByteArray::fromStdString (sv.str ()));
      }; 
      parser.parse (rawdata.constData ());
      return data;
}
