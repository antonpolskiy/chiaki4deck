#include "gamepadmessagebox.h"
#include "controllerevent.h"
#include "QAbstractButton"

GamepadMessageBox::GamepadMessageBox(QWidget *parent)
    : QMessageBox{parent}
{}

GamepadMessageBox::GamepadMessageBox(Icon icon, const QString &title, const QString &text, StandardButtons buttons, QWidget *parent, Qt::WindowFlags f)
    : QMessageBox(icon, title, text, buttons, parent, f)
{
    std::cout << __func__ << std::endl;
}

QMessageBox::StandardButton GamepadMessageBox::critical(QWidget *parent, const QString &title, const QString &text, StandardButtons buttons, StandardButton defaultButton)
{
    return showMessageBox(parent, QMessageBox::Critical, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton GamepadMessageBox::information(QWidget *parent, const QString &title, const QString &text, StandardButtons buttons, StandardButton defaultButton)
{
    return showMessageBox(parent, QMessageBox::Information, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton GamepadMessageBox::question(QWidget *parent, const QString &title, const QString &text, StandardButtons buttons, StandardButton defaultButton)
{
    return showMessageBox(parent, QMessageBox::Question, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton GamepadMessageBox::warning(QWidget *parent, const QString &title, const QString &text, StandardButtons buttons, StandardButton defaultButton)
{
    return showMessageBox(parent, QMessageBox::Warning, title, text, buttons, defaultButton);
}

bool GamepadMessageBox::event(QEvent *event)
{
    if( event->type() != ControllerEvent::type )
        return QMessageBox::event(event);

    std::cout <<"GAMEPAD " <<__func__ << " " << std::endl;
    ControllerEvent* cevent = dynamic_cast<ControllerEvent*>(event);
    if(cevent->eventType() != ControllerEvent::Event_Button)
        return true;
    ControllerButtonEvent* cbevent = dynamic_cast<ControllerButtonEvent*>(event);
    std::string key;

    QList<QAbstractButton*> btns = buttons();

    auto it = btns.begin();
    while( it != btns.end() )
    {
        if( (*it)->hasFocus() )
            break;
        it++;
    }

    if( cbevent->pressed() )
    {
        switch( cbevent->button() )
        {
        case CHIAKI_CONTROLLER_BUTTON_DPAD_LEFT : key = "Left"; focusPreviousChild(); break;
        case CHIAKI_CONTROLLER_BUTTON_DPAD_RIGHT : key = "Right"; focusNextChild();  break;
        case CHIAKI_CONTROLLER_BUTTON_CROSS : key = "Enter"; (*it)->click(); break;
        case CHIAKI_CONTROLLER_BUTTON_MOON : key = "Back"; this->reject(); break;
        }
    }
    std::cout << "key: " << key << std::endl;

    return true;
}

QMessageBox::StandardButton GamepadMessageBox::showMessageBox(QWidget *parent, Icon icon, const QString &title, const QString &text, StandardButtons buttons, StandardButton defaultButton)
{
    GamepadMessageBox msgBox(icon, title, text, QMessageBox::NoButton, parent);
    QDialogButtonBox *buttonBox = msgBox.findChild<QDialogButtonBox*>();
    Q_ASSERT(buttonBox != nullptr);

    uint mask = QMessageBox::FirstButton;
    while (mask <= QMessageBox::LastButton) {
        uint sb = buttons & mask;
        mask <<= 1;
        if (!sb)
            continue;
        QPushButton *button = msgBox.addButton((QMessageBox::StandardButton)sb);
        // Choose the first accept role as the default
        if (msgBox.defaultButton())
            continue;
        if ((defaultButton == QMessageBox::NoButton && buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
            || (defaultButton != QMessageBox::NoButton && sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}
