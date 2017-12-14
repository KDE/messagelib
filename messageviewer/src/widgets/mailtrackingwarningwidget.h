/*
   Copyright (C) 2017 Laurent Montel <montel@kde.org>

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
#include <QMap>

#include <viewer/webengine/blockmailtrackingurlinterceptor/blockmailtrackingurlinterceptor.h>

namespace MessageViewer {
class MailTrackingWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit MailTrackingWarningWidget(QWidget *parent = nullptr);
    ~MailTrackingWarningWidget();
    void addTracker(const MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList &);
    void hideAndClear();
private:
    struct blackListFound {
        QString url;
        int number = 1;
    };
    QMap<QString, blackListFound> mBackLists;
};
}

#endif // MAILTRACKINGWARNINGWIDGET_H
