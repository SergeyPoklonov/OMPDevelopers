#include "tabledata.h"
#include "apputils.h"

QString HTMLTableCellData::getHTMLStr()
{
  return getDataHTMLStr();
}

HTMLTableCellData* HTMLTableCellData::createNull()
{
  return new HTMLTableCellDataNull;
}

HTMLTableCellData* HTMLTableCellData::createData(QString str)
{
  if( !str.length() )
    return createNull();

  return new HTMLTableCellDataStr(str);
}

HTMLTableCellData* HTMLTableCellData::createData(bool b)
{
  return new HTMLTableCellDataBool(b);
}

HTMLTableCellData* HTMLTableCellData::createData(double f, QString lbl)
{
  return new HTMLTableCellDataNumber(f,lbl);
}


HTMLTableCellDataStr::HTMLTableCellDataStr(QString str)
{
  m_Val = str;
}

QString HTMLTableCellDataStr::getDataHTMLStr()
{
  return QString("'%1'").arg(m_Val);
}

HTMLTableCellDataBool::HTMLTableCellDataBool(bool b)
{
  m_Val = b;
}

QString HTMLTableCellDataBool::getDataHTMLStr()
{
  return m_Val ? "true" : "false";
}

HTMLTableCellDataNumber::HTMLTableCellDataNumber(double f, QString lbl)
{
  m_Val = std::make_pair(f,lbl);
}

QString HTMLTableCellDataNumber::getDataHTMLStr()
{
  return QString("{v: %1, f: '%2'}").arg(m_Val.first).arg(m_Val.second);
}

HTMLTableRowData& HTMLTableRowData::addColData(QString str, bool isNull)
{
  if( isNull )
    return addNull();

  m_ColData.push_back( std::shared_ptr<HTMLTableCellData>(HTMLTableCellData::createData(str)) );
  return *this;
}

HTMLTableRowData& HTMLTableRowData::addColData(bool b, bool isNull)
{
  if( isNull )
    return addNull();

  m_ColData.push_back( std::shared_ptr<HTMLTableCellData>(HTMLTableCellData::createData(b)) );
  return *this;
}

HTMLTableRowData& HTMLTableRowData::addColData(double f, bool isNull)
{
  return addColData(f,QString::number(f),isNull);
}

HTMLTableRowData& HTMLTableRowData::addColData(double f, QString lbl, bool isNull)
{
  if( isNull )
  {
    m_ColData.push_back( std::shared_ptr<HTMLTableCellData>(HTMLTableCellData::createData(0.0," ")) );
  }
  else
  {
    m_ColData.push_back( std::shared_ptr<HTMLTableCellData>(HTMLTableCellData::createData(f,lbl)) );
  }
  return *this;
}

HTMLTableRowData& HTMLTableRowData::addNull()
{
  m_ColData.push_back( std::shared_ptr<HTMLTableCellData>(HTMLTableCellData::createNull()) );
  return *this;
}

QString HTMLTableRowData::colHtmlStr(int colind) const
{
  if( !utils::is_valid_index(m_ColData.size(), colind) )
    return "";

  return m_ColData[colind]->getHTMLStr();
}

int HTMLTableRowData::colCount() const
{
  return (int)m_ColData.size();
}

HTMLTableHeaderData& HTMLTableHeaderData::addCol(QString name, colDataType type)
{
  m_ColData.push_back( std::make_pair(name, type) );
  return *this;
}

QString HTMLTableHeaderData::colName(int colind) const
{
  if( !utils::is_valid_index(m_ColData.size(), colind) )
    return "";

  return QString("'%1'").arg(m_ColData[colind].first);
}

QString HTMLTableHeaderData::colHTMLType(int colind) const
{
  if( !utils::is_valid_index(m_ColData.size(), colind) )
    return "";

  QString typeName;

  switch (m_ColData[colind].second)
  {
    case ctString: typeName = "string"; break;
    case ctNumber: typeName = "number"; break;
    case ctBoolean: typeName = "boolean"; break;
    default: return "";
  }

  return QString("'%1'").arg(typeName);
}

int HTMLTableHeaderData::colCount() const
{
  return (int)m_ColData.size();
}


void HTMLTableData::setHeader(const HTMLTableHeaderData &headerData)
{
  m_Header = headerData;
}

void HTMLTableData::addRow(const HTMLTableRowData &rowData)
{
  m_RowsData.push_back( rowData );
}

QString HTMLTableData::getColString(int colind) const
{
  if( !utils::is_valid_index(colCount(), colind) )
    return "";

  return QString("%1,%2").arg(m_Header.colHTMLType(colind)).arg(m_Header.colName(colind));
}

int HTMLTableData::colCount() const
{
  return m_Header.colCount();
}

QString HTMLTableData::getRowString(int rowind) const
{
  if( !utils::is_valid_index(rowCount(), rowind) )
    return "";

  const HTMLTableRowData& rowData = m_RowsData[rowind];

  QString htmlStr;

  for(int i = 0; i < rowData.colCount(); i++)
  {
    if( i )
      htmlStr += ", ";

    htmlStr += rowData.colHtmlStr(i);
  }

  return htmlStr;
}

int HTMLTableData::rowCount() const
{
  return (int)m_RowsData.size();
}
