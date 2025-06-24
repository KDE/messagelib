/*
  This file is part of KMail, the KDE mail client.
  SPDX-FileCopyrightText: 2011-2025 Laurent Montel <montel@kde.org>

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
using namespace Qt::Literals::StringLiterals;

QString MessageList::Util::messageSortingConfigName()
{
    return u"MessageSorting"_s;
}

QString MessageList::Util::messageSortDirectionConfigName()
{
    return u"MessageSortDirection"_s;
}

QString MessageList::Util::groupSortingConfigName()
{
    return u"GroupSorting"_s;
}

QString MessageList::Util::groupSortDirectionConfigName()
{
    return u"GroupSortDirection"_s;
}

QString MessageList::Util::messageUniqueIdConfigName()
{
    return u"MessageUniqueIdForStorageModel%1"_s;
}

QString MessageList::Util::storageModelSortOrderGroup()
{
    return u"MessageListView::StorageModelSortOrder"_s;
}

QString MessageList::Util::storageModelThemesGroup()
{
    return u"MessageListView::StorageModelThemes"_s;
}

QString MessageList::Util::storageModelAggregationsGroup()
{
    return u"MessageListView::StorageModelAggregations"_s;
}

QString MessageList::Util::setForStorageModelConfigName()
{
    return u"SetForStorageModel%1"_s;
}

QString MessageList::Util::storageModelSelectedMessageGroup()
{
    return u"MessageListView::StorageModelSelectedMessages"_s;
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
    sortingMenu->setIcon(QIcon::fromTheme(u"view-sort-ascending"_s));
    menu->addMenu(sortingMenu);
    QObject::connect(sortingMenu, SIGNAL(aboutToShow()), receiver, SLOT(sortOrderMenuAboutToShow()));

    auto aggregationMenu = new QMenu(i18n("Aggregation"), menu);
    aggregationMenu->setIcon(QIcon::fromTheme(u"view-process-tree"_s));
    menu->addMenu(aggregationMenu);
    QObject::connect(aggregationMenu, SIGNAL(aboutToShow()), receiver, SLOT(aggregationMenuAboutToShow()));

    auto themeMenu = new QMenu(i18n("Theme"), menu);
    themeMenu->setIcon(QIcon::fromTheme(u"preferences-desktop-theme"_s));
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
    const QString content = textContent->decodedText(KMime::Content::TrimSpaces);
    if (content.isEmpty()) {
        return {};
    }

    // Extract the first 5 non-empty, non-quoted lines from the content and return it
    int numLines = 0;
    const int maxLines = 5;
    const QStringList lines = content.split(u'\n');
    if (lines.isEmpty()) {
        return {};
    }
    if (lines.count() == 1 && content.length() > 100) {
        return content.left(100);
    }
    QString ret;
    for (const QString &line : lines) {
        const QString lineTrimmed = line.trimmed();
        const bool isQuoted = lineTrimmed.startsWith(QLatin1Char('>')) || lineTrimmed.startsWith(u'|');
        if (!isQuoted && !lineTrimmed.isEmpty()) {
            ret += line + u'\n';
            numLines++;
            if (numLines >= maxLines) {
                break;
            }
        }
    }
    return ret.toHtmlEscaped();
}
