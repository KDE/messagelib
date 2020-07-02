/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WEBENGINEVIEWWITHSAFEBROWSINGSUPPORT_H
#define WEBENGINEVIEWWITHSAFEBROWSINGSUPPORT_H

#include <QWidget>
#include "../searchfullhashjob.h"
#include <WebEngineViewer/LocalDataBaseManager>
class QPlainTextEdit;
namespace WebEngineViewer {
class WebEngineView;
}
class WebEngineViewWithSafeBrowsingSupport : public QWidget
{
    Q_OBJECT
public:
    explicit WebEngineViewWithSafeBrowsingSupport(QWidget *parent = nullptr);
    ~WebEngineViewWithSafeBrowsingSupport();

private Q_SLOTS:
    void slotUrlClicked(const QUrl &url);
    void slotCheckedUrlFinished(const QUrl &url, WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status);

private:
    QPlainTextEdit *mDebug = nullptr;
    WebEngineViewer::WebEngineView *pageView = nullptr;
};

#endif // WEBENGINEVIEWWITHSAFEBROWSINGSUPPORT_H
