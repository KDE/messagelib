/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2009 Thomas McGuire <mcguire@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/
#include "core/sortorder.h"
using namespace Qt::Literals::StringLiterals;

#include "messagelistutil_p.h"

#include <KLocalizedString>

#include <QMetaEnum>

using namespace MessageList::Core;

SortOrder::SortOrder() = default;

SortOrder::GroupSorting SortOrder::groupSorting() const
{
    return mGroupSorting;
}

void SortOrder::setGroupSorting(SortOrder::GroupSorting gs)
{
    mGroupSorting = gs;
}

SortOrder::SortDirection SortOrder::groupSortDirection() const
{
    return mGroupSortDirection;
}

void SortOrder::setGroupSortDirection(SortOrder::SortDirection groupSortDirection)
{
    mGroupSortDirection = groupSortDirection;
}

SortOrder::MessageSorting SortOrder::messageSorting() const
{
    return mMessageSorting;
}

void SortOrder::setMessageSorting(SortOrder::MessageSorting ms)
{
    mMessageSorting = ms;
}

SortOrder::SortDirection SortOrder::messageSortDirection() const
{
    return mMessageSortDirection;
}

void SortOrder::setMessageSortDirection(SortOrder::SortDirection messageSortDirection)
{
    mMessageSortDirection = messageSortDirection;
}

QList<QPair<QString, int>> SortOrder::enumerateMessageSortingOptions(Aggregation::Threading t)
{
    QList<QPair<QString, int>> ret;
    ret.append({i18n("None (Storage Order)"), SortOrder::NoMessageSorting});
    ret.append({i18n("By Date/Time"), SortOrder::SortMessagesByDateTime});
    if (t != Aggregation::NoThreading) {
        ret.append({i18n("By Date/Time of Most Recent in Subtree"), SortOrder::SortMessagesByDateTimeOfMostRecent});
    }
    ret.append({i18n("By Sender"), SortOrder::SortMessagesBySender});
    ret.append({i18n("By Receiver"), SortOrder::SortMessagesByReceiver});
    ret.append({i18n("By Smart Sender/Receiver"), SortOrder::SortMessagesBySenderOrReceiver});
    ret.append({i18n("By Subject"), SortOrder::SortMessagesBySubject});
    ret.append({i18n("By Size"), SortOrder::SortMessagesBySize});
    ret.append({i18n("By Action Item Status"), SortOrder::SortMessagesByActionItemStatus});
    ret.append({i18n("By Unread Status"), SortOrder::SortMessagesByUnreadStatus});
    ret.append({i18n("By Important Status"), SortOrder::SortMessagesByImportantStatus});
    ret.append({i18n("By Attachment Status"), SortOrder::SortMessagesByAttachmentStatus});
    return ret;
}

QList<QPair<QString, int>> SortOrder::enumerateMessageSortDirectionOptions(MessageSorting ms)
{
    QList<QPair<QString, int>> ret;
    if (ms == SortOrder::NoMessageSorting) {
        return ret;
    }

    if ((ms == SortOrder::SortMessagesByDateTime) || (ms == SortOrder::SortMessagesByDateTimeOfMostRecent)) {
        ret.append({i18n("Least Recent on Top"), SortOrder::Ascending});
        ret.append({i18n("Most Recent on Top"), SortOrder::Descending});
        return ret;
    }

    ret.append({i18nc("Sort order for messages", "Ascending"), SortOrder::Ascending});
    ret.append({i18nc("Sort order for messages", "Descending"), SortOrder::Descending});
    return ret;
}

QList<QPair<QString, int>> SortOrder::enumerateGroupSortingOptions(Aggregation::Grouping g)
{
    QList<QPair<QString, int>> ret;
    if (g == Aggregation::NoGrouping) {
        return ret;
    }
    if ((g == Aggregation::GroupByDate) || (g == Aggregation::GroupByDateRange)) {
        ret.append({i18n("by Date/Time"), SortOrder::SortGroupsByDateTime});
    } else {
        ret.append({i18n("None (Storage Order)"), SortOrder::NoGroupSorting});
        ret.append({i18n("by Date/Time of Most Recent Message in Group"), SortOrder::SortGroupsByDateTimeOfMostRecent});
    }

    if (g == Aggregation::GroupBySenderOrReceiver) {
        ret.append({i18n("by Sender/Receiver"), SortOrder::SortGroupsBySenderOrReceiver});
    } else if (g == Aggregation::GroupBySender) {
        ret.append({i18n("by Sender"), SortOrder::SortGroupsBySender});
    } else if (g == Aggregation::GroupByReceiver) {
        ret.append({i18n("by Receiver"), SortOrder::SortGroupsByReceiver});
    }

    return ret;
}

QList<QPair<QString, int>> SortOrder::enumerateGroupSortDirectionOptions(Aggregation::Grouping g, GroupSorting gs)
{
    QList<QPair<QString, int>> ret;
    if (g == Aggregation::NoGrouping || gs == SortOrder::NoGroupSorting) {
        return ret;
    }

    if (gs == SortOrder::SortGroupsByDateTimeOfMostRecent) {
        ret.append({i18n("Least Recent on Top"), SortOrder::Ascending});
        ret.append({i18n("Most Recent on Top"), SortOrder::Descending});
        return ret;
    }
    ret.append({i18nc("Sort order for mail groups", "Ascending"), SortOrder::Ascending});
    ret.append({i18nc("Sort order for mail groups", "Descending"), SortOrder::Descending});
    return ret;
}

using OptionList = QList<QPair<QString, int>>;
static bool optionListHasOption(const OptionList &optionList, int optionValue, int defaultOptionValue)
{
    for (const auto &pair : optionList) {
        if (pair.second == optionValue) {
            return true;
        }
    }
    if (optionValue != defaultOptionValue) {
        return false;
    } else {
        return true;
    }
}

bool SortOrder::validForAggregation(const Aggregation *aggregation) const
{
    OptionList messageSortings = enumerateMessageSortingOptions(aggregation->threading());
    OptionList messageSortDirections = enumerateMessageSortDirectionOptions(mMessageSorting);
    OptionList groupSortings = enumerateGroupSortingOptions(aggregation->grouping());
    OptionList groupSortDirections = enumerateGroupSortDirectionOptions(aggregation->grouping(), mGroupSorting);
    SortOrder defaultSortOrder = defaultForAggregation(aggregation, SortOrder());
    bool messageSortingOk = optionListHasOption(messageSortings, mMessageSorting, defaultSortOrder.messageSorting());
    bool messageSortDirectionOk = optionListHasOption(messageSortDirections, mMessageSortDirection, defaultSortOrder.messageSortDirection());

    bool groupSortingOk = optionListHasOption(groupSortings, mGroupSorting, defaultSortOrder.groupSorting());
    bool groupSortDirectionOk = optionListHasOption(groupSortDirections, mGroupSortDirection, defaultSortOrder.groupSortDirection());
    return messageSortingOk && messageSortDirectionOk && groupSortingOk && groupSortDirectionOk;
}

SortOrder SortOrder::defaultForAggregation(const Aggregation *aggregation, SortOrder oldSortOrder)
{
    SortOrder newSortOrder;

    //
    // First check if we can adopt the message sorting and the message sort direction from
    // the old sort order. This is mostly true, except, for example, when the old message sorting
    // was "by most recent in subtree", and the aggregation doesn't use threading.
    //
    OptionList messageSortings = enumerateMessageSortingOptions(aggregation->threading());
    bool messageSortingOk = optionListHasOption(messageSortings, oldSortOrder.messageSorting(), SortOrder().messageSorting());
    bool messageSortDirectionOk = false;
    if (messageSortingOk) {
        const OptionList messageSortDirections = enumerateMessageSortDirectionOptions(oldSortOrder.messageSorting());
        messageSortDirectionOk = optionListHasOption(messageSortDirections, oldSortOrder.messageSortDirection(), SortOrder().messageSortDirection());
        newSortOrder.setMessageSorting(oldSortOrder.messageSorting());
    } else {
        newSortOrder.setMessageSorting(SortMessagesByDateTime);
    }
    if (messageSortDirectionOk) {
        newSortOrder.setMessageSortDirection(oldSortOrder.messageSortDirection());
    } else {
        newSortOrder.setMessageSortDirection(Descending);
    }

    //
    // Now set the group sorting and group sort direction, depending on the aggregation.
    //
    Aggregation::Grouping grouping = aggregation->grouping();
    if (grouping == Aggregation::GroupByDate || grouping == Aggregation::GroupByDateRange) {
        newSortOrder.setGroupSortDirection(Descending);
        newSortOrder.setGroupSorting(SortGroupsByDateTime);
    } else if (grouping == Aggregation::GroupByReceiver || grouping == Aggregation::GroupBySender || grouping == Aggregation::GroupBySenderOrReceiver) {
        newSortOrder.setGroupSortDirection(Descending);
        switch (grouping) {
        case Aggregation::GroupByReceiver:
            newSortOrder.setGroupSorting(SortGroupsByReceiver);
            break;
        case Aggregation::GroupBySender:
            newSortOrder.setGroupSorting(SortGroupsBySender);
            break;
        case Aggregation::GroupBySenderOrReceiver:
            newSortOrder.setGroupSorting(SortGroupsBySenderOrReceiver);
            break;
        default:
            break;
        }
    }

    return newSortOrder;
}

bool SortOrder::readConfigHelper(KConfigGroup &conf, const QString &id)
{
    if (!conf.hasKey(id + MessageList::Util::messageSortingConfigName())) {
        return false;
    }
    mMessageSorting = messageSortingForName(conf.readEntry(id + MessageList::Util::messageSortingConfigName()));
    mMessageSortDirection = sortDirectionForName(conf.readEntry(id + MessageList::Util::messageSortDirectionConfigName()));
    mGroupSorting = groupSortingForName(conf.readEntry(id + MessageList::Util::groupSortingConfigName()));
    mGroupSortDirection = sortDirectionForName(conf.readEntry(id + MessageList::Util::groupSortDirectionConfigName()));
    return true;
}

void SortOrder::readConfig(KConfigGroup &conf, const QString &storageId, bool *storageUsesPrivateSortOrder)
{
    SortOrder privateSortOrder;
    SortOrder globalSortOrder;
    globalSortOrder.readConfigHelper(conf, u"GlobalSortOrder"_s);
    *storageUsesPrivateSortOrder = privateSortOrder.readConfigHelper(conf, storageId);
    if (*storageUsesPrivateSortOrder) {
        *this = privateSortOrder;
    } else {
        *this = globalSortOrder;
    }
}

void SortOrder::writeConfig(KConfigGroup &conf, const QString &storageId, bool storageUsesPrivateSortOrder) const
{
    QString id = storageId;
    if (!storageUsesPrivateSortOrder) {
        id = u"GlobalSortOrder"_s;
        conf.deleteEntry(storageId + MessageList::Util::messageSortingConfigName());
        conf.deleteEntry(storageId + MessageList::Util::messageSortDirectionConfigName());
        conf.deleteEntry(storageId + MessageList::Util::groupSortingConfigName());
        conf.deleteEntry(storageId + MessageList::Util::groupSortDirectionConfigName());
    }

    conf.writeEntry(id + MessageList::Util::messageSortingConfigName(), nameForMessageSorting(mMessageSorting));
    conf.writeEntry(id + MessageList::Util::messageSortDirectionConfigName(), nameForSortDirection(mMessageSortDirection));
    conf.writeEntry(id + MessageList::Util::groupSortingConfigName(), nameForGroupSorting(mGroupSorting));
    conf.writeEntry(id + MessageList::Util::groupSortDirectionConfigName(), nameForSortDirection(mGroupSortDirection));
}

bool SortOrder::isValidMessageSorting(SortOrder::MessageSorting ms)
{
    switch (ms) {
    case SortOrder::NoMessageSorting:
    case SortOrder::SortMessagesByDateTime:
    case SortOrder::SortMessagesByDateTimeOfMostRecent:
    case SortOrder::SortMessagesBySenderOrReceiver:
    case SortOrder::SortMessagesBySender:
    case SortOrder::SortMessagesByReceiver:
    case SortOrder::SortMessagesBySubject:
    case SortOrder::SortMessagesBySize:
    case SortOrder::SortMessagesByActionItemStatus:
    case SortOrder::SortMessagesByUnreadStatus:
    case SortOrder::SortMessagesByImportantStatus:
    case SortOrder::SortMessagesByAttachmentStatus:
        // ok
        break;
    default:
        // b0rken
        return false;
    }

    return true;
}

const QString SortOrder::nameForSortDirection(SortDirection sortDirection)
{
    int index = staticMetaObject.indexOfEnumerator("SortDirection");
    return QLatin1StringView(staticMetaObject.enumerator(index).valueToKey(sortDirection));
}

const QString SortOrder::nameForMessageSorting(MessageSorting messageSorting)
{
    int index = staticMetaObject.indexOfEnumerator("MessageSorting");
    return QLatin1StringView(staticMetaObject.enumerator(index).valueToKey(messageSorting));
}

const QString SortOrder::nameForGroupSorting(GroupSorting groupSorting)
{
    int index = staticMetaObject.indexOfEnumerator("GroupSorting");
    return QLatin1StringView(staticMetaObject.enumerator(index).valueToKey(groupSorting));
}

SortOrder::SortDirection SortOrder::sortDirectionForName(const QString &name)
{
    int index = staticMetaObject.indexOfEnumerator("SortDirection");
    return static_cast<SortDirection>(staticMetaObject.enumerator(index).keyToValue(name.toLatin1().constData()));
}

SortOrder::MessageSorting SortOrder::messageSortingForName(const QString &name)
{
    int index = staticMetaObject.indexOfEnumerator("MessageSorting");
    return static_cast<MessageSorting>(staticMetaObject.enumerator(index).keyToValue(name.toLatin1().constData()));
}

SortOrder::GroupSorting SortOrder::groupSortingForName(const QString &name)
{
    int index = staticMetaObject.indexOfEnumerator("GroupSorting");
    return static_cast<GroupSorting>(staticMetaObject.enumerator(index).keyToValue(name.toLatin1().constData()));
}

#include "moc_sortorder.cpp"
