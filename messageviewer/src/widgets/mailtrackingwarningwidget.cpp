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

#include "mailtrackingwarningwidget.h"
#include <KLocalizedString>

using namespace MessageViewer;
MailTrackingWarningWidget::MailTrackingWarningWidget(QWidget *parent)
    : KMessageWidget(parent)
{
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Warning);
    setWordWrap(true);

    //Add info here.
    setText(i18n("Some Mail Tracker was found and was blocked.<a href=\"mailtrackingdetails\">(Details...)"));
    connect(this, &MailTrackingWarningWidget::linkActivated, this,
            &MailTrackingWarningWidget::slotShowDetails);

}

MailTrackingWarningWidget::~MailTrackingWarningWidget()
{

}

void MailTrackingWarningWidget::slotShowDetails(const QString &content)
{
    if (content == QLatin1String("mailtrackingdetails")) {
        //TODO
    }
}

void MailTrackingWarningWidget::addTracker(const MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList &tracker)
{
    blackListFound item = mBackLists.value(tracker.mCompanyName);
    if (item.url.isEmpty()) {
        item.url = tracker.mCompanyUrl;
        mBackLists.insert(tracker.mCompanyName, item);
    } else {
        item.number = item.number + 1;
        mBackLists.insert(tracker.mCompanyName, item);
    }
    //TODO create list
    if (!isVisible()) {
        animatedShow();
    }
}

void MailTrackingWarningWidget::hideAndClear()
{
    mBackLists.clear();
    setVisible(false);
}
