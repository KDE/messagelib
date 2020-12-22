/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILTRACKINGWARNINGWIDGET_H
#define MAILTRACKINGWARNINGWIDGET_H

#include <KMessageWidget>

#include "messageviewer_private_export.h"

#include <QMap>
#include <QPointer>

#include <viewer/webengine/blockmailtrackingurlinterceptor/blockmailtrackingurlinterceptor.h>

namespace MessageViewer {
class MailTrackingDetailsDialog;
class MESSAGEVIEWER_TESTS_EXPORT MailTrackingWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit MailTrackingWarningWidget(QWidget *parent = nullptr);
    ~MailTrackingWarningWidget() override;
    void addTracker(const MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList &);
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

#endif // MAILTRACKINGWARNINGWIDGET_H
