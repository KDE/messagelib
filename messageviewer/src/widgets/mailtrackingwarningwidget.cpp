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
#include "mailtrackingdetailsdialog.h"
#include <KLocalizedString>

using namespace MessageViewer;
MailTrackingWarningWidget::MailTrackingWarningWidget(QWidget *parent)
    : KMessageWidget(parent)
{
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Warning);
    setWordWrap(true);

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
        if (!mMailTrackingDetailDialog) {
            mMailTrackingDetailDialog = new MessageViewer::MailTrackingDetailsDialog;
        }
        mMailTrackingDetailDialog->setDetails(generateDetails());
        mMailTrackingDetailDialog->show();
    }
}

QString MailTrackingWarningWidget::generateDetails() const
{
    QString details = QLatin1String("<b>") + i18n("Details:") + QLatin1String("</b><ul>");

    QMapIterator<QString, blackListFound> i(mBackLists);
    while (i.hasNext()) {
        i.next();
        details += QLatin1String("<li>") + i18np("1 tracker from the company %2 (%3)",
                                                 "%1 trackers from the company %2 (%3)",
                                                 i.value().number, i.key(), i.value().url);
    }
    details += QLatin1String("</ul>");
    return details;
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
    if (!isVisible()) {
        animatedShow();
    }
}

void MailTrackingWarningWidget::hideAndClear()
{
    mBackLists.clear();
    setVisible(false);
}
