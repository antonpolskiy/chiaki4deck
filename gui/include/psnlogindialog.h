#ifndef PSNLOGINDIALOG_H
#define PSNLOGINDIALOG_H

#include <QNetworkReply>
#include <QWebEngineView>
#include "registdialog.h"
#include "psnaccountid.h"
#include "settings.h"

class PSNLoginDialog : public QDialog {
    Q_OBJECT

private:
    QWebEngineView* web_engine_view;
    ChiakiLog log;

public:
    PSNLoginDialog(Settings *settings, RegistDialog *parent = nullptr);

    private slots:
        void handleWebEngineLoadFinished(bool);
        void handlePsnAccountIdResponse(QString accountId);
        void handlePsnAccountIdCollectionError(const QString& url, const QNetworkReply::NetworkError& error);

};



#endif //PSNLOGINDIALOG_H
