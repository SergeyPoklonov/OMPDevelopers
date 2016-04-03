#ifndef GITANALYZER_H
#define GITANALYZER_H

#include "developerworkdata.h"

#include <QDate>



class GitAnalyzer
{
public:
  GitAnalyzer();
  
public:
  class AnalyzeSettings
  {
  public:
    QString RepositoryPath;
    QDate DateFrom;
    QDate DateTo;
  };
  
  bool AnalyzeRepository(AnalyzeSettings settings, std::vector<CDeveloperWorkData> &workDevList, QString *errStr = nullptr);
};

#endif // GITANALYZER_H
