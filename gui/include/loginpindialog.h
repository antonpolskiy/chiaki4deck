// SPDX-License-Identifier: LicenseRef-AGPL-3.0-only-OpenSSL

#ifndef CHIAKI_LOGINPINDIALOG_H
#define CHIAKI_LOGINPINDIALOG_H

#include <QDialog>
#include <QIcon>
#include <QVector>

class QLineEdit;
class QDialogButtonBox;

class LoginPINDialog : public QDialog
{
	Q_OBJECT

	private:
        int m_entered_count;
		QString pin;
        QIcon m_button_map;
        QIcon m_entry_empty;
        QIcon m_entry_filled;
        QVector<int16_t> m_axises;

	public:
		explicit LoginPINDialog(bool incorrect, QWidget *parent = nullptr);

        QString GetPIN();

    protected:
        virtual void keyPressEvent(QKeyEvent *event) override;
        virtual bool event(QEvent* event) override;
        virtual void paintEvent(QPaintEvent* event) override;

    private:
        void paintEntry(QPainter& p, QRect r);
        void paintButtonMap(QPainter &p, QRect r);
        bool enterChar(QChar c);
        bool removeChar();
};

#endif // CHIAKI_LOGINPINDIALOG_H
