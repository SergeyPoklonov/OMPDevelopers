#include "wizardmain.h"
#include <QApplication>

int main(int argc, char *argv[])
{

  QApplication a(argc, argv);

  WizardMain w;
             w.show();

  return a.exec();
}
