/*
  This file is part of KMail, the KDE mail client.
  SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "messagelistutil.h"
#include "messagelistsettings.h"
#include "messagelistutil_p.h"

#include <Akonadi/Item>
#include <KColorScheme>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMime/Message>
#include <QIcon>
#include <QMenu>

QString MessageList::Util::messageSortingConfigName()
{
    return QStringLiteral("MessageSorting");
}

QString MessageList::Util::messageSortDirectionConfigName()
{
    return QStringLiteral("MessageSortDirection");
}

QString MessageList::Util::groupSortingConfigName()
{
    return QStringLiteral("GroupSorting");
}

QString MessageList::Util::groupSortDirectionConfigName()
{
    return QStringLiteral("GroupSortDirection");
}

QString MessageList::Util::messageUniqueIdConfigName()
{
    return QStringLiteral("MessageUniqueIdForStorageModel%1");
}

QString MessageList::Util::storageModelSortOrderGroup()
{
    return QStringLiteral("MessageListView::StorageModelSortOrder");
}

QString MessageList::Util::storageModelThemesGroup()
{
    return QStringLiteral("MessageListView::StorageModelThemes");
}

QString MessageList::Util::storageModelAggregationsGroup()
{
    return QStringLiteral("MessageListView::StorageModelAggregations");
}

QString MessageList::Util::setForStorageModelConfigName()
{
    return QStringLiteral("SetForStorageModel%1");
}

QString MessageList::Util::storageModelSelectedMessageGroup()
{
    return QStringLiteral("MessageListView::StorageModelSelectedMessages");
}

void MessageList::Util::deleteConfig(const QString &collectionId)
{
    KConfigGroup confselectedMessage(MessageListSettings::self()->config(), MessageList::Util::storageModelSelectedMessageGroup());
    confselectedMessage.deleteEntry(MessageList::Util::messageUniqueIdConfigName().arg(collectionId));

    KConfigGroup storageModelOrder(MessageListSettings::self()->config(), MessageList::Util::storageModelSortOrderGroup());
    storageModelOrder.deleteEntry(collectionId + groupSortDirectionConfigName());
    storageModelOrder.deleteEntry(collectionId + groupSortingConfigName());
    storageModelOrder.deleteEntry(collectionId + messageSortDirectionConfigName());
    storageModelOrder.deleteEntry(collectionId + messageSortingConfigName());

    KConfigGroup storageModelTheme(MessageListSettings::self()->config(), MessageList::Util::storageModelThemesGroup());
    storageModelTheme.deleteEntry(collectionId + setForStorageModelConfigName());

    KConfigGroup storageModelAggregation(MessageListSettings::self()->config(), MessageList::Util::storageModelAggregationsGroup());
    storageModelAggregation.deleteEntry(collectionId + setForStorageModelConfigName());
}

QColor MessageList::Util::unreadDefaultMessageColor()
{
    return KColorScheme(QPalette::Active).foreground(KColorScheme::ActiveText).color();
}

QColor MessageList::Util::importantDefaultMessageColor()
{
    return KColorScheme(QPalette::Active).foreground(KColorScheme::NeutralText).color();
}

QColor MessageList::Util::todoDefaultMessageColor()
{
    return KColorScheme(QPalette::Active).foreground(KColorScheme::PositiveText).color();
}

void MessageList::Util::fillViewMenu(QMenu *menu, QObject *receiver)
{
    auto sortingMenu = new QMenu(i18n("Sorting"), menu);
    sortingMenu->setIcon(QIcon::fromTheme(QStringLiteral("view-sort-ascending")));
    menu->addMenu(sortingMenu);
    QObject::connect(sortingMenu, SIGNAL(aboutToShow()), receiver, SLOT(sortOrderMenuAboutToShow()));

    auto aggregationMenu = new QMenu(i18n("Aggregation"), menu);
    aggregationMenu->setIcon(QIcon::fromTheme(QStringLiteral("view-process-tree")));
    menu->addMenu(aggregationMenu);
    QObject::connect(aggregationMenu, SIGNAL(aboutToShow()), receiver, SLOT(aggregationMenuAboutToShow()));

    auto themeMenu = new QMenu(i18n("Theme"), menu);
    themeMenu->setIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop-theme")));
    menu->addMenu(themeMenu);
    QObject::connect(themeMenu, SIGNAL(aboutToShow()), receiver, SLOT(themeMenuAboutToShow()));
}

QString MessageList::Util::contentSummary(const Akonadi::Item &item)
{
    if (!item.hasPayload<KMime::Message::Ptr>()) {
        return {};
    }

    auto message = item.payload<KMime::Message::Ptr>();
    KMime::Content *textContent = message->textContent();
    if (!textContent) {
        return {};
    }
    const QString content = textContent->decodedText(true, true);
    if (content.isEmpty()) {
        return {};
    }

    // Extract the first 5 non-empty, non-quoted lines from the content and return it
    int numLines = 0;
    const int maxLines = 5;
    const QStringList lines = content.split(QLatin1Char('\n'));
    if (lines.isEmpty()) {
        return {};
    }
    if (lines.count() == 1 && content.length() > 100) {
        return content.left(100);
    }
    QString ret;
    for (const QString &line : lines) {
        const QString lineTrimmed = line.trimmed();
        const bool isQuoted = lineTrimmed.startsWith(QLatin1Char('>')) || lineTrimmed.startsWith(QLatin1Char('|'));
        if (!isQuoted && !lineTrimmed.isEmpty()) {
            ret += line + QLatin1Char('\n');
            numLines++;
            if (numLines >= maxLines) {
                break;
            }
        }
    }
    return ret.toHtmlEscaped();
}
