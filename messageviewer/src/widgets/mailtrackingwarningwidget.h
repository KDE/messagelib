/*
   Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
    ~MailTrackingWarningWidget();
    void addTracker(const MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList &);
    void hideAndClear();
private:
    void slotShowDetails(const QString &content);
    QString generateDetails() const;

    struct blackListFound {
        QString url;
        int number = 1;
    };
    QMap<QString, blackListFound> mBackLists;
    QPointer<MailTrackingDetailsDialog> mMailTrackingDetailDialog;
};
}

#endif // MAILTRACKINGWARNINGWIDGET_H
