#ifndef TABLEDATA_H
#define TABLEDATA_H

#include <QString>
#include <vector>
#include <memory>
#include <utility>

class HTMLTableCellData
{
public:
  QString getHTMLStr();

  static HTMLTableCellData* createNull();
  static HTMLTableCellData* createData(QString str);
  static HTMLTableCellData* createData(bool b);
  static HTMLTableCellData* createData(double f, QString lbl);

protected:
  HTMLTableCellData() {}

  virtual QString getDataHTMLStr() = 0;
};

class HTMLTableCellDataNull : public HTMLTableCellData
{
public:
  HTMLTableCellDataNull() {}

  virtual QString getDataHTMLStr() override { return "''"; }
};

class HTMLTableCellDataStr : public HTMLTableCellData
{
public:
  HTMLTableCellDataStr(QString str);

  virtual QString getDataHTMLStr() override;

private:
  QString m_Val;
};

class HTMLTableCellDataBool : public HTMLTableCellData
{
public:
  HTMLTableCellDataBool(bool b);

  virtual QString getDataHTMLStr() override;

private:
  bool m_Val = false;
};

class HTMLTableCellDataNumber : public HTMLTableCellData
{
public:
  HTMLTableCellDataNumber(double f, QString lbl);

  virtual QString getDataHTMLStr() override;

private:
  std::pair<double,QString> m_Val;
};

///////////////////////////////////////////////////////////////////////

class HTMLTableRowData
{
public:
  HTMLTableRowData() {}
  ~HTMLTableRowData() {}

  HTMLTableRowData& addColData(QString str, bool isNull = false);
  HTMLTableRowData& addColData(bool b, bool isNull = false);
  HTMLTableRowData& addColData(double f, bool isNull = false);
  HTMLTableRowData& addColData(double f, QString lbl, bool isNull = false);
  HTMLTableRowData& addNull();

  QString colHtmlStr(int colind) const;
  int colCount() const;

private:

  std::vector<std::shared_ptr<HTMLTableCellData>> m_ColData;
};

///////////////////////////////////////////////////////////////////////

class HTMLTableHeaderData
{
public:
  HTMLTableHeaderData() {}
  ~HTMLTableHeaderData() {}

  enum colDataType
  {
    ctString = 1,
    ctNumber = 2,
    ctBoolean = 3
  };

  HTMLTableHeaderData& addCol(QString name, colDataType type);

  QString colName(int colind) const;
  QString colHTMLType(int colind) const;
  int colCount() const;

private:
  std::vector<std::pair<QString,colDataType>> m_ColData;
};

///////////////////////////////////////////////////////////////////////

class HTMLTableData
{
public:
  HTMLTableData() {}
  ~HTMLTableData() {}

  void setHeader(const HTMLTableHeaderData &headerData);
  void addRow(const HTMLTableRowData &rowData);

  QString getColString(int colind) const;
  int colCount() const;

  QString getRowString(int rowind) const;
  int rowCount() const;

private:
  HTMLTableHeaderData m_Header;
  std::vector<HTMLTableRowData> m_RowsData;
};

#endif // TABLEDATA_H
