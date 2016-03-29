#ifndef DOCUMENTDATAMANAGER_H
#define DOCUMENTDATAMANAGER_H

#include <QObject>

class CDeveloperListDataManager;

class DocumentDataManager : public QObject
{
  Q_OBJECT
public:
  explicit DocumentDataManager(QObject *parent = 0);

  CDeveloperListDataManager& getDevelopersManager();

  bool SaveGeneralSettings();
  bool LoadGeneralSettings();

  QString getGeneralSettingsFilePath() const;

signals:

public slots:

private:
  void Construct();

  bool MakeGeneralSettingsXML(QString &xmlFileText);

private:
  CDeveloperListDataManager *m_DevelopersManager;
};

#endif // DOCUMENTDATAMANAGER_H
