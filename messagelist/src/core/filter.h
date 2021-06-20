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

#include "messagelist_export.h"
#include "widgets/quicksearchline.h"
#include <Akonadi/KMime/MessageStatus>
#include <Collection>
#include <KSharedConfig>

namespace MessageList
{
namespace Core
{
class MessageItem;

/**
 * This class is responsable of matching messages that should be displayed
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
    Q_REQUIRED_RESULT bool match(const MessageItem *item) const;

    /**
     * Returns the currently set status mask
     */
    Q_REQUIRED_RESULT QVector<Akonadi::MessageStatus> status() const;

    /**
     * Sets the status mask for this filter.
     */
    void setStatus(const QVector<Akonadi::MessageStatus> &lstStatus);

    /**
     * Sets the current folder of this filter.
     */
    void setCurrentFolder(const Akonadi::Collection &collection);

    /**
     * Returns the currently set search string.
     */
    Q_REQUIRED_RESULT const QString &searchString() const;

    /**
     * Sets the search string for this filter.
     */
    void setSearchString(const QString &search, QuickSearchLine::SearchOptions options);

    /**
     * Returns the currently set MessageItem::Tag id
     */
    Q_REQUIRED_RESULT const QString &tagId() const;

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
    Q_REQUIRED_RESULT bool isEmpty() const;

    Q_REQUIRED_RESULT QuickSearchLine::SearchOptions currentOptions() const;

    void save(const KSharedConfig::Ptr &config, const QString &filtername, const QString &iconName);
    static Q_REQUIRED_RESULT Filter *load(const KSharedConfig::Ptr &config, int filternumber);
    void generateRandomIdentifier();
    Q_REQUIRED_RESULT QString identifier() const;
    void setIdentifier(const QString &newIdentifier);

    Q_REQUIRED_RESULT const QString &filterName() const;
    void setFilterName(const QString &newFilterName);

    void setOptions(QuickSearchLine::SearchOptions newOptions);

    static Q_REQUIRED_RESULT Filter *loadFromConfigGroup(const KConfigGroup &newGroup);
    Q_REQUIRED_RESULT const QString &iconName() const;
    void setIconName(const QString &newIconName);

Q_SIGNALS:
    void finished();

private:
    Q_REQUIRED_RESULT bool containString(const QString &searchInString) const;
    QVector<Akonadi::MessageStatus> mStatus; ///< Messages must match these statuses, if non 0
    QString mSearchString; ///< Messages must match this search string, if not empty
    QString mTagId; ///< Messages must have this tag, if not empty. Contains a tag url.
    Akonadi::Collection mCurrentFolder;
    QSet<qint64> mMatchingItemIds;
    QuickSearchLine::SearchOptions mOptions;
    QStringList mSearchList;
    QString mIdentifier;
    QString mFilterName;
    QString mIconName;
};
} // namespace Core
} // namespace MessageList

MESSAGELIST_EXPORT QDebug operator<<(QDebug d, const MessageList::Core::Filter &t);
