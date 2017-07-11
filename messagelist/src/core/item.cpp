/******************************************************************************
 *
 *  Copyright 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/

#include "core/item.h"
#include "core/item_p.h"
#include "core/model.h"
#include "core/manager.h"

#include <kio/global.h> // for KIO::filesize_t and related functions
#include <kmime/kmime_dateformatter.h> // kdepimlibs

#include <KLocalizedString>

using namespace MessageList::Core;

/**
  * Returns a displayable string from the list of email @p addresses.
  * Inefficient, use KMime::Headers::*::displayString() directly instead.
  */
QString stripEmailAddr(const QString &aStr)
{
    //qCDebug(MESSAGECORE_LOG) << "(" << aStr << ")";

    if (aStr.isEmpty()) {
        return QString();
    }

    QString result;

    // The following is a primitive parser for a mailbox-list (cf. RFC 2822).
    // The purpose is to extract a displayable string from the mailboxes.
    // Comments in the addr-spec are not handled. No error checking is done.

    QString name;
    QString comment;
    QString angleAddress;
    enum { TopLevel, InComment, InAngleAddress } context = TopLevel;
    bool inQuotedString = false;
    int commentLevel = 0;

    QChar ch;
    int strLength(aStr.length());
    for (int index = 0; index < strLength; ++index) {
        ch = aStr[index];
        switch (context) {
        case TopLevel : {
            switch (ch.toLatin1()) {
            case '"' : inQuotedString = !inQuotedString;
                break;
            case '(' : if (!inQuotedString) {
                    context = InComment;
                    commentLevel = 1;
                } else {
                    name += ch;
                }
                break;
            case '<' : if (!inQuotedString) {
                    context = InAngleAddress;
                } else {
                    name += ch;
                }
                break;
            case '\\' : // quoted character
                ++index; // skip the '\'
                if (index < aStr.length()) {
                    name += aStr[index];
                }
                break;
            case ',' : if (!inQuotedString) {
                    // next email address
                    if (!result.isEmpty()) {
                        result += QLatin1String(", ");
                    }
                    name = name.trimmed();
                    comment = comment.trimmed();
                    angleAddress = angleAddress.trimmed();
                    if (angleAddress.isEmpty() && !comment.isEmpty()) {
                        // handle Outlook-style addresses like
                        // john.doe@invalid (John Doe)
                        result += comment;
                    } else if (!name.isEmpty()) {
                        result += name;
                    } else if (!comment.isEmpty()) {
                        result += comment;
                    } else if (!angleAddress.isEmpty()) {
                        result += angleAddress;
                    }
                    name.clear();
                    comment.clear();
                    angleAddress.clear();
                } else {
                    name += ch;
                }
                break;
            default :  name += ch;
            }
            break;
        }
        case InComment : {
            switch (ch.toLatin1()) {
            case '(' : ++commentLevel;
                comment += ch;
                break;
            case ')' : --commentLevel;
                if (commentLevel == 0) {
                    context = TopLevel;
                    comment += QLatin1Char(' '); // separate the text of several comments
                } else {
                    comment += ch;
                }
                break;
            case '\\' : // quoted character
                ++index; // skip the '\'
                if (index < aStr.length()) {
                    comment += aStr[index];
                }
                break;
            default :  comment += ch;
            }
            break;
        }
        case InAngleAddress : {
            switch (ch.toLatin1()) {
            case '"' : inQuotedString = !inQuotedString;
                angleAddress += ch;
                break;
            case '>' : if (!inQuotedString) {
                    context = TopLevel;
                } else {
                    angleAddress += ch;
                }
                break;
            case '\\' : // quoted character
                ++index; // skip the '\'
                if (index < aStr.length()) {
                    angleAddress += aStr[index];
                }
                break;
            default :  angleAddress += ch;
            }
            break;
        }
        } // switch ( context )
    }
    if (!result.isEmpty()) {
        result += QLatin1String(", ");
    }
    name = name.trimmed();
    comment = comment.trimmed();
    angleAddress = angleAddress.trimmed();
    if (angleAddress.isEmpty() && !comment.isEmpty()) {
        // handle Outlook-style addresses like
        // john.doe@invalid (John Doe)
        result += comment;
    } else if (!name.isEmpty()) {
        result += name;
    } else if (!comment.isEmpty()) {
        result += comment;
    } else if (!angleAddress.isEmpty()) {
        result += angleAddress;
    }

    //qCDebug(MESSAGECORE_LOG) << "Returns \"" << result << "\"";
    return result;
}

Item::Item(Type type)
    : d_ptr(new ItemPrivate(this))
{
    d_ptr->mType = type;
}

Item::Item(Item::Type type, ItemPrivate *dd)
    : d_ptr(dd)
{
    d_ptr->mType = type;
}

Item::~Item()
{
    killAllChildItems();

    if (d_ptr->mParent) {
        d_ptr->mParent->d_ptr->childItemDead(this);
    }

    delete d_ptr;
}

void Item::childItemStats(ChildItemStats &stats) const
{
    Q_ASSERT(d_ptr->mChildItems);

    stats.mTotalChildCount += d_ptr->mChildItems->count();
    for (const auto child : qAsConst(*d_ptr->mChildItems)) {
        if (!child->status().isRead()) {
            stats.mUnreadChildCount++;
        }
        if (child->d_ptr->mChildItems) {
            child->childItemStats(stats);
        }
    }
}

QList< Item * > *Item::childItems() const
{
    return d_ptr->mChildItems;
}

Item *Item::childItem(int idx) const
{
    if (idx < 0) {
        return nullptr;
    }
    if (!d_ptr->mChildItems) {
        return nullptr;
    }
    if (d_ptr->mChildItems->count() <= idx) {
        return nullptr;
    }
    return d_ptr->mChildItems->at(idx);
}

Item *Item::firstChildItem() const
{
    return d_ptr->mChildItems ? (d_ptr->mChildItems->count() > 0 ? d_ptr->mChildItems->at(0) : nullptr) : nullptr;
}

Item *Item::itemBelowChild(Item *child)
{
    Q_ASSERT(d_ptr->mChildItems);

    int idx = indexOfChildItem(child);
    Q_ASSERT(idx >= 0);

    idx++;

    if (idx < d_ptr->mChildItems->count()) {
        return d_ptr->mChildItems->at(idx);
    }

    if (!d_ptr->mParent) {
        return nullptr;
    }

    return d_ptr->mParent->itemBelowChild(this);
}

Item *Item::itemBelow()
{
    if (d_ptr->mChildItems) {
        if (!d_ptr->mChildItems->isEmpty()) {
            return d_ptr->mChildItems->at(0);
        }
    }

    if (!d_ptr->mParent) {
        return nullptr;
    }

    return d_ptr->mParent->itemBelowChild(this);
}

Item *Item::deepestItem()
{
    if (d_ptr->mChildItems) {
        if (!d_ptr->mChildItems->isEmpty()) {
            return d_ptr->mChildItems->at(d_ptr->mChildItems->count() - 1)->deepestItem();
        }
    }

    return this;
}

Item *Item::itemAboveChild(Item *child)
{
    if (d_ptr->mChildItems) {
        int idx = indexOfChildItem(child);
        Q_ASSERT(idx >= 0);
        idx--;

        if (idx >= 0) {
            return d_ptr->mChildItems->at(idx);
        }
    }

    return this;
}

Item *Item::itemAbove()
{
    if (!d_ptr->mParent) {
        return nullptr;
    }

    Item *siblingAbove = d_ptr->mParent->itemAboveChild(this);
    if (siblingAbove && siblingAbove != this && siblingAbove != d_ptr->mParent &&
            siblingAbove->childItemCount() > 0) {
        return siblingAbove->deepestItem();
    }

    return d_ptr->mParent->itemAboveChild(this);
}

int Item::childItemCount() const
{
    return d_ptr->mChildItems ? d_ptr->mChildItems->count() : 0;
}

bool Item::hasChildren() const
{
    return childItemCount() > 0;
}

int Item::indexOfChildItem(Item *child) const
{
    if (!d_ptr->mChildItems) {
        return -1;
    }
    int idx = child->d_ptr->mThisItemIndexGuess;
    if (idx < d_ptr->mChildItems->count() && d_ptr->mChildItems->at(idx) == child) {
        return idx;    // good guess
    }

    idx = d_ptr->mChildItems->indexOf(child);
    if (idx >= 0) {
        child->d_ptr->mThisItemIndexGuess = idx;
    }
    return idx;
}

void Item::setIndexGuess(int index)
{
    d_ptr->mThisItemIndexGuess = index;
}

Item *Item::topmostNonRoot()
{
    Q_ASSERT(d_ptr->mType != InvisibleRoot);

    if (!d_ptr->mParent) {
        return this;
    }

    if (d_ptr->mParent->type() == InvisibleRoot) {
        return this;
    }

    return d_ptr->mParent->topmostNonRoot();
}

static inline void append_string(QString &buffer, const QString &append)
{
    if (!buffer.isEmpty()) {
        buffer += QLatin1String(", ");
    }
    buffer += append;
}

QString Item::statusDescription() const
{
    QString ret;
    if (status().isRead()) {
        append_string(ret, i18nc("Status of an item", "Read"));
    } else {
        append_string(ret, i18nc("Status of an item", "Unread"));
    }

    if (status().hasAttachment()) {
        append_string(ret, i18nc("Status of an item", "Has Attachment"));
    }

    if (status().isToAct()) {
        append_string(ret, i18nc("Status of an item", "Action Item"));
    }

    if (status().isReplied()) {
        append_string(ret, i18nc("Status of an item", "Replied"));
    }

    if (status().isForwarded()) {
        append_string(ret, i18nc("Status of an item", "Forwarded"));
    }

    if (status().isSent()) {
        append_string(ret, i18nc("Status of an item", "Sent"));
    }

    if (status().isImportant()) {
        append_string(ret, i18nc("Status of an item", "Important"));
    }

    if (status().isSpam()) {
        append_string(ret, i18nc("Status of an item", "Spam"));
    }

    if (status().isHam()) {
        append_string(ret, i18nc("Status of an item", "Ham"));
    }

    if (status().isWatched()) {
        append_string(ret, i18nc("Status of an item", "Watched"));
    }

    if (status().isIgnored()) {
        append_string(ret, i18nc("Status of an item", "Ignored"));
    }

    return ret;
}

QString Item::formattedSize() const
{
    return KIO::convertSize((KIO::filesize_t) size());
}

QString Item::formattedDate() const
{
    if (static_cast< uint >(date()) == static_cast< uint >(-1)) {
        return Manager::instance()->cachedLocalizedUnknownText();
    } else {
        return Manager::instance()->dateFormatter()->dateString(date());
    }
}

QString Item::formattedMaxDate() const
{
    if (static_cast< uint >(maxDate()) == static_cast< uint >(-1)) {
        return Manager::instance()->cachedLocalizedUnknownText();
    } else {
        return Manager::instance()->dateFormatter()->dateString(maxDate());
    }
}

bool Item::recomputeMaxDate()
{
    time_t newMaxDate = d_ptr->mDate;

    if (d_ptr->mChildItems) {
        for (auto child : qAsConst(*d_ptr->mChildItems)) {
            if (child->d_ptr->mMaxDate > newMaxDate) {
                newMaxDate = child->d_ptr->mMaxDate;
            }
        }
    }

    if (newMaxDate != d_ptr->mMaxDate) {
        setMaxDate(newMaxDate);
        return true;
    }
    return false;
}

Item::Type Item::type() const
{
    return d_ptr->mType;
}

Item::InitialExpandStatus Item::initialExpandStatus() const
{
    return d_ptr->mInitialExpandStatus;
}

void Item::setInitialExpandStatus(InitialExpandStatus initialExpandStatus)
{
    d_ptr->mInitialExpandStatus = initialExpandStatus;
}

bool Item::isViewable() const
{
    return d_ptr->mIsViewable;
}

bool Item::hasAncestor(const Item *it) const
{
    return d_ptr->mParent ? (d_ptr->mParent == it ? true : d_ptr->mParent->hasAncestor(it)) : false;
}

void Item::setViewable(Model *model, bool bViewable)
{
    if (d_ptr->mIsViewable == bViewable) {
        return;
    }

    if (!d_ptr->mChildItems) {
        d_ptr->mIsViewable = bViewable;
        return;
    }

    if (d_ptr->mChildItems->isEmpty()) {
        d_ptr->mIsViewable = bViewable;
        return;
    }

    if (bViewable) {
        if (model) {
            // fake having no children, for a second
            QList< Item * > *tmp = d_ptr->mChildItems;
            d_ptr->mChildItems = nullptr;
            //qDebug("BEGIN INSERT ROWS FOR PARENT %x: from %d to %d, (will) have %d children",this,0,tmp->count()-1,tmp->count());
            model->beginInsertRows(model->index(this, 0), 0, tmp->count() - 1);
            d_ptr->mChildItems = tmp;
            d_ptr->mIsViewable = true;
            model->endInsertRows();
        } else {
            d_ptr->mIsViewable = true;
        }

        for (const auto child : qAsConst(*d_ptr->mChildItems)) {
            child->setViewable(model, bViewable);
        }
    } else {
        for (const auto child : qAsConst(*d_ptr->mChildItems)) {
            child->setViewable(model, bViewable);
        }

        // It seems that we can avoid removing child items here since the parent has been removed: this is a hack tough
        // and should check if Qt4 still supports it in the next (hopefully largely fixed) release

        if (model) {
            // fake having no children, for a second
            model->beginRemoveRows(model->index(this, 0), 0, d_ptr->mChildItems->count() - 1);
            QList< Item * > *tmp = d_ptr->mChildItems;
            d_ptr->mChildItems = nullptr;
            d_ptr->mIsViewable = false;
            model->endRemoveRows();
            d_ptr->mChildItems = tmp;
        } else {
            d_ptr->mIsViewable = false;
        }
    }
}

void Item::killAllChildItems()
{
    if (!d_ptr->mChildItems) {
        return;
    }

    while (!d_ptr->mChildItems->isEmpty()) {
        delete d_ptr->mChildItems->first();    // this will call childDead() which will remove the child from the list
    }

    delete d_ptr->mChildItems;
    d_ptr->mChildItems = nullptr;
}

Item *Item::parent() const
{
    return d_ptr->mParent;
}

void Item::setParent(Item *pParent)
{
    d_ptr->mParent = pParent;
}

const Akonadi::MessageStatus &Item::status() const
{
    return d_ptr->mStatus;
}

void Item::setStatus(Akonadi::MessageStatus status)
{
    d_ptr->mStatus = status;
}

size_t Item::size() const
{
    return d_ptr->mSize;
}

void Item::setSize(size_t size)
{
    d_ptr->mSize = size;
}

time_t Item::date() const
{
    return d_ptr->mDate;
}

void Item::setDate(time_t date)
{
    d_ptr->mDate = date;
}

time_t Item::maxDate() const
{
    return d_ptr->mMaxDate;
}

void Item::setMaxDate(time_t date)
{
    d_ptr->mMaxDate = date;
}

const QString &Item::sender() const
{
    return d_ptr->mSender;
}

void Item::setSender(const QString &sender)
{
    d_ptr->mSender = sender;
}

QString Item::displaySender() const
{
    return stripEmailAddr(sender());
}

const QString &Item::receiver() const
{
    return d_ptr->mReceiver;
}

void Item::setReceiver(const QString &receiver)
{
    d_ptr->mReceiver = receiver;
}

QString Item::displayReceiver() const
{
    return stripEmailAddr(receiver());
}

const QString &Item::senderOrReceiver() const
{
    return d_ptr->mUseReceiver ? d_ptr->mReceiver : d_ptr->mSender;
}

QString Item::displaySenderOrReceiver() const
{
    return stripEmailAddr(senderOrReceiver());
}

bool Item::useReceiver() const
{
    return d_ptr->mUseReceiver;
}

const QString &Item::subject() const
{
    return d_ptr->mSubject;
}

void Item::setSubject(const QString &subject)
{
    d_ptr->mSubject = subject;
}

void MessageList::Core::Item::initialSetup(time_t date, size_t size,
        const QString &sender,
        const QString &receiver,
        bool useReceiver)
{
    d_ptr->mDate = date;
    d_ptr->mMaxDate = date;
    d_ptr->mSize = size;
    d_ptr->mSender = sender;
    d_ptr->mReceiver = receiver;
    d_ptr->mUseReceiver = useReceiver;
}

void MessageList::Core::Item::setItemId(qint64 id)
{
    d_ptr->mItemId = id;
}

qint64 MessageList::Core::Item::itemId() const
{
    return d_ptr->mItemId;
}

void Item::setParentCollectionId(qint64 id)
{
    d_ptr->mParentCollectionId = id;
}

qint64 Item::parentCollectionId() const
{
    return d_ptr->mParentCollectionId;
}

void MessageList::Core::Item::setSubjectAndStatus(const QString &subject,
        Akonadi::MessageStatus status)
{
    d_ptr->mSubject = subject;
    d_ptr->mStatus = status;
}

// FIXME: Try to "cache item insertions" and call beginInsertRows() and endInsertRows() in a chunked fashion...

void Item::rawAppendChildItem(Item *child)
{
    if (!d_ptr->mChildItems) {
        d_ptr->mChildItems = new QList< Item * >();
    }
    d_ptr->mChildItems->append(child);
}

int Item::appendChildItem(Model *model, Item *child)
{
    if (!d_ptr->mChildItems) {
        d_ptr->mChildItems = new QList< Item * >();
    }
    const int idx = d_ptr->mChildItems->count();
    if (d_ptr->mIsViewable) {
        if (model) {
            model->beginInsertRows(model->index(this, 0), idx, idx);    // THIS IS EXTREMELY UGLY, BUT IT'S THE ONLY POSSIBLE WAY WITH QT4 AT THE TIME OF WRITING
        }
        d_ptr->mChildItems->append(child);
        child->setIndexGuess(idx);
        if (model) {
            model->endInsertRows();    // THIS IS EXTREMELY UGLY, BUT IT'S THE ONLY POSSIBLE WAY WITH QT4 AT THE TIME OF WRITING
        }
        child->setViewable(model, true);
    } else {
        d_ptr->mChildItems->append(child);
        child->setIndexGuess(idx);
    }
    return idx;
}

void Item::dump(const QString &prefix)
{
    QString out = QStringLiteral("%1 %x VIEWABLE:%2").arg(prefix, d_ptr->mIsViewable ? QStringLiteral("yes") : QStringLiteral("no"));
    qDebug(out.toUtf8().data(), this);

    QString nPrefix(prefix);
    nPrefix += QLatin1String("  ");

    if (!d_ptr->mChildItems) {
        return;
    }

    for (const auto child : qAsConst(*d_ptr->mChildItems)) {
        child->dump(nPrefix);
    }
}

void Item::takeChildItem(Model *model, Item *child)
{
    if (!d_ptr->mChildItems) {
        return;    // Ugh... not our child ?
    }

    if (!d_ptr->mIsViewable) {
        //qDebug("TAKING NON VIEWABLE CHILD ITEM %x",child);
        // We can highly optimize this case
        d_ptr->mChildItems->removeOne(child);
#if 0
        // This *could* be done, but we optimize and avoid it.
        if (d->mChildItems->isEmpty()) {
            delete d->mChildItems;
            d->mChildItems = 0;
        }
#endif
        child->setParent(nullptr);
        return;
    }

    const int idx = indexOfChildItem(child);
    if (idx < 0) {
        return;    // Aaargh... not our child ?
    }

    child->setViewable(model, false);
    if (model) {
        model->beginRemoveRows(model->index(this, 0), idx, idx);
    }
    child->setParent(nullptr);
    d_ptr->mChildItems->removeAt(idx);
    if (model) {
        model->endRemoveRows();
    }

#if 0
    // This *could* be done, but we optimize and avoid it.
    if (d->mChildItems->isEmpty()) {
        delete d->mChildItems;
        d->mChildItems = 0;
    }
#endif
}

void ItemPrivate::childItemDead(Item *child)
{
    // mChildItems MUST be non zero here, if it's not then it's a bug in THIS FILE
    mChildItems->removeOne(child);   // since we always have ONE (if we not, it's a bug)
}
