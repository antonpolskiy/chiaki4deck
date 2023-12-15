#include "SteamDeckMainWindow.h"
#include "gamepadmessagebox.h"
#include "registdialog.h"
#include "settingsdialog.h"
#include "streamsession.h"
#include "streamwindow.h"
#include "ui_SteamDeckMainWindow.h"
#include "QStringListModel"
#include "displayserverbutton.h"
#include <iostream>
#include <QKeyEvent>
#include <QButtonGroup>
#include <QCursor>
#include <QApplication>
#include <settings.h>
#include <controllerevent.h>

#define SETTINGS_BTN INT_MAX
#define ADD_CONSOLE_BTN SETTINGS_BTN-1

SteamDeckMainWindow::SteamDeckMainWindow(Settings *settings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SteamDeckMainWindow),
    m_show_registered(true),
    last_checked_id(-1),
    m_settings(settings)
{
    ui->setupUi(this);
    setWindowTitle(qApp->applicationName());
    QCoreApplication::instance()->installEventFilter(this);

    connect(&m_button_group, SIGNAL(buttonToggled(int,bool)), this, SLOT(onButtonToggled(int,bool)));
    connect(&m_timer, &QTimer::timeout, this, [this]{controlCursor(false);});

    connect(&discovery_manager, &DiscoveryManager::HostsUpdated, this, &SteamDeckMainWindow::UpdateDisplayServers);
    connect(settings, &Settings::RegisteredHostsUpdated, this, &SteamDeckMainWindow::UpdateDisplayServers);

    m_button_group.addButton(ui->settings_btn,SETTINGS_BTN);
    connect( ui->settings_btn, SIGNAL(clicked(bool)), this, SLOT(onButtonTrigered()));

    DisplayServerButton* pb = new DisplayServerButton("Add Console", ":/icons/AddConsole.svg");

    addButton(pb,HostMAC(), ADD_CONSOLE_BTN);


    m_timer.setSingleShot(true);
    m_timer.start(1);
    discovery_manager.SetActive(true);
    UpdateDisplayServers();
    UpdateDiscoveryEnabled();
    connect(ControllerManager::GetInstance(), SIGNAL(AvailableControllersUpdated(void)), this, SLOT(UpdateGamepads(void)) );

    grabControls();
}

bool SteamDeckMainWindow::eventFilter(QObject *pWatched, QEvent *pEvent)
{
    if (pEvent->type() == QEvent::MouseMove)
    {
        controlCursor(true);

        m_timer.start(m_inactivity_duration);
    }

    return QObject::eventFilter(pWatched, pEvent);
}

void SteamDeckMainWindow::SendWakeup(const DisplayServer *server)
{
    if(!server->registered)
        return;

    try
    {
        discovery_manager.SendWakeup(server->GetHostAddr(), server->registered_host.GetRPRegistKey(),
                                     chiaki_target_is_ps5(server->registered_host.GetTarget()));
    }
    catch(const Exception &e)
    {
        GamepadMessageBox::critical(this, tr("Wakeup failed"), tr("Failed to send Wakeup packet:\n%1").arg(e.what()));
        return;
    }
}

void SteamDeckMainWindow::showRegisteredServers( bool show_registered )
{
    std::cout << __func__ << " " << show_registered << std::endl;
    if(show_registered == m_show_registered)
    {
        return;
    }
    if( show_registered && m_settings->GetRegisteredHosts().isEmpty() ) return;
    m_show_registered = show_registered;
    ui->title->setText(m_show_registered?"Select console":"Add console");
    dynamic_cast<DisplayServerButton*>(m_button_group.button(ADD_CONSOLE_BTN))->setName(m_show_registered?"Add console":"Add manually");
    UpdateDisplayServers();
}

void SteamDeckMainWindow::grabControls()
{
    grabKeyboard();
    UpdateGamepads();
}

void SteamDeckMainWindow::releaseControls(bool release_gamepad)
{
    releaseKeyboard();
    if(!release_gamepad) return;
    auto it = m_controllers.begin();
    while( it != m_controllers.end() )
    {
        delete *it;
        it = m_controllers.erase(it);
    }

}

void SteamDeckMainWindow::addButton( DisplayServerButton* button, const HostMAC& host, int id  )
{
    connect( button, SIGNAL(clicked(bool)), this, SLOT(onButtonTrigered()));
    QHBoxLayout* layout = dynamic_cast<QHBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    layout->insertWidget(id,button);
    m_button_group.addButton(button, id);
    if( id != ADD_CONSOLE_BTN )
        m_server_map.insert(id,host);

    if( id == 0 || id == ADD_CONSOLE_BTN )
    {
        button->setChecked(true);
    }
}

QMap<int,HostMAC>::iterator SteamDeckMainWindow::removeButton( QMap<int,HostMAC>::iterator it  )
{
    DisplayServerButton* button = dynamic_cast<DisplayServerButton*>(m_button_group.button(it.key()));
    disconnect( button );
    QHBoxLayout* layout = dynamic_cast<QHBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    layout->removeWidget(button);
    m_button_group.removeButton(button);
    delete button;
    return m_server_map.erase(it);
}

int SteamDeckMainWindow::nextId(int id)
{
    if( id == -1 )
    {
        return m_server_map.empty() ? 0 : m_server_map.lastKey()+1;
    }
    else
    {
        auto it = m_server_map.find(id)+1;
        if( it != m_server_map.end() )
            return it.key();
        //worst
        for( auto it = m_server_map.keyBegin(); it != m_server_map.keyEnd(); it++ )
        {
            if( *it > id )
                return *it;
        }
    }
    return -1;
}

int SteamDeckMainWindow::prevId(int id)
{
    if( id <= 0 )
    {
        return -1;
    }
    else
    {
        auto it = m_server_map.find(id)-1;
        if( it != m_server_map.end() )
        {

            std::cout << "prevID1 " << it.key()  << std::endl;
            return it.key();
        }
        //worst
        for( auto it = m_server_map.keyBegin(); it != m_server_map.keyEnd(); it++ )
        {
            if( *it >= id )
                return *(it--);
        }
    }
    return -1;
}

void SteamDeckMainWindow::selectNext(bool rightToLeft)
{
    int id = -1;
    if(m_button_group.checkedId() == SETTINGS_BTN || m_server_map.empty())
        return;
    if( rightToLeft  )
    {
        if(m_button_group.checkedId() == ADD_CONSOLE_BTN)
        {
            id = m_server_map.lastKey();
        }
        else
        {
            id = prevId(m_button_group.checkedId());

            //std::cout << id << std::endl;
        }
    }
    else
    {
        if( m_button_group.checkedId() == ADD_CONSOLE_BTN )
            return;
        if( m_button_group.checkedId() == m_server_map.lastKey() )
            id = ADD_CONSOLE_BTN;
        else
            id = nextId(m_button_group.checkedId());
    }

    if( id != -1)
        m_button_group.button(id)->setChecked(true);
}

void SteamDeckMainWindow::controlCursor(bool show)
{
    QCursor cursor(show?Qt::ArrowCursor:Qt::BlankCursor);
    QApplication::setOverrideCursor(cursor);
}


SteamDeckMainWindow::~SteamDeckMainWindow()
{
    delete ui;
}

void SteamDeckMainWindow::keyPressEvent(QKeyEvent *event) {
    std::string key;

    switch( event->key() )
    {
    case Qt::Key_Up : key = "Up"; m_button_group.button(last_checked_id)->setChecked(true); break;
    case Qt::Key_Down : key = "Down"; m_button_group.button(SETTINGS_BTN)->setChecked(true); break;
    case Qt::Key_Left : key = "Left"; selectNext(true); break;
    case Qt::Key_Right : key = "Right"; selectNext(false); break;
    case Qt::Key_Return : key = "Enter"; onButtonTrigered();  break;
    case Qt::Key_Escape : key = "Back"; if( !m_show_registered ) showRegisteredServers(true); break;
    case Qt::Key_J : QCoreApplication::postEvent (QApplication::focusWidget(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier ));
    }
    std::cout << "key: " << key << std::endl;
}

bool SteamDeckMainWindow::event(QEvent *event)
{
    if( event->type() != ControllerEvent::type )
        return QMainWindow::event(event);

    std::cout << __func__ << " " << std::endl;
    ControllerEvent* cevent = dynamic_cast<ControllerEvent*>(event);
    if(cevent->eventType() != ControllerEvent::Event_Button)
        return true;
    ControllerButtonEvent* cbevent = dynamic_cast<ControllerButtonEvent*>(event);
    std::string key;

    if( cbevent->pressed() )
    {
        switch( cbevent->button() )
        {
        case CHIAKI_CONTROLLER_BUTTON_DPAD_UP : key = "Up"; m_button_group.button(last_checked_id)->setChecked(true); break;
        case CHIAKI_CONTROLLER_BUTTON_DPAD_DOWN : key = "Down"; m_button_group.button(SETTINGS_BTN)->setChecked(true); break;
        case CHIAKI_CONTROLLER_BUTTON_DPAD_LEFT : key = "Left"; selectNext(true); break;
        case CHIAKI_CONTROLLER_BUTTON_DPAD_RIGHT : key = "Right"; selectNext(false); break;
        case CHIAKI_CONTROLLER_BUTTON_CROSS : key = "Enter"; onButtonTrigered();  break;
        case CHIAKI_CONTROLLER_BUTTON_MOON : key = "Back"; if( !m_show_registered ) showRegisteredServers(true); break;
        }
    }
    std::cout << "key: " << key << std::endl;

    return true;
}

void SteamDeckMainWindow::onButtonToggled(int id, bool checked)
{
    if(checked && id != SETTINGS_BTN)
    {
        last_checked_id = id;
    }
}

void SteamDeckMainWindow::onButtonTrigered()
{
    int id = m_button_group.checkedId();
    if( id == ADD_CONSOLE_BTN )
    {
        if( m_show_registered )
        {
            showRegisteredServers(false);
        }
        else
        {
            releaseControls();

            RegistDialog regist_dialog(m_settings, QString(), this);// subnet as parameter
            regist_dialog.exec();

            grabControls();
        }
        return;
    }
    if( id == SETTINGS_BTN )
    {
        releaseControls();
        SettingsDialog dialog(m_settings, this);
        dialog.exec();
        grabControls();
        return;
    }
    DisplayServer* s = DisplayServerFromId(id);
    ServerItemWidgetTriggered(s);

}

void SteamDeckMainWindow::ServerItemWidgetTriggered(DisplayServer* server)
{
    if(!server)
        return;

    if(server->registered)
    {
        if(server->discovered && server->discovery_host.state == CHIAKI_DISCOVERY_HOST_STATE_STANDBY)
        {
            if(m_settings->GetAutomaticConnect())
            {
                SendWakeup(server);
                time_t start, end;
                time(&start);
                do
                    time(&end);
                while(difftime(end, start) <= 10);
            }
            else
            {
                releaseControls();
                int r = GamepadMessageBox::question(this,
                                              tr("Start Stream"),
                                              tr("The Console is currently in standby mode.\nShould we send a Wakeup packet instead of trying to connect immediately?"),
                                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                if(r == QMessageBox::Cancel)
                {
                    grabControls();
                    return;
                }
                else if (r == QMessageBox::Yes)
                {
                    SendWakeup(server);
                    time_t start, end;
                    time(&start);
                    do
                        time(&end);
                    while(difftime(end, start) <= 10);
                }
            }
        }



        QString host = server->GetHostAddr();

        releaseControls(true);
        StreamSessionConnectInfo info(
            m_settings,
            server->registered_host.GetTarget(),
            host,
            server->registered_host.GetRPRegistKey(),
            server->registered_host.GetRPKey(),
            QString(""),
            false,
            false,
            false);
        StreamWindow* wnd = new StreamWindow(info);
        connect( wnd, SIGNAL(WindowClosed()), this, SLOT(OnStreamWindowClosed()) );
        hide();
    }
    else
    {
        releaseControls();
        RegistDialog regist_dialog(m_settings, server->GetHostAddr(), this);
        int r = regist_dialog.exec();
        showRegisteredServers(true);
        grabControls();
    }
}

void SteamDeckMainWindow::ServerItemWidgetDeleteTriggered()
{

}

void SteamDeckMainWindow::UpdateDiscoveryEnabled()
{

}

void SteamDeckMainWindow::keyReleaseEvent(QKeyEvent *event){}

void SteamDeckMainWindow::addDisplayServer( const RegisteredHost& registered )
{
    DisplayServer server;
    server.discovered = false;
    server.registered_host = registered;
    server.registered = true;
    display_servers.insert(registered.GetServerMAC(), server);
}

void SteamDeckMainWindow::addDisplayServer( const DiscoveryHost& discovery )
{
    if( m_settings->GetRegisteredHostRegistered(discovery.GetHostMAC()) )
    {
        std::cout << "already registered" <<std::endl;
        return;
    }

    DisplayServer server;
    server.discovered = true;
    server.discovery_host = discovery;

    server.registered = false;
    display_servers.insert(discovery.GetHostMAC(), server);
}

void SteamDeckMainWindow::updateDisplayServer( const DiscoveryHost& discovery )
{
    auto& server = display_servers[discovery.GetHostMAC()];
    server.discovered = true;
    server.discovery_host = discovery;
};

void SteamDeckMainWindow::UpdateDisplayServers()
{
    display_servers.clear();

    if( m_show_registered )
    {
        auto servers = m_settings->GetRegisteredHosts();
        if( servers.isEmpty() )
        {
            showRegisteredServers(false);
            return;
        }
        for( const auto &host : qAsConst(servers) )
        {
            addDisplayServer(host);
        }
    }

    for(const auto &host : discovery_manager.GetHosts())
    {
        if( display_servers.contains(host.GetHostMAC()) )
            updateDisplayServer(host);
        else if( !m_show_registered )
        {

            std::cout << "show new" <<std::endl;
            addDisplayServer(host);
        }
    }

    UpdateServerWidgets(display_servers);
}

DisplayServer *SteamDeckMainWindow::DisplayServerFromId( int id )
{
    if(!m_server_map.contains(id))
        return nullptr;
    HostMAC host = m_server_map[id];
    int i = 0;
    if( display_servers.contains(host))
        return &display_servers[host];

    std::cout << "NO SERVER FOUND " << display_servers.size() << std::endl;
    return nullptr;
}

void SteamDeckMainWindow::UpdateServerWidgets(QMap<HostMAC,DisplayServer> servers)
{
    for( auto it = m_server_map.begin(); it != m_server_map.end();)
    {
        auto display_server = servers.find(it.value());
        if( display_server == servers.end() )
        {
            std::cout << "removeButton" <<std::endl;
            it = removeButton(it);
            continue;
        }

        DisplayServerButton* pb = dynamic_cast<DisplayServerButton*>(m_button_group.button(it.key()));
        pb->setName(display_server->discovered ?
                        display_server->discovery_host.host_name : display_server->GetHostAddr());

        std::cout << "updateButton" <<std::endl;
        servers.erase(display_server);
        ++it;
    }

    for( auto it = servers.begin(); it != servers.end(); ++it )
    {
        QString name = it->registered ? it->registered_host.GetServerNickname() :
                       ( it->discovered ? it->discovery_host.host_name : it->GetHostAddr() );
        DisplayServerButton* pb = new DisplayServerButton(name, it->IsPS5() ?
                                                              ":/icons/PS5.svg" : ":/icons/PS4.svg" );
        int id = nextId();
        addButton(pb,it.key(),id);
        std::cout << "addButton" <<std::endl;
    }
}

void SteamDeckMainWindow::UpdateGamepads()
{
#if CHIAKI_GUI_ENABLE_SDL_GAMECONTROLLER
    for(auto controller_id : m_controllers.keys())
    {
        auto controller = m_controllers[controller_id];
        if(!controller->IsConnected())
        {
            m_controllers.remove(controller_id);
            delete controller;
        }
    }

    const auto available_controllers = ControllerManager::GetInstance()->GetAvailableControllers();
    for(auto controller_id : available_controllers)
    {
        if(!m_controllers.contains(controller_id))
        {
            auto controller = ControllerManager::GetInstance()->OpenController(controller_id);
            if(!controller)
            {
                continue;
            }
            m_controllers[controller_id] = controller;

        }
    }
#endif
}

void SteamDeckMainWindow::OnStreamWindowClosed()
{
    std::cout << __func__ <<std::endl;
    this->show();
    grabControls();
}
