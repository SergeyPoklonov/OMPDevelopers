#include "htmlgenerator.h"

HtmlGenerator::HtmlGenerator()
{
  
}

void HtmlGenerator::clear()
{
  m_DocText.clear();
}

HtmlGenerator& HtmlGenerator::docBeg( const QString &title )
{
  clear();
  
  m_DocText += "<!DOCTYPE HTML>\n";
  m_DocText += "<html>\n";
  m_DocText += "<head>\n";
  m_DocText += "<meta charset=""utf-8"">\n";
  m_DocText += QString("<title>%1</title>\n").arg( title );
  m_DocText += "</head>\n";
  m_DocText += "<body>\n";
  
  return *this;
}

HtmlGenerator& HtmlGenerator::docEnd()
{
  m_DocText += "</body>\n";
  m_DocText += "</html>\n";
  
  return *this;
}

HtmlGenerator& HtmlGenerator::para( const QString &paraText )
{
  m_DocText += QString("<p>%1</p>\n").arg( paraText );
  
  return *this;
}

HtmlGenerator& HtmlGenerator::paraBeg()
{
  m_DocText += QString("<p>");
  
  return *this;
}

HtmlGenerator& HtmlGenerator::paraEnd()
{
  m_DocText += QString("</p>\n");
  
  return *this;
}

HtmlGenerator& HtmlGenerator::header( const QString &headText, int headerSize )
{
  m_DocText += QString("<h%1>%2</h%1>\n").arg( headerSize ).arg( headText );
  
  return *this;
}

HtmlGenerator& HtmlGenerator::headerBeg( int headerSize )
{
  m_DocText += QString("<h%1>").arg( headerSize );
  
  return *this;
}

HtmlGenerator& HtmlGenerator::headerEnd( int headerSize )
{
  m_DocText += QString("</h%1>").arg( headerSize );
  
  return *this;
}

HtmlGenerator& HtmlGenerator::text( const QString &txt )
{
  m_DocText += txt;
  
  return *this;
}

HtmlGenerator& HtmlGenerator::textBold( const QString &txt )
{
  m_DocText += QString("<b>%1</b>").arg( txt );
  
  return *this;
}

HtmlGenerator& HtmlGenerator::tableBeg( const QString &caption, int borderWidth )
{
  m_DocText += QString("<table border=%1>\n").arg( borderWidth );
  m_DocText += QString("<caption>%1</caption>\n").arg( caption );
  
  return *this;
}

HtmlGenerator& HtmlGenerator::tableEnd()
{
  m_DocText += "</table>\n";
  
  return *this;
}

HtmlGenerator& HtmlGenerator::tableHeader( const std::vector<QString> &headerList )
{
  m_DocText += "<tr>";
  
  for(QString headerName : headerList)
  {
    m_DocText += QString("<th>%1</th>\n").arg( headerName );
  }

  m_DocText += "</tr>\n";
  
  return *this;
}

HtmlGenerator& HtmlGenerator::tableRow( const std::vector<QString> &colDataList )
{
  m_DocText += "<tr>";
  
  for(QString colData : colDataList)
  {
    m_DocText += QString("<td>%1</td>\n").arg( colData );
  }
  
  m_DocText += "</tr>\n";
  
  return *this;
}
