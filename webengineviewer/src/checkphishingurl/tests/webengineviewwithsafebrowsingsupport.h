/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "../searchfullhashjob.h"
#include <QWidget>
#include <WebEngineViewer/LocalDataBaseManager>
class QPlainTextEdit;
namespace WebEngineViewer
{
class WebEngineView;
}
class WebEngineViewWithSafeBrowsingSupport : public QWidget
{
    Q_OBJECT
public:
    explicit WebEngineViewWithSafeBrowsingSupport(QWidget *parent = nullptr);
    ~WebEngineViewWithSafeBrowsingSupport() override;

private Q_SLOTS:
    void slotUrlClicked(const QUrl &url);
    void slotCheckedUrlFinished(const QUrl &url, WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status);

private:
    QPlainTextEdit *mDebug = nullptr;
    WebEngineViewer::WebEngineView *pageView = nullptr;
};
