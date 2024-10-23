/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QDebug>
#include <QObject>
#include <QSet>
#include <QString>

#include "core/widgets/searchlinecommand.h"
#include "messagelist_export.h"
#include "widgets/quicksearchline.h"
#include <Akonadi/Collection>
#include <Akonadi/MessageStatus>
#include <KSharedConfig>

namespace MessageList
{
namespace Core
{
class MessageItem;

/**
 * This class is responsible of matching messages that should be displayed
 * in the View. It's used mainly by Model and Widget.
 */
class Filter : public QObject
{
    Q_OBJECT

public:
    explicit Filter(QObject *parent = nullptr);

public:
    /**
     * Returns true if the specified parameters match this filter and false otherwise.
     * The msg pointer must not be null.
     */
    [[nodiscard]] bool match(const MessageItem *item) const;

    /**
     * Returns the currently set status mask
     */
    [[nodiscard]] QList<Akonadi::MessageStatus> status() const;

    /**
     * Sets the status mask for this filter.
     */
    void setStatus(const QList<Akonadi::MessageStatus> &lstStatus);

    /**
     * Sets the current folder of this filter.
     */
    void setCurrentFolder(const Akonadi::Collection &collection);

    /**
     * Returns the currently set search string.
     */
    [[nodiscard]] const QString &searchString() const;

    /**
     * Sets the search string for this filter.
     */
    void setSearchString(const QString &search, SearchMessageByButtons::SearchOptions options);

    /**
     * Returns the currently set MessageItem::Tag id
     */
    [[nodiscard]] const QString &tagId() const;

    /**
     * Sets the id of a MessageItem::Tag that the matching messages must contain.
     */
    void setTagId(const QString &tagId);

    /**
     * Clears this filter (sets status to 0, search string and tag id to empty strings)
     */
    void clear();

    /**
     * Returns true if this filter is empty (0 status mask, empty search string and empty tag)
     * and it's useless to call match() that will always return true.
     */
    [[nodiscard]] bool isEmpty() const;

    [[nodiscard]] SearchMessageByButtons::SearchOptions currentOptions() const;

    void save(const KSharedConfig::Ptr &config, const QString &filtername, const QString &iconName, int numFilter = -1);
    [[nodiscard]] static Filter *load(const KSharedConfig::Ptr &config, int filternumber);
    void generateRandomIdentifier();
    [[nodiscard]] QString identifier() const;
    void setIdentifier(const QString &newIdentifier);

    [[nodiscard]] const QString &filterName() const;
    void setFilterName(const QString &newFilterName);

    void setOptions(SearchMessageByButtons::SearchOptions newOptions);

    [[nodiscard]] static Filter *loadFromConfigGroup(const KConfigGroup &newGroup);
    [[nodiscard]] const QString &iconName() const;
    void setIconName(const QString &newIconName);

    void setSearchString(const SearchLineCommand &command);
Q_SIGNALS:
    void finished();

private:
    [[nodiscard]] bool containString(const QString &searchInString) const;
    QList<Akonadi::MessageStatus> mStatus; ///< Messages must match these statuses, if non 0
    QString mSearchString; ///< Messages must match this search string, if not empty
    QString mTagId; ///< Messages must have this tag, if not empty. Contains a tag url.
    Akonadi::Collection mCurrentFolder;
    QSet<qint64> mMatchingItemIds;
    SearchMessageByButtons::SearchOptions mOptions;
    QStringList mSearchList;
    QString mIdentifier;
    QString mFilterName;
    QString mIconName;
};
} // namespace Core
} // namespace MessageList

MESSAGELIST_EXPORT QDebug operator<<(QDebug d, const MessageList::Core::Filter &t);
