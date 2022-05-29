#ifndef SPEEDMENU_H
#define SPEEDMENU_H

#include <QMenu>

class SpeedMenu : public QMenu
{
    Q_OBJECT
protected:
public:
    void showEvent(QShowEvent *event) override;
    explicit SpeedMenu(QWidget *parent=nullptr);
    ~SpeedMenu();
private:
    QAction* m_Actions;
    QAction* m_AdditionalAction;
    QAction* m_AutoStartApp;
    void SetupUi();
    void SetupConntects();
    void SetupSettingMenu();
    void SetupImageType(QMenu* parent, QAction* ac);
    void SetAdditionalMenu();
private slots:
    void ScreenShot();
signals:
    void ChangeBoxColor(QColor col);
    void ChangeBoxAlpha(int alpha);
};

#endif // SPEEDMENU_H
