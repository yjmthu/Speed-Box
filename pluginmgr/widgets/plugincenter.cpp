#include "plugincenter.hpp"
#include "downloadingdlg.hpp"
#include "tabnative.hpp"
#include "tabonline.hpp"
#include "tabversion.hpp"
#include "tabnetproxy.hpp"
#include "tabhotkey.hpp"

#include <httplib.h>
#include <yjson.h>
#include <menubase.hpp>
#include <pluginmgr.h>
#include <neomenu.hpp>

#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace std::literals;

const std::u8string PluginCenter::m_RawUrl = u8"https://gitlab.com/yjmthu1/neoboxplg/-/raw/main/";
PluginCenter* PluginCenter::m_Instance = nullptr;

PluginCenter::PluginCenter()
  : WidgetBase(nullptr)
  , m_Setting(mgr->GetPluginsInfo())
  , m_PluginData(nullptr)
  , m_MainLayout(new QVBoxLayout(this))
  , m_TabWidget(new QTabWidget(this))
  , m_TabNative(new TabNative(this))
  , m_TabOnline(new TabOnline(this))
  , m_TabHotKey(new TabHotKey(this))
  , m_TabNetProxy(new TabNetProxy(this))
  , m_TabVersion(new TabVersion(this))
{
  m_Instance = this;
  SetupUi();
  // m_TabWidget->setResizeMode(QListView::Adjust);
  InitConnect();
}

PluginCenter::~PluginCenter()
{
  m_Instance = nullptr;
  delete m_PluginData;
}

void PluginCenter::SetupUi()
{
  setWindowTitle(QStringLiteral("Neobox-控制面板"));
  setFixedSize(520, 360);
  setWindowFlag(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setAttribute(Qt::WA_DeleteOnClose, true);
  setStyleSheet(mgr->m_Menu->styleSheet());
  auto backGround = new QWidget(this);
  backGround->setObjectName("emptyBackground");
  m_MainLayout->setContentsMargins(3, 3, 3, 3);
  m_MainLayout->addWidget(backGround);
  
  AddCloseButton();
  AddMinButton();

  auto layout = new QHBoxLayout(backGround);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_TabWidget);
  m_TabWidget->addTab(m_TabNative, "本地插件");
  m_TabWidget->addTab(m_TabOnline, "网络插件");
  m_TabWidget->addTab(m_TabHotKey, "热键管理");
  m_TabWidget->addTab(m_TabNetProxy, "网络设置");
  m_TabWidget->addTab(m_TabVersion, "关于软件");
}

void PluginCenter::InitConnect()
{
  connect(m_TabWidget, &QTabWidget::currentChanged, this, [this](int index){
    if (index == 1) {
      m_TabOnline->UpdatePlugins();
      m_TabNative->UpdatePlugins();
    }
  });
}

bool PluginCenter::UpdatePluginData()
{
  if (m_PluginData) return true;
  
  bool result = false, exit = false;

  if (!HttpLib::IsOnline()) {
    mgr->ShowMsgbox(u8"失败", u8"请检查网络连接！");
    return result;
  }

  const auto dialog = new DownloadingDlg(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  dialog->m_PreventClose = true;
  connect(this, &PluginCenter::DownloadFinished, dialog, &QDialog::close);
  // connect(this, &ItemBase::Downloading, dialog, &DownloadingDlg::SetPercent);

  std::thread thread([&](){
    HttpLib clt(m_RawUrl + u8"plugins.json"s);
    auto res = clt.Get();
    if (res->status == 200) {
      m_PluginData = new YJson(res->body.begin(), res->body.end());
      result = true;
    }
    dialog->m_PreventClose = false;
    emit DownloadFinished();
  });

  dialog->exec();
  thread.join();
  // https://gitlab.com/yjmthu1/neoboxplg/-/raw/main/plugins/neohotkeyplg/manifest.json
  return result;
}

