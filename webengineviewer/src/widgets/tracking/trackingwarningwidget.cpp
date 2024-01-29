/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "trackingwarningwidget.h"
#include "trackingdetailsdialog.h"
#include <KLocalizedString>

using namespace WebEngineViewer;
TrackingWarningWidget::TrackingWarningWidget(QWidget *parent)
    : KMessageWidget(parent)
{
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Warning);
    setWordWrap(true);
    setPosition(KMessageWidget::Header);

    setText(i18n("Some Mail Tracker was found and was blocked.<a href=\"mailtrackingdetails\">(Details...)"));
    connect(this, &TrackingWarningWidget::linkActivated, this, &TrackingWarningWidget::slotShowDetails);
}

TrackingWarningWidget::~TrackingWarningWidget() = default;

void TrackingWarningWidget::slotShowDetails(const QString &content)
{
    if (content == QLatin1StringView("mailtrackingdetails")) {
        if (!mMailTrackingDetailDialog) {
            mMailTrackingDetailDialog = new WebEngineViewer::TrackingDetailsDialog;
        }
        mMailTrackingDetailDialog->setDetails(generateDetails());
        mMailTrackingDetailDialog->show();
    }
}

QString TrackingWarningWidget::generateDetails() const
{
    QString details = QLatin1StringView("<b>") + i18n("Details:") + QLatin1StringView("</b><ul>");

    QMapIterator<QString, blackListFound> i(mBackLists);
    while (i.hasNext()) {
        i.next();
        details += QLatin1StringView("<li>")
            + i18np("1 tracker from the company %2 (%3)", "%1 trackers from the company %2 (%3)", i.value().number, i.key(), i.value().url);
    }
    details += QLatin1StringView("</ul>");
    return details;
}

void TrackingWarningWidget::addTracker(const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &tracker)
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

void TrackingWarningWidget::hideAndClear()
{
    mBackLists.clear();
    setVisible(false);
}

#include "moc_trackingwarningwidget.cpp"
