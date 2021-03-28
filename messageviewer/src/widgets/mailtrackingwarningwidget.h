/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMessageWidget>

#include "messageviewer_private_export.h"

#include <QMap>
#include <QPointer>

#include <WebEngineViewer/BlockTrackingUrlInterceptor>

namespace MessageViewer
{
class MailTrackingDetailsDialog;
class MESSAGEVIEWER_TESTS_EXPORT MailTrackingWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit MailTrackingWarningWidget(QWidget *parent = nullptr);
    ~MailTrackingWarningWidget() override;
    void addTracker(const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &);
    void hideAndClear();

private:
    void slotShowDetails(const QString &content);
    Q_REQUIRED_RESULT QString generateDetails() const;

    struct blackListFound {
        QString url;
        int number = 1;
    };
    QMap<QString, blackListFound> mBackLists;
    QPointer<MailTrackingDetailsDialog> mMailTrackingDetailDialog;
};
}

