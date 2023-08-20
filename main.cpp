#include <QApplication>
#include <neobox/pluginmgr.h>
#include <QMessageBox>

// #include <winrt/Windows.Foundation.h>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  a.setQuitOnLastWindowClosed(false);
#ifdef _RELEASE
  try {
    PluginMgr manager;
    manager.Exec();
  } catch (const std::runtime_error& error) {
    auto msg = QString::fromLocal8Bit(error.what());
    QMessageBox::critical(nullptr, "Runtime Error", msg);
  } catch (const std::logic_error& error) {
    auto msg = QString::fromLocal8Bit(error.what());
    QMessageBox::critical(nullptr, "Logic Error", msg);
  }
#else
  PluginMgr manager;
  manager.Exec();
#endif
  return 0;
}
