#ifndef GENERALSETTINGSHOLDER_H
#define GENERALSETTINGSHOLDER_H

#include "developerdatamanager.h"

#include <QObject>

class QFile;

class GeneralSettingsHolder : public QObject
{
  Q_OBJECT
public:
  explicit GeneralSettingsHolder(QObject *parent = 0);
  
  void clear();
  
  CDeveloperListDataManager& getDevelopersManager();
  
  void setGitPath(QString gitPath);
  QString getGitPath() const;
  
  void setGitWeb(QString url);
  QString getGitWeb() const;
  
  QString getRedmineURL() const;
  void setRedmineURL(const QString &RedmineURL);
  
  QString getRedmineAuthKey() const;
  void setRedmineAuthKey(const QString &RedmineAuthKey);
  
  bool isVaild(QString *errStr);
  
  bool load(const QString &filePath);
  bool save(const QString &filePath);
  
private:
  bool MakeXMLText(QString &xmlFileText);
  bool ParseXMLFile(QFile *xmlFile);
  
private:
  CDeveloperListDataManager *m_DevelopersManager;
  
  QString m_GitRepositoryPath;
  QString m_GitWebURL;
  
  QString m_RedmineURL;
  QString m_RedmineAuthKey;
};

#endif // GENERALSETTINGSHOLDER_H
