#ifndef GAMEPADMESSAGEBOX_H
#define GAMEPADMESSAGEBOX_H

#include <QDialog>
#include <QMessageBox>
#include <QMap>

class GamepadMessageBox : public QDialog
{
    Q_OBJECT
public:
    GamepadMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::NoButton,
                      QWidget *parent = nullptr, int timeout = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    virtual ~GamepadMessageBox();
    static QMessageBox::StandardButton critical(QWidget* parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton information(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton question(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No), QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton warning(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static GamepadMessageBox *splash(QWidget *parent, const QString &title, const QString &text, int timeout);

    QMessageBox::StandardButton selectedButton();


protected:
    virtual bool event(QEvent* event) override;
    virtual void paintEvent(QPaintEvent* paintEvent) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;


private:
    QMessageBox::Icon m_icon;
    QString m_title;
    QString m_text;
    QMessageBox::StandardButton m_selected_button;

    QVector<QMessageBox::StandardButton> m_buttons;
    int m_focused;
    int m_inside;
    bool m_update_rects;
    QRect m_wnd_rect;
    QRect m_icon_rect;
    QRect m_title_rect;
    QRect m_message_rect;
    QVector<QRect> m_buttons_rect;


    static QMessageBox::StandardButton messageBox(QWidget* parent, QMessageBox::Icon icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton);
    void focusNextButton();
    void focusPrevButton();
    void setFocused(int button);

    void getRects(const QRect& wnd_rect);
    void drawIcon(QPainter *p);
    void drawTitle(QPainter *p);
    void drawMessage(QPainter *p);
    void drawButtons(QPainter *p);

    static const QMap<QMessageBox::StandardButton, QString> c_button_text;
    static QIcon c_information_icon;
    static QIcon c_question_icon;
    static QIcon c_warning_icon;
    static QIcon c_critical_icon;
    static QFont c_title_font;
    static QFont c_message_font;
    static void initResources();
};

#endif // GAMEPADMESSAGEBOX_H
