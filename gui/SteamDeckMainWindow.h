#ifndef STEAMDECKMAINWINDOW_H
#define STEAMDECKMAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QTimer>
#include <QMap>
#include <displayserverbutton.h>

#include "discoverymanager.h"
#include "host.h"

#include "mainwindow.h"

// struct DisplayServer
// {
//     DiscoveryHost discovery_host;
//     ManualHost manual_host;
//     bool discovered;

//     RegisteredHost registered_host;
//     bool registered;

//     QString GetHostAddr() const { return discovered ? discovery_host.host_addr : manual_host.GetHost(); }
//     bool IsPS5() const { return discovered ? discovery_host.ps5 :
//                                     (registered ? chiaki_target_is_ps5(registered_host.GetTarget()) : false); }
// };

namespace Ui {
class SteamDeckMainWindow;
}

class SteamDeckMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SteamDeckMainWindow(Settings* settings, QWidget *parent = nullptr);
    ~SteamDeckMainWindow();
protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;


    virtual bool eventFilter(QObject *pWatched, QEvent *pEvent) override;

private:
    Ui::SteamDeckMainWindow *ui;
    bool m_show_registered;
    //legacy
    Settings *m_settings;

    DiscoveryManager discovery_manager;

    QMap<HostMAC,DisplayServer> display_servers;

    DisplayServer *DisplayServerFromId(int id);
    void SendWakeup(const DisplayServer *server);
    //legacy

    QButtonGroup m_button_group;
    int last_checked_id;
    QTimer m_timer;
    const int m_inactivity_duration = 1000;

    QMap<int,HostMAC> m_server_map;

    int nextId(int id = -1);
    void addButton(DisplayServerButton* button, const HostMAC &host, int id  );
    QMap<int,HostMAC>::iterator removeButton( QMap<int,HostMAC>::iterator it  );

    int prevId(int id);
    void selectNext(bool rightToLeft);

    void controlCursor(bool show);

    void addDisplayServer( const RegisteredHost& registered );
    void addDisplayServer( const ManualHost& manual );
    void addDisplayServer( const DiscoveryHost& manual );
    void updateDisplayServer( const RegisteredHost& registered );
    void updateDisplayServer( const ManualHost& manual );
    void updateDisplayServer( const DiscoveryHost& discovery );

    void showRegisteredServers(bool show_registered);
private slots:
    void onButtonToggled( int id, bool checked );
    void onButtonTrigered();
//legacy
    void ServerItemWidgetTriggered(DisplayServer *s);
    void ServerItemWidgetDeleteTriggered();

    void UpdateDiscoveryEnabled();

    void UpdateDisplayServers();
    void UpdateServerWidgets(QMap<HostMAC, DisplayServer> servers);
};

#endif // STEAMDECKMAINWINDOW_H
