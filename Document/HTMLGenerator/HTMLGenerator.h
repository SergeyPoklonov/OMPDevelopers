#ifndef HTMLGENERATOR_H
#define HTMLGENERATOR_H

#include "piechartdata.h"
#include "tabledata.h"

#include <QObject>

#include <Document/developerworkdata.h>

class DocumentDataManager;

class HTMLGenerator
{
public:
  HTMLGenerator() {}
  
  QVector<QPair<QString, QString>> generateHTMLFilesTexts( const DocumentDataManager &docObj );
  
private:
  struct HorizontalBarData
  {
    QString name; // название линейки
    bool addNameTags; // добавлять ли тэги к названию
    QString inbarStr; // метка внутри линейки
    QString color; // цвет
    double barLenPrc; // заполнена на %
    
    HorizontalBarData() : barLenPrc(0), addNameTags(true) {}
  };
  
private:
  void addHorizontalBars( std::vector<HorizontalBarData> &barsData, QString inbarTextColor = "white", QString barBackColor = "silver" );

  void addTable( QString tableCaption, const HTMLTableData &tableData );
  void addRevisionsTable( QString tableCaption, const std::vector<CRevisionData> &revsList );
  void addOverdueIssuesTable( const std::vector<CRedmineIssueData> &issuesList );

  void addPieChart( QString chartCaption, const HTMLPieChartData &pieData );
  void addPlanNonPlanPieChart( QString chartCaption, std::vector< CDeveloperWorkData >&srcDevList );
  
  QString makeGitHTMLRevisionURL(const QString &sha);
  QString makeRedmineIssueURL(const CRedmineIssueData &issue);
  
  const DocumentDataManager& Doc() const;
  
  QString makeNameUnique(QString name);
  
  void addHTMLLine(QString lineText);
  
  static std::map<int,QString> getTrackersColors();

private:
  QString m_HTMLText;
  const DocumentDataManager *m_pDoc;
  unsigned m_UniqueSuffix;
};

#endif // HTMLGENERATOR_H
