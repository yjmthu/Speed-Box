#include <QApplication>
#include <QProcess>

#include <speedbox.h>
#include <varbox.h>
#include <appcode.hpp>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  VarBox var;
  SpeedBox box;
  box.show();
  if (a.exec() == (int)ExitCode::RETCODE_RESTART) {
    QProcess::startDetached(a.applicationFilePath(), QStringList());
  }
  return 0;
}
