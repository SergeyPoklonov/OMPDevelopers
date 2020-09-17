#include <QApplication>

#include <WizardUI/wizardmain.h>

int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

  QApplication a(argc, argv);

  WizardMain w;

  if( !w.Initialize() )
  {
    Q_ASSERT( false );
    return 1;
  }

  w.show();

  return a.exec();
}
