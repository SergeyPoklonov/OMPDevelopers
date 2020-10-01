#include "HTMLGenerator.h"

#include <Document/documentdatamanager.h>

#include <QLocale>

QVector<QPair<QString, QString>> HTMLGenerator::generateHTMLFilesTexts( const DocumentDataManager &docObj)
{
    QVector<QPair<QString, QString>> result;
    m_UniqueSuffix = 1;
    m_pDoc = &docObj;

    const QDate dateFrom = Doc().devStatistic().getDateFrom();
    const QDate dateTo = Doc().devStatistic().getDateTo();

    m_HTMLText += "<!DOCTYPE HTML>\n";
    m_HTMLText += "<html>\n";
    m_HTMLText += "<head>\n";
    m_HTMLText += "<meta charset=""utf-8"">\n";
    m_HTMLText += QString("<title>Статистика по разработчикам %1-%2</title>\n").arg(dateFrom.toString("dd.MM.yyyy")).arg(dateTo.toString("dd.MM.yyyy"));
    m_HTMLText += "</head>\n";
    m_HTMLText += "<body>\n";
    m_HTMLText += QString("<h2>Период: %1 - %2</h2>\n").arg(dateFrom.toString("dd.MM.yyyy")).arg(dateTo.toString("dd.MM.yyyy"));

    const unsigned workDaysQty = Doc().devStatistic().getWorkingDaysQty();
    const unsigned workDayLen = Doc().devStatistic().getWorkDayLenHrs();

    const unsigned totalPeriodWorkLenHrs = workDaysQty * workDayLen;

    m_HTMLText += QString("<p>Всего рабочего времени: %1дн <b>(%2ч)</b></p>\n").arg(workDaysQty).arg( totalPeriodWorkLenHrs );

    std::vector< CDeveloperWorkData > devStatList = Doc().devStatistic().getDevelopersStatisticData();

    std::sort(devStatList.begin(), devStatList.end(), [](const CDeveloperWorkData &f, const CDeveloperWorkData &s)
    {
      return f.getName() < s.getName();
    });

    addHTMLLine("<h3>Трудоемкость за период</h3>");

    // линейки трудоемкости по разработчикам
    if( Doc().devStatistic().isRedmineEnabled() )
    {
      std::vector<HorizontalBarData> barsData;
      for( const CDeveloperWorkData &devData : devStatList )
      {
        HorizontalBarData bar;
                          bar.name = QString( "<a href=""%1.html"">%1</a>" ).arg(devData.getName());
                          bar.addNameTags = false;
                          bar.barLenPrc = devData.totalLabourHrs() / (totalPeriodWorkLenHrs / 100.0);
                          bar.inbarStr = QString("%1ч").arg( QLocale().toString( devData.totalLabourHrs(), 'f', 1) );

        if( bar.barLenPrc < 33.3 )
          bar.color = "red";
        else if( bar.barLenPrc < 66.6 )
          bar.color = "gold";
        else
          bar.color = "SpringGreen";

        barsData.push_back( bar );
      }

      addHorizontalBars( barsData, "black" );
    }

    // график по трудоемкости план/не план
    addPlanNonPlanPieChart( "Использование рабочего времени по плану", devStatList );

    m_HTMLText += "</body>\n";
    m_HTMLText += "</html>\n";

    result.push_back({"main.html", m_HTMLText});

    for( const CDeveloperWorkData &devData : devStatList )
    {
      m_HTMLText.clear();
      m_HTMLText += "<!DOCTYPE HTML>\n";
      m_HTMLText += "<html>\n";
      m_HTMLText += "<head>\n";
      m_HTMLText += "<meta charset=""utf-8"">\n";
      m_HTMLText += QString("<title>Статистика по разработчику %1</title>\n").arg(devData.getName());
      m_HTMLText += "</head>\n";
      m_HTMLText += "<body>\n";

      addHTMLLine( QString("<h3 id=""%1"">%1</h3>").arg( devData.getName() ) );
      addHTMLLine( QString("<a href=""main.html""><h2>На главную</h2></a>") );

      if( Doc().devStatistic().isRedmineEnabled() )
      {
        const double redmineTotalHrs = devData.redmineTotalHrs();
        const double developmentOtherHrs = devData.developOtherHrs();
        const double holydaysHrs = devData.calendar().getExceptDaysQty(dateFrom, dateTo, ExceptDayType::HOLIDAYS, DaysExcludeMode::STDNONWORK) * Doc().devStatistic().getWorkDayLenHrs();
        const double sickHrs = devData.calendar().getExceptDaysQty(dateFrom, dateTo, ExceptDayType::SICKLIST, DaysExcludeMode::STDNONWORK) * Doc().devStatistic().getWorkDayLenHrs();
        const double freeTimeHrs = totalPeriodWorkLenHrs - (redmineTotalHrs + developmentOtherHrs + holydaysHrs + sickHrs);

        if( freeTimeHrs == totalPeriodWorkLenHrs )
        {
          m_HTMLText += "<p>Отсутствуют какие-либо данные по разработчику за указанный период времени.</p>\n";
          continue;
        }

        m_HTMLText += QString("<p>Общие трудозатраты: <b>%1ч</b>.</p>\n").arg( QLocale().toString( devData.totalLabourHrs(), 'f', 1) );

        // график по типам времени
        {
          HTMLPieChartData pieChartData;

          pieChartData.setDimensions(640,480);
          pieChartData.setResidueSliceParams("Прочее", "Yellow");
          pieChartData.setSliceVisibilityThresholdPrc(1);

          if( redmineTotalHrs > 0 )
            pieChartData.addSlice("Трудоемкость по Redmine", redmineTotalHrs, "Green");

          if( developmentOtherHrs > 0 )
            pieChartData.addSlice("Разработка без привязки к Redmine", developmentOtherHrs, "Lime");

          if( holydaysHrs > 0 )
            pieChartData.addSlice( getExceptDayTypeName(ExceptDayType::HOLIDAYS) , holydaysHrs, "Blue");

          if( freeTimeHrs > 0 )
            pieChartData.addSlice("Неучтенное время", freeTimeHrs, "Silver");

          if( sickHrs > 0 )
            pieChartData.addSlice( getExceptDayTypeName(ExceptDayType::SICKLIST) , sickHrs, "Fuchsia");

          addPieChart( QString("%1: Использование рабочего времени").arg(devData.getName()), pieChartData );
        }

        // график по типам задач в редмайн
        if( redmineTotalHrs )
        {
          std::map<int,double> trackersTime;
          std::vector<CRedmineTimeData> devTEList = devData.redmineTimesList();

          for( CRedmineTimeData &teData : devTEList )
          {
            const int trackerID = Doc().devStatistic().issueToTracker(teData.IssueID());
            if( teData.HoursSpent() > 0 )
              trackersTime[trackerID] += teData.HoursSpent();
          }

          Q_ASSERT( trackersTime.size() );

          std::map<int,QString> trackerColors = getTrackersColors();

          HTMLPieChartData pieChartData;

          pieChartData.setDimensions(640,480);
          pieChartData.setResidueSliceParams("Прочее");
          pieChartData.setSliceVisibilityThresholdPrc(1);

          for(auto &trackerPair : trackersTime)
          {
            const double trackerHrs = trackerPair.second;
            const QString trackerName = Doc().devStatistic().trackerName(trackerPair.first);

            pieChartData.addSlice(trackerName, trackerHrs, trackerColors[trackerPair.first]);
          }

          addPieChart( QString("%1: Трудозатраты по типам задач").arg(devData.getName()), pieChartData );
        }

        // график по трудоемкости план/не план
        std::vector< CDeveloperWorkData > devVec{devData};
        addPlanNonPlanPieChart( "Использование рабочего времени по плану", devVec );

        // список просроченных задач
        std::map<long,CRedmineIssueData> issues = Doc().devStatistic().getIssues();
        std::vector<CRedmineIssueData> overdueIssues;
        std::map<long, double> hrsSpentForIssues;
        devData.getIssuesWithHrsSpent( issues, overdueIssues, hrsSpentForIssues);
        {
          addIssuesTable( overdueIssues, hrsSpentForIssues );
        }

        std::vector<CRevisionData> nonRMRevisions = devData.nonRedmineRevisionsList(false);

        if( nonRMRevisions.size() )
        {
          addRevisionsTable( QString("Ревизии без привязки к Redmine"), nonRMRevisions );

          m_HTMLText += "<br>";
        }
      }

      if( Doc().devStatistic().isLargeRevisionListEnabled() )
      {
        const double largeRevHrsMin = Doc().devStatistic().getMinRevHrs();

        std::vector<CRevisionData> largeRevisions = devData.revisionsList( largeRevHrsMin );

        if( largeRevisions.size() )
        {
          addRevisionsTable( QString("Большие ревизии (не менее %1 ч)\n").arg( largeRevHrsMin ), largeRevisions );
        }
        else
        {
          m_HTMLText += QString("<p>Ревизии %1 ч и больше отсутствуют.</p>\n").arg( largeRevHrsMin );
        }
      }

      if( Doc().devStatistic().isCoreRevisionListEnabled() )
      {
        std::vector<CRevisionData> coreRevisions = devData.coreRevisionsList();

        if( coreRevisions.size() )
        {
          addRevisionsTable( "Ревизии меняющие базовый функционал\n", coreRevisions );
        }
        else
        {
          addHTMLLine( "<p>Ревизии меняющие базовый функционал отсутствуют.</p>" );
        }
      }

      m_HTMLText += QString("<p>Всего ревизий: %1</p>\n").arg( devData.revisionsCount() );

      m_HTMLText += "</body>\n";
      m_HTMLText += "</html>\n";

      result.push_back({QString("%1.html").arg(devData.getName()), m_HTMLText});
    }

    return result;
}

#define T_ERROR       1
#define T_DEVELOP     2
#define T_SUPPDEVELOP 3
#define T_PATCH       4
#define T_RELEASE     5
#define T_TALK        6
#define T_DOCS        7
#define T_SUPPERROR   8
#define T_INTERNAL    9
#define T_LAST        T_INTERNAL

std::map<int,QString> HTMLGenerator::getTrackersColors()
{
  std::map<int,QString> tarckersColors;
  
  tarckersColors[T_ERROR] = "red";
  tarckersColors[T_DEVELOP] = "green";
  tarckersColors[T_SUPPDEVELOP] = "lime";
  tarckersColors[T_PATCH] = "gold";
  tarckersColors[T_RELEASE] = "aqua";
  tarckersColors[T_TALK] = "teal";
  tarckersColors[T_DOCS] = "navy";
  tarckersColors[T_SUPPERROR] = "maroon";
  tarckersColors[T_INTERNAL] = "blue";
  
  int trackID = T_LAST;
  
  tarckersColors[++trackID] = "pink";
  tarckersColors[++trackID] = "Olive";
  tarckersColors[++trackID] = "Thistle";
  tarckersColors[++trackID] = "IndianRed";
  tarckersColors[++trackID] = "MediumSpringGreen";
  tarckersColors[++trackID] = "MediumPurple";
  tarckersColors[++trackID] = "Coral";
  tarckersColors[++trackID] = "DarkKhaki";
  tarckersColors[++trackID] = "CadetBlue";
  tarckersColors[++trackID] = "RosyBrown";
  
  return tarckersColors;
}

void HTMLGenerator::addHorizontalBars( std::vector<HorizontalBarData> &barsData, QString inbarTextColor, QString barBackColor )
{
  if( barsData.empty() )
    return;
  
  const QString className = makeNameUnique("horBars");
  const QString barContainerName = QString("%1container").arg(className);
  
  m_HTMLText += "<style>\n";
  m_HTMLText += "* {box-sizing: border-box}\n";
  addHTMLLine( QString(".%1 {").arg( barContainerName ) );
  m_HTMLText += "width: 100%;\n";
  m_HTMLText += QString("background-color: %1;\n").arg(barBackColor);
  m_HTMLText += "}\n";
  
  auto makeBarName = [className](int barInd)->QString
  {
    return QString("%1bar%2").arg(className).arg(barInd);
  };
  
  for(int i = 0; i < (int)barsData.size(); i++)
  {
    const HorizontalBarData &barData = barsData[i];
    
    double barLenPrc = barData.barLenPrc;
    if( barLenPrc < 0 )
      barLenPrc = 0;
    else if( barLenPrc > 100 )
      barLenPrc = 100;
    
    addHTMLLine( QString(".%1{").arg( makeBarName(i) ) );
    addHTMLLine( "text-align: right;" );
    addHTMLLine( "padding-right: 20px;" );
    addHTMLLine( "line-height: 20px;" );
    addHTMLLine( QString("color: %1;").arg(inbarTextColor) );
    addHTMLLine( QString("width: %1%;").arg( QString::number( barLenPrc, 'f', 1) ) );
    addHTMLLine( QString("background-color: %1").arg(barData.color) );
    addHTMLLine( "}" );
  }
  
  addHTMLLine("</style>");
  
  for(int i = 0; i < (int)barsData.size(); i++)
  {
    const HorizontalBarData &barData = barsData[i];
    
    addHTMLLine("<hr>");
    
    if( barData.addNameTags )
      addHTMLLine( QString("<p>%1</p>").arg(barData.name) );
    else
      addHTMLLine( barData.name );
    
    addHTMLLine( QString("<div class=""%1"">").arg(barContainerName) );
    addHTMLLine( QString("<div class=""%1"">%2</div>").arg(makeBarName(i)).arg(barData.inbarStr) );
    addHTMLLine( "</div>" );
  }
  
  addHTMLLine("<hr>");
}

void HTMLGenerator::addHTMLLine(QString lineText)
{
  if( !lineText.size() )
    return;
  
  m_HTMLText += QString("%1\n").arg(lineText);
}

void HTMLGenerator::addTable( QString tableCaption, const HTMLTableData &tableData )
{
  if( !tableData.colCount() )
    return;

  const QString tableHTMLName = makeNameUnique("table");
  const QString drawFunctionName = makeNameUnique("drawTable");

  addHTMLLine( QString("<p><b>%1</b></p>").arg(tableCaption) );

  addHTMLLine( QString("<div id=""%1""></div>").arg( tableHTMLName ) );
  addHTMLLine( "<script type=""text/javascript"" src=""https://www.gstatic.com/charts/loader.js""></script>" );
  addHTMLLine( "<script type=""text/javascript"">" );
  addHTMLLine( "google.charts.load('current', {'packages':['table']});" );
  addHTMLLine( QString("google.charts.setOnLoadCallback(%1);").arg( drawFunctionName ) );
  addHTMLLine( QString("function %1() {").arg(drawFunctionName) );
  addHTMLLine( "var data = new google.visualization.DataTable();" );

  for(int i = 0; i < tableData.colCount(); i++)
  {
    addHTMLLine( QString("data.addColumn(%1);").arg( tableData.getColString(i) ) );
  }

  if( tableData.rowCount() )
  {
    addHTMLLine("data.addRows([");

    for(int i = 0; i < tableData.rowCount(); i++)
    {
      if( i )
        m_HTMLText += ",";

      addHTMLLine( QString("[%1]").arg( tableData.getRowString(i) ) );
    }

    addHTMLLine("]);");
  }

  addHTMLLine( QString("var table = new google.visualization.Table(document.getElementById('%1'));").arg(tableHTMLName) );
  addHTMLLine( "table.draw(data, {showRowNumber: true, allowHtml: true});" );
  addHTMLLine( "}" );
  addHTMLLine( "</script>" );
}

void HTMLGenerator::addPieChart( QString chartCaption, const HTMLPieChartData &pieData )
{
  std::vector<HTMLPieChartSlice> slices = pieData.getSlices();
  
  if( slices.empty() )
    return;
  
  const QString chartHTMLName = makeNameUnique("piechart");
  const QString drawFunctionName = makeNameUnique("drawChart");
  
  m_HTMLText += QString("<div id=""%1""></div>\n").arg( chartHTMLName );
  
  m_HTMLText += "<script type=""text/javascript"" src=""https://www.gstatic.com/charts/loader.js""></script>\n";
  m_HTMLText += "<script type=""text/javascript"">\n";
  m_HTMLText += "google.charts.load('current', {'packages':['corechart']}); \n";
  m_HTMLText += QString("google.charts.setOnLoadCallback(%1); \n").arg( drawFunctionName );
  
  m_HTMLText += QString("function %1() { \n").arg( drawFunctionName );
  m_HTMLText += "var data = google.visualization.arrayToDataTable([ \n";
  
  m_HTMLText += QString("['%1', '%2'] \n").arg(pieData.getSliceTypeName()).arg(pieData.getSliceMeasName());
  
  for( HTMLPieChartSlice &slice : slices )
  {
    m_HTMLText += QString(",['%1', %2] \n").arg(slice.caption).arg(slice.weight);
  }
  
  m_HTMLText += "]); \n";
  
  m_HTMLText += "var options = {\n";
  m_HTMLText += QString("'title':'%1', \n").arg( chartCaption );
  m_HTMLText += QString("'width':%1, \n").arg( pieData.getWidth() );
  m_HTMLText += QString("'height':%1 \n").arg( pieData.getHeight() );
  
  if( pieData.getResidueSliceColor().size() )
    m_HTMLText += QString(", 'pieResidueSliceColor':'%1' \n").arg( pieData.getResidueSliceColor() );
  
  if( pieData.getResidueSliceLabel().size() )
    m_HTMLText += QString(", 'pieResidueSliceLabel':'%1' \n").arg( pieData.getResidueSliceLabel() );
  
  if( pieData.getSliceVisibilityThreshold() )
    m_HTMLText += QString(", 'sliceVisibilityThreshold':%1 \n").arg( pieData.getSliceVisibilityThreshold() );
  
  if( pieData.slicesAreAdvanced() )
  {
    m_HTMLText += ", \n slices: { \n";
    
    for(int i = 0; i < (int)slices.size(); i++)
    {
      if( i )
        m_HTMLText += ",";
      
      m_HTMLText += QString("%1: { color: '%2'} \n").arg(i).arg(slices[i].colorString);
    }
    
    m_HTMLText += "}";
  }
  
  m_HTMLText += "}; \n";
  
  m_HTMLText += QString("var chart = new google.visualization.PieChart(document.getElementById('%1')); \n").arg(chartHTMLName);
  m_HTMLText += "chart.draw(data, options); \n";
  
  m_HTMLText += "} \n";
  m_HTMLText += "</script> \n";
}

void HTMLGenerator::addIssuesTable( const std::vector<CRedmineIssueData> &issuesList, const std::map<long, double> &hrsSpentForIssues )
{
  if( issuesList.size() )
  {
    HTMLTableData tableData;

    tableData.setHeader( HTMLTableHeaderData()
                         .addCol("Задача",HTMLTableHeaderData::ctString)
                         .addCol("Deadline",HTMLTableHeaderData::ctString)
                         .addCol("Дата закрытия",HTMLTableHeaderData::ctString)
                         .addCol("Затрачено,ч", HTMLTableHeaderData::ctNumber)
                         .addCol("Включена в план",HTMLTableHeaderData::ctBoolean)
                         );

    std::vector<CRedmineIssueData> sortedIssuesList( issuesList );

    std::sort(sortedIssuesList.begin(), sortedIssuesList.end(), [](const CRedmineIssueData &f, const CRedmineIssueData &s)
    {
      if( f.DeadLine() != s.DeadLine() )
        return f.DeadLine() < s.DeadLine();

      return f.CloseDate() < s.CloseDate();
    });

    for( const CRedmineIssueData &issueData : sortedIssuesList )
    {
      auto spentIt = hrsSpentForIssues.find( issueData.ID() );
      const double spentHrs = spentIt != hrsSpentForIssues.end() ? spentIt->second : 0.0;

      tableData.addRow( HTMLTableRowData()
                        .addColData( makeRedmineIssueURL(issueData) )
                        .addColData( issueData.DeadLine().toString("yyyy-MM-dd") )
                        .addColData( issueData.CloseDate().isValid() ? issueData.CloseDate().toString("yyyy-MM-dd") : "-" )
                        .addColData( spentHrs, spentHrs == 0.0 )
                        .addColData( issueData.IsInPlan() )
                        );
    }

    addTable( "Задачи в работе", tableData );
  }
}

void HTMLGenerator::addRevisionsTable( QString tableCaption, const std::vector<CRevisionData> &revsList )
{
  if( revsList.size() )
  {
    HTMLTableData tableData;

    tableData.setHeader( HTMLTableHeaderData()
                         .addCol("Трудоемкость",HTMLTableHeaderData::ctNumber)
                         .addCol("Ссылка",HTMLTableHeaderData::ctString)
                         .addCol("По Redmine",HTMLTableHeaderData::ctBoolean)
                         );

    std::vector<CRevisionData> sortedRevsList( revsList );

    std::sort(sortedRevsList.begin(), sortedRevsList.end(), [](const CRevisionData &f, const CRevisionData &s)
    {
      return f.HoursSpent() < s.HoursSpent();
    });

    for( const CRevisionData &revData : sortedRevsList )
    {
      tableData.addRow( HTMLTableRowData()
                        .addColData( revData.HoursSpent() )
                        .addColData( makeGitHTMLRevisionURL( revData.SHA() ) )
                        .addColData( revData.RedmineLinked() )
                        );
    }

    addTable( tableCaption, tableData );
  }
}

QString HTMLGenerator::makeNameUnique(QString name)
{
  return QString("%1%2").arg(name).arg(m_UniqueSuffix++);
}

const DocumentDataManager& HTMLGenerator::Doc() const 
{ 
  Q_ASSERT( m_pDoc );
  return *m_pDoc; 
}

QString HTMLGenerator::makeRedmineIssueURL(const CRedmineIssueData &issue)
{
  // issues/47395
  QString redmineURL = Doc().generalSettings().getRedmineURL();

  QString url = QString("%1/issues/%2").arg(redmineURL).arg(issue.ID());

  QString htmlURLStr = QString("<a href=""%1"">#%2 %3</a>").arg( url ).arg( issue.ID() ).arg( issue.Name() );

  return htmlURLStr;
}

QString HTMLGenerator::makeGitHTMLRevisionURL(const QString &sha)
{
  QString gitUrlPrefix = Doc().generalSettings().getGitWeb();
  
  while( gitUrlPrefix.size() && gitUrlPrefix.endsWith('/') )
    gitUrlPrefix.chop(1);
  
  QString htmlURLStr;
  
  if( gitUrlPrefix.size() )
  {
    QString fullUrlStr = QString("%1/commit/?r=omp.git&h=%2").arg( gitUrlPrefix ).arg( sha );
    
    htmlURLStr = QString("<a href=""%1"">%2</a>").arg( fullUrlStr ).arg( sha );
  }
  else
  {
    htmlURLStr = sha;
  }
  
  return htmlURLStr;
}

void HTMLGenerator::addPlanNonPlanPieChart( QString chartCaption, std::vector< CDeveloperWorkData >&srcDevList )
{
  HTMLPieChartData pieChartData;

  pieChartData.setDimensions(640,480);
  pieChartData.setSliceVisibilityThresholdPrc(1);

  std::map<long,CRedmineIssueData> issues = Doc().devStatistic().getIssues();

  double spentOnPlan = 0.0;
  double spentNonPlan = 0.0;
  for( const CDeveloperWorkData &devData : srcDevList )
  {
    spentOnPlan += devData.spentOnPlanHrs( issues );
    spentNonPlan += devData.spentNonPlanHrs( issues );
  }

  if( spentOnPlan > 0 )
    pieChartData.addSlice("По плану", spentOnPlan, "Green");

  if( spentNonPlan > 0 )
    pieChartData.addSlice("Вне плана", spentNonPlan, "Red");

  addPieChart( chartCaption, pieChartData );
}
