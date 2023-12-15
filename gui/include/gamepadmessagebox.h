#ifndef GAMEPADMESSAGEBOX_H
#define GAMEPADMESSAGEBOX_H

#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>

class GamepadMessageBox : public QMessageBox
{
    Q_OBJECT
public:
    explicit GamepadMessageBox(QWidget *parent = nullptr);
    GamepadMessageBox(Icon icon, const QString &title, const QString &text, StandardButtons buttons = NoButton,
                      QWidget *parent = nullptr, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    static StandardButton critical(QWidget *parent, const QString &title, const QString &text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
    static StandardButton information(QWidget *parent, const QString &title, const QString &text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
    static StandardButton question(QWidget *parent, const QString &title, const QString &text, StandardButtons buttons = StandardButtons(Yes | No), StandardButton defaultButton = NoButton);
    static StandardButton warning(QWidget *parent, const QString &title, const QString &text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
protected:
    virtual bool event(QEvent* event) override;

private:
    static StandardButton showMessageBox(QWidget *parent,
                                         QMessageBox::Icon icon,
                                         const QString& title, const QString& text,
                                         QMessageBox::StandardButtons buttons,
                                         QMessageBox::StandardButton defaultButton);

signals:
};

#endif // GAMEPADMESSAGEBOX_H
