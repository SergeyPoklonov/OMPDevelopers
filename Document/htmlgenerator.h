#ifndef HTMLGENERATOR_H
#define HTMLGENERATOR_H

#include <QString>
#include <vector>


class HtmlGenerator
{
public:
  HtmlGenerator();
  
  void clear();
  
  HtmlGenerator& docBeg( const QString &title );
  HtmlGenerator& docEnd();
  
  HtmlGenerator& para( const QString &paraText );
  HtmlGenerator& paraBeg();
  HtmlGenerator& paraEnd();
  
  HtmlGenerator& header( const QString &headText, int headerSize );
  HtmlGenerator& headerBeg( int headerSize );
  HtmlGenerator& headerEnd( int headerSize );
  
  HtmlGenerator& text( const QString &txt );
  HtmlGenerator& textBold( const QString &txt );
  
  HtmlGenerator& tableBeg( const QString &caption, int borderWidth = 1 );
  HtmlGenerator& tableEnd();
  HtmlGenerator& tableHeader( const std::vector<QString> &headerList );
  HtmlGenerator& tableRow( const std::vector<QString> &colDataList );
  
  
private:
  QString m_DocText;
};

#endif // HTMLGENERATOR_H
