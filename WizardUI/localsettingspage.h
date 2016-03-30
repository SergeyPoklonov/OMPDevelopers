#ifndef LOCALSETTINGSPAGE_H
#define LOCALSETTINGSPAGE_H

#include "wizardpagedocumentsupport.h"

#include <QWizardPage>

namespace Ui {
class LocalSettingsPage;
}

class QTableWidgetItem;

class LocalSettingsPage : public QWizardPage
                        , public CWizardPageDocumentSupport
{
  Q_OBJECT

public:
  explicit LocalSettingsPage(QWidget *parent = 0);
  ~LocalSettingsPage();

  virtual bool initialize( DocumentDataManager *doc ) override;

  virtual bool validatePage() override;

public slots:
  void onDeveloperDataChanged( QTableWidgetItem *item );

private:
  void intializeDevelopersList();
  void saveDevelopersToDocument();

private:
  Ui::LocalSettingsPage *ui;

  static const int DEVLIST_COLSQTY = 2;
  static const int DEVLIST_COLNUM_NAME = 0;
  static const int DEVLIST_COLNUM_HDAYS = 1;
};

#endif // LOCALSETTINGSPAGE_H
