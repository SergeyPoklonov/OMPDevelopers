#include "developerworkdata.h"
#include <numeric>

CRedmineIssueData::CRedmineIssueData()
{
    clear();
}

void CRedmineIssueData::clear()
{
    m_ID = 0;
    m_State = 0;
    m_IsInPlan = false;
    m_Tracker = 0;
    m_PerformerID = 0;
    m_DeadLine = QDate();
    m_CloseDate = QDate();
    m_Name.clear();
    m_AssignedTo.clear();
}

long CRedmineIssueData::ID() const
{
    return m_ID;
}

void CRedmineIssueData::setID(long id)
{
    m_ID = id;
}

long CRedmineIssueData::PerformerID() const
{
    return m_PerformerID;
}

void CRedmineIssueData::setPerformerID(long id)
{
    m_PerformerID = id;
}

long CRedmineIssueData::State() const
{
    return m_State;
}

void CRedmineIssueData::setState(long state)
{
    m_State = state;
}

long CRedmineIssueData::Tracker() const
{
    return m_Tracker;
}

void CRedmineIssueData::setTracker(long tracker)
{
    m_Tracker = tracker;
}

bool CRedmineIssueData::IsInPlan() const
{
    return m_IsInPlan;
}

void CRedmineIssueData::setIsInPlan(bool b)
{
    m_IsInPlan = b;
}

QDate CRedmineIssueData::DeadLine() const
{
    return m_DeadLine;
}

void CRedmineIssueData::setDeadLine(QDate d)
{
    m_DeadLine = d;
}

std::set<long> CRedmineIssueData::openStates()
{
    return { 1,2,7,13 };
}

bool CRedmineIssueData::isClosed() const
{
    std::set<long> states = openStates();
    return states.find( State() ) == states.end();
}

QDate CRedmineIssueData::CloseDate() const
{
    return m_CloseDate;
}

void CRedmineIssueData::setCloseDate(QDate d)
{
    m_CloseDate = d;
}

QString CRedmineIssueData::Name() const
{
  return m_Name;
}

void CRedmineIssueData::setName(QString str)
{
  m_Name = str;
}

QString CRedmineIssueData::PerformerName() const
{
  return m_AssignedTo;
}

void CRedmineIssueData::setPerformerName(QString str)
{
  m_AssignedTo = str;
}

bool CRedmineIssueData::isOverdued() const
{
  if( !DeadLine().isValid() )
    return false;

  QDate deadlineCheckDate = isClosed() && CloseDate().isValid() ? CloseDate() : QDate::currentDate();

  return DeadLine() < deadlineCheckDate;
}

////////////////////////////////////////////////////////////////////////////////////////

CDeveloperWorkData::CDeveloperWorkData()
{
  clear();
}

CDeveloperWorkData::CDeveloperWorkData(CDeveloperData devData)
{
  clear();
  setDeveloperData( devData );
}

void CDeveloperWorkData::clear()
{
  m_DevData.clear();
  m_RevisionsList.clear();
  m_RedmineTimeList.clear();
}

QString CDeveloperWorkData::getName() const
{
  return m_DevData.getName();
}

double CDeveloperWorkData::getWageRate() const
{
  return m_DevData.getWageRate();
}

void CDeveloperWorkData::getIssuesWithHrsSpent( const std::map<long,CRedmineIssueData> &issuesData, std::vector<CRedmineIssueData> &issuesList, std::map<long, double> &hrsSpentForIssues) const
{
  // добавляем все задачи на которые списывалась трудоемкость
  issuesList.clear();
  hrsSpentForIssues.clear();

  std::set<long> addedIssues;

  for( const CRedmineTimeData &te : m_RedmineTimeList )
  {
    const CRedmineIssueData &issue = issuesData.find( te.IssueID() )->second;

    hrsSpentForIssues[te.IssueID()] += te.HoursSpent();

    if( addedIssues.find(issue.ID()) != addedIssues.end() )
      continue;

    issuesList.push_back(issue);
    addedIssues.insert( issue.ID() );
  }

  // добавляем те, на которые списания не было, но они просрочены
  for(auto &issuePair : issuesData)
  {
    const CRedmineIssueData &issue = issuePair.second;

    if( addedIssues.find(issue.ID()) != addedIssues.end() )
      continue;

    if( issue.PerformerName() == m_DevData.getName() && issue.isOverdued() )
    {
      issuesList.push_back(issue);
      addedIssues.insert( issue.ID() );
    }
  }
}

double CDeveloperWorkData::spentOnPlanHrs(std::map<long,CRedmineIssueData> &issuesData) const
{
  return std::accumulate(m_RedmineTimeList.begin(), m_RedmineTimeList.end(), 0.0, [&issuesData](double val, const CRedmineTimeData &te)->double
  {
    std::vector<long> tasks{ te.IssueID() };
    for(long tID : tasks)
    {
      if( tID > 0 && issuesData[tID].IsInPlan() )
      {
        val += te.HoursSpent();
        break;
      }
    }

    return val;
  });
}

double CDeveloperWorkData::spentNonPlanHrs(std::map<long,CRedmineIssueData> &issuesData) const
{
  return std::accumulate(m_RedmineTimeList.begin(), m_RedmineTimeList.end(), 0.0, [&issuesData](double val, const CRedmineTimeData &te)->double
  {
    std::vector<long> tasks{ te.IssueID() };
    for(long tID : tasks)
    {
      if( tID > 0 && !issuesData[tID].IsInPlan() )
      {
        val += te.HoursSpent();
        break;
      }
    }

    return val;
  });
}

double CDeveloperWorkData::developOtherHrs() const
{
  return std::accumulate(m_RevisionsList.begin(), m_RevisionsList.end(), 0.0, [](double val, const CRevisionData &rev)->double
  {
    if( !rev.RedmineLinked() )
      val += rev.HoursSpent();
    
    return val;
  });
}

double CDeveloperWorkData::redmineDevelopHrs() const
{
  return std::accumulate(m_RedmineTimeList.begin(), m_RedmineTimeList.end(), 0.0, [](double val, const CRedmineTimeData &td)->double
  {
    if( td.IsDevelopment() )
      val += td.HoursSpent();
    
    return val;
  });
}

double CDeveloperWorkData::redmineTotalHrs() const
{
  return std::accumulate(m_RedmineTimeList.begin(), m_RedmineTimeList.end(), 0.0, [](double val, const CRedmineTimeData &td)->double
  {
    return val + td.HoursSpent();
  });
}

void CDeveloperWorkData::setDeveloperData(CDeveloperData devData)
{
  m_DevData = devData;
}

std::vector<CRevisionData> CDeveloperWorkData::coreRevisionsList() const
{
  std::vector<CRevisionData> retList;
  for( const CRevisionData &rev : m_RevisionsList )
  {
    if( rev.isChangeCore() )
      retList.push_back( rev );
  }
  
  return retList;
}

std::vector<CRevisionData> CDeveloperWorkData::nonRedmineRevisionsList(bool includeZeroTime) const
{
  std::vector<CRevisionData> retList;
  for( const CRevisionData &rev : m_RevisionsList )
  {
    if( !rev.RedmineLinked() && (includeZeroTime || rev.HoursSpent()> 0.0) )
      retList.push_back( rev );
  }
  
  return retList;
}

std::vector<CRevisionData> CDeveloperWorkData::revisionsList( double minTime ) const
{
  std::vector<CRevisionData> retList;
  for( const CRevisionData &rev : m_RevisionsList )
  {
    if( rev.HoursSpent() >= minTime )
      retList.push_back( rev );
  }
  
  return retList;
}

std::vector<CRevisionData> CDeveloperWorkData::revisionsList() const
{
  return m_RevisionsList;
}

size_t CDeveloperWorkData::revisionsCount() const
{
  return m_RevisionsList.size();
}

void CDeveloperWorkData::addRevision(const CRevisionData &revData)
{
  m_RevisionsList.push_back( revData );
}

std::vector<CRedmineTimeData> CDeveloperWorkData::redmineTimesList() const
{
  return m_RedmineTimeList;
}

size_t CDeveloperWorkData::redmineTimesCount() const
{
  return m_RedmineTimeList.size();
}

void CDeveloperWorkData::addRedmineTime(const CRedmineTimeData &timeData)
{
  m_RedmineTimeList.push_back( timeData ); 
}

///////////////////////////////////////////////////////////

void CRevisionData::clear()
{
  m_SHA.clear();
  m_DeveloperName.clear();
  m_HoursSpent = 0.0;
  m_RedmineTasksIds.clear();
  m_ChangeCore = false;
}

CRevisionData::CRevisionData()
{
  clear();
}

QString CRevisionData::SHA() const
{
    return m_SHA;
}

void CRevisionData::setSHA(const QString &SHA)
{
    m_SHA = SHA;
}

QString CRevisionData::DeveloperName() const
{
    return m_DeveloperName;
}

void CRevisionData::setDeveloperName(const QString &DeveloperName)
{
    m_DeveloperName = DeveloperName;
}

double CRevisionData::HoursSpent() const
{
    return m_HoursSpent;
}

void CRevisionData::addHoursSpent(double h)
{
  m_HoursSpent += h;
}

void CRevisionData::setHoursSpent(double HoursSpent)
{
    m_HoursSpent = HoursSpent;
}

bool CRevisionData::RedmineLinked() const
{
    return m_RedmineTasksIds.size() > 0;
}

std::vector<long> CRevisionData::RedmineTasksIds() const
{
  std::vector<long> vec;
  std::copy(m_RedmineTasksIds.begin(),m_RedmineTasksIds.end(),std::back_inserter(vec));
  return vec;
}

void CRevisionData::setRedmineLinked(const std::vector<long> &tasksIds)
{
  m_RedmineTasksIds.clear();
  m_RedmineTasksIds.insert(tasksIds.begin(), tasksIds.end());
}

bool CRevisionData::isChangeCore() const
{
  return m_ChangeCore;
}

void CRevisionData::setChangesCore(bool changes)
{
  m_ChangeCore = changes;
}

///////////////////////////////////////////////////////////////////////////

CRedmineTimeData::CRedmineTimeData()
{
  clear();
}

void CRedmineTimeData::clear()
{
  m_IssueID = 0;
  m_HoursSpent = 0.0;
  m_SHA.clear();
  m_DeveloperName.clear();
}

long CRedmineTimeData::IssueID() const
{
    return m_IssueID;
}

void CRedmineTimeData::setIssueID(long IssueID)
{
    m_IssueID = IssueID;
}

double CRedmineTimeData::HoursSpent() const
{
    return m_HoursSpent;
}

void CRedmineTimeData::setHoursSpent(double HoursSpent)
{
    m_HoursSpent = HoursSpent;
}

bool CRedmineTimeData::IsDevelopment() const
{
    return !m_SHA.isEmpty();
}

void CRedmineTimeData::setRevision(const QString &revisionSHA)
{
  m_SHA = revisionSHA;
}

QString CRedmineTimeData::DeveloperName() const
{
    return m_DeveloperName;
}

void CRedmineTimeData::setDeveloperName(const QString &DeveloperName)
{
    m_DeveloperName = DeveloperName;
}
