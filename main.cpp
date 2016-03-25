#include <QApplication>

#include <WizardUI/wizardmain.h>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  WizardMain w;

  if( !w.Initialize() )
    return 1;

  w.show();

  return a.exec();
}
