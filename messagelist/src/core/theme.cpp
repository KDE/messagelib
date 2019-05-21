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

#include "core/theme.h"

#include <QDataStream>
#include <QPixmap>
#include <QIcon>
#include <QStandardPaths>
#include <QApplication>

#include <KLocalizedString>
#include "messagelist_debug.h"

using namespace MessageList::Core;

//
// Theme versioning
//
// The themes simply have a DWORD version number attached.
// The earliest version we're able to load is 0x1013.
//
// Theme revision history:
//
//  Version Date introduced Description
// --------------------------------------------------------------------------------------------------------------
//  0x1013  08.11.2008      Initial theme version, introduced when this piece of code has been moved into trunk.
//  0x1014  12.11.2008      Added runtime column data: width and column visibility
//  0x1015  03.03.2009      Added icon size
//  0x1016  08.03.2009      Added support for sorting by New/Unread status
//  0x1017  16.08.2009      Added support for column icon
//  0x1018  17.01.2010      Added support for annotation icon
//  0x1019  13.07.2010      Added support for invitation icon
//
static const int gThemeCurrentVersion = 0x1019; // increase if you add new fields or change the meaning of some
// you don't need to change the values below, but you might want to add new ones
static const int gThemeMinimumSupportedVersion = 0x1013;
static const int gThemeMinimumVersionWithColumnRuntimeData = 0x1014;
static const int gThemeMinimumVersionWithIconSizeField = 0x1015;
static const int gThemeMinimumVersionWithSortingByUnreadStatusAllowed = 0x1016;
static const int gThemeMinimumVersionWithColumnIcon = 0x1017;
static const int gThemeMinimumVersionWithAnnotationIcon = 0x1018;
static const int gThemeMinimumVersionWithInvitationIcon = 0x1019;

// the default icon size
static const int gThemeDefaultIconSize = 16;

Theme::ContentItem::ContentItem(Type type)
    : mType(type)
    , mFlags(0)
{
}

Theme::ContentItem::ContentItem(const ContentItem &src)
    : mType(src.mType)
    , mFlags(src.mFlags)
    , mCustomColor(src.mCustomColor)
{
}

Theme::ContentItem::Type Theme::ContentItem::type() const
{
    return mType;
}

bool Theme::ContentItem::canBeDisabled() const
{
    return (static_cast< int >(mType) & CanBeDisabled) != 0;
}

bool Theme::ContentItem::canUseCustomColor() const
{
    return (static_cast< int >(mType) & CanUseCustomColor) != 0;
}

bool Theme::ContentItem::displaysText() const
{
    return (static_cast< int >(mType) & DisplaysText) != 0;
}

bool Theme::ContentItem::displaysLongText() const
{
    return (static_cast< int >(mType) & LongText) != 0;
}

bool Theme::ContentItem::isIcon() const
{
    return (static_cast< int >(mType) & IsIcon) != 0;
}

bool Theme::ContentItem::isClickable() const
{
    return (static_cast< int >(mType) & IsClickable) != 0;
}

bool Theme::ContentItem::isSpacer() const
{
    return (static_cast< int >(mType) & IsSpacer) != 0;
}

QString Theme::ContentItem::description(Type type)
{
    switch (type) {
    case Subject:
        return i18nc("Description of Type Subject", "Subject");
        break;
    case Date:
        return i18nc("Description of Type Date", "Date");
        break;
    case SenderOrReceiver:
        return i18n("Sender/Receiver");
        break;
    case Sender:
        return i18nc("Description of Type Sender", "Sender");
        break;
    case Receiver:
        return i18nc("Description of Type Receiver", "Receiver");
        break;
    case Size:
        return i18nc("Description of Type Size", "Size");
        break;
    case ReadStateIcon:
        return i18n("Unread/Read Icon");
        break;
    case AttachmentStateIcon:
        return i18n("Attachment Icon");
        break;
    case RepliedStateIcon:
        return i18n("Replied/Forwarded Icon");
        break;
    case CombinedReadRepliedStateIcon:
        return i18n("Combined New/Unread/Read/Replied/Forwarded Icon");
        break;
    case ActionItemStateIcon:
        return i18n("Action Item Icon");
        break;
    case ImportantStateIcon:
        return i18n("Important Icon");
        break;
    case GroupHeaderLabel:
        return i18n("Group Header Label");
        break;
    case SpamHamStateIcon:
        return i18n("Spam/Ham Icon");
        break;
    case WatchedIgnoredStateIcon:
        return i18n("Watched/Ignored Icon");
        break;
    case ExpandedStateIcon:
        return i18n("Group Header Expand/Collapse Icon");
        break;
    case EncryptionStateIcon:
        return i18n("Encryption State Icon");
        break;
    case SignatureStateIcon:
        return i18n("Signature State Icon");
        break;
    case VerticalLine:
        return i18n("Vertical Separation Line");
        break;
    case HorizontalSpacer:
        return i18n("Horizontal Spacer");
        break;
    case MostRecentDate:
        return i18n("Max Date");
        break;
    case TagList:
        return i18n("Message Tags");
        break;
    case AnnotationIcon:
        return i18n("Note Icon");
    case InvitationIcon:
        return i18n("Invitation Icon");
    default:
        return i18nc("Description for an Unknown Type", "Unknown");
        break;
    }
}

bool Theme::ContentItem::useCustomColor() const
{
    return mFlags & UseCustomColor;
}

void Theme::ContentItem::setUseCustomColor(bool useCustomColor)
{
    if (useCustomColor) {
        mFlags |= UseCustomColor;
    } else {
        mFlags &= ~UseCustomColor;
    }
}

bool Theme::ContentItem::isBold() const
{
    return mFlags & IsBold;
}

void Theme::ContentItem::setBold(bool isBold)
{
    if (isBold) {
        mFlags |= IsBold;
    } else {
        mFlags &= ~IsBold;
    }
}

bool Theme::ContentItem::isItalic() const
{
    return mFlags & IsItalic;
}

void Theme::ContentItem::setItalic(bool isItalic)
{
    if (isItalic) {
        mFlags |= IsItalic;
    } else {
        mFlags &= ~IsItalic;
    }
}

bool Theme::ContentItem::hideWhenDisabled() const
{
    return mFlags & HideWhenDisabled;
}

void Theme::ContentItem::setHideWhenDisabled(bool hideWhenDisabled)
{
    if (hideWhenDisabled) {
        mFlags |= HideWhenDisabled;
    } else {
        mFlags &= ~HideWhenDisabled;
    }
}

bool Theme::ContentItem::softenByBlendingWhenDisabled() const
{
    return mFlags & SoftenByBlendingWhenDisabled;
}

void Theme::ContentItem::setSoftenByBlendingWhenDisabled(bool softenByBlendingWhenDisabled)
{
    if (softenByBlendingWhenDisabled) {
        mFlags |= SoftenByBlendingWhenDisabled;
    } else {
        mFlags &= ~SoftenByBlendingWhenDisabled;
    }
}

bool Theme::ContentItem::softenByBlending() const
{
    return mFlags & SoftenByBlending;
}

void Theme::ContentItem::setSoftenByBlending(bool softenByBlending)
{
    if (softenByBlending) {
        mFlags |= SoftenByBlending;
    } else {
        mFlags &= ~SoftenByBlending;
    }
}

const QColor &Theme::ContentItem::customColor() const
{
    return mCustomColor;
}

void Theme::ContentItem::setCustomColor(const QColor &clr)
{
    mCustomColor = clr;
}

bool Theme::ContentItem::applicableToMessageItems(Type type)
{
    return static_cast< int >(type) & ApplicableToMessageItems;
}

bool Theme::ContentItem::applicableToGroupHeaderItems(Type type)
{
    return static_cast< int >(type) & ApplicableToGroupHeaderItems;
}

void Theme::ContentItem::save(QDataStream &stream) const
{
    stream << (int)mType;
    stream << mFlags;
    stream << mCustomColor;
}

bool Theme::ContentItem::load(QDataStream &stream, int /*themeVersion*/)
{
    int val;

    stream >> val;
    mType = static_cast< Type >(val);
    switch (mType) {
    case Subject:
    case Date:
    case SenderOrReceiver:
    case Sender:
    case Receiver:
    case Size:
    case ReadStateIcon:
    case AttachmentStateIcon:
    case RepliedStateIcon:
    case GroupHeaderLabel:
    case ActionItemStateIcon:
    case ImportantStateIcon:
    case SpamHamStateIcon:
    case WatchedIgnoredStateIcon:
    case ExpandedStateIcon:
    case EncryptionStateIcon:
    case SignatureStateIcon:
    case VerticalLine:
    case HorizontalSpacer:
    case MostRecentDate:
    case CombinedReadRepliedStateIcon:
    case TagList:
    case AnnotationIcon:
    case InvitationIcon:
        // ok
        break;
    default:
        qCDebug(MESSAGELIST_LOG) << "Invalid content item type";
        return false; // b0rken
        break;
    }

    stream >> mFlags;
    stream >> mCustomColor;
    if (mFlags & UseCustomColor) {
        if (!mCustomColor.isValid()) {
            mFlags &= ~UseCustomColor;
        }
    }
    return true;
}

Theme::Row::Row()
{
}

Theme::Row::Row(const Row &src)
{
    for (const auto ci : qAsConst(src.mLeftItems)) {
        addLeftItem(new ContentItem(*ci));
    }

    for (const auto ci : qAsConst(src.mRightItems)) {
        addRightItem(new ContentItem(*ci));
    }
}

Theme::Row::~Row()
{
    removeAllLeftItems();
    removeAllRightItems();
}

void Theme::Row::removeAllLeftItems()
{
    while (!mLeftItems.isEmpty()) {
        delete mLeftItems.takeFirst();
    }
}

void Theme::Row::addLeftItem(Theme::ContentItem *item)
{
    mLeftItems.append(item);
}

void Theme::Row::removeAllRightItems()
{
    while (!mRightItems.isEmpty()) {
        delete mRightItems.takeFirst();
    }
}

void Theme::Row::addRightItem(Theme::ContentItem *item)
{
    mRightItems.append(item);
}

void Theme::Row::insertLeftItem(int idx, ContentItem *item)
{
    if (idx >= mLeftItems.count()) {
        mLeftItems.append(item);
        return;
    }
    mLeftItems.insert(idx, item);
}

void Theme::Row::removeLeftItem(Theme::ContentItem *item)
{
    mLeftItems.removeAll(item);
}

const QList<Theme::ContentItem *> &Theme::Row::rightItems() const
{
    return mRightItems;
}

void Theme::Row::insertRightItem(int idx, ContentItem *item)
{
    if (idx >= mRightItems.count()) {
        mRightItems.append(item);
        return;
    }
    mRightItems.insert(idx, item);
}

void Theme::Row::removeRightItem(Theme::ContentItem *item)
{
    mRightItems.removeAll(item);
}

bool Theme::Row::containsTextItems() const
{
    for (const auto ci : qAsConst(mLeftItems)) {
        if (ci->displaysText()) {
            return true;
        }
    }
    for (const auto ci : qAsConst(mRightItems)) {
        if (ci->displaysText()) {
            return true;
        }
    }
    return false;
}

void Theme::Row::save(QDataStream &stream) const
{
    stream << (int)mLeftItems.count();

    int cnt = mLeftItems.count();

    for (int i = 0; i < cnt; ++i) {
        ContentItem *ci = mLeftItems.at(i);
        ci->save(stream);
    }

    stream << (int)mRightItems.count();

    cnt = mRightItems.count();

    for (int i = 0; i < cnt; ++i) {
        ContentItem *ci = mRightItems.at(i);
        ci->save(stream);
    }
}

bool Theme::Row::LoadContentItem(int val, QDataStream &stream, int themeVersion, bool leftItem)
{
    if ((val < 0) || (val > 50)) {
        return false;    // senseless
    }

    // FIXME: Remove code duplication here

    for (int i = 0; i < val; ++i) {
        ContentItem *ci = new ContentItem(ContentItem::Subject);    // dummy type
        if (!ci->load(stream, themeVersion)) {
            qCDebug(MESSAGELIST_LOG) << "Left content item loading failed";
            delete ci;
            return false;
        }
        if (leftItem) {
            addLeftItem(ci);
        } else {
            addRightItem(ci);
        }

        // Add the annotation item next to the attachment icon, so that users upgrading from old
        // versions don't manually need to set this.
        // Don't do this for the stand-alone attachment column.
        if (ci->type() == ContentItem::AttachmentStateIcon
            && themeVersion < gThemeMinimumVersionWithAnnotationIcon
            && val > 1) {
            qCDebug(MESSAGELIST_LOG) << "Old theme version detected, adding annotation item next to attachment icon.";
            ContentItem *annotationItem = new ContentItem(ContentItem::AnnotationIcon);
            annotationItem->setHideWhenDisabled(true);
            if (leftItem) {
                addLeftItem(annotationItem);
            } else {
                addRightItem(annotationItem);
            }
        }

        // Same as above, for the invitation icon
        if (ci->type() == ContentItem::AttachmentStateIcon
            && themeVersion < gThemeMinimumVersionWithInvitationIcon
            && val > 1) {
            qCDebug(MESSAGELIST_LOG) << "Old theme version detected, adding invitation item next to attachment icon.";
            ContentItem *invitationItem = new ContentItem(ContentItem::InvitationIcon);
            invitationItem->setHideWhenDisabled(true);
            if (leftItem) {
                addLeftItem(invitationItem);
            } else {
                addRightItem(invitationItem);
            }
        }
    }
    return true;
}

const QList<Theme::ContentItem *> &Theme::Row::leftItems() const
{
    return mLeftItems;
}

bool Theme::Row::load(QDataStream &stream, int themeVersion)
{
    removeAllLeftItems();
    removeAllRightItems();

    int val;

    // left item count

    stream >> val;
    if (!LoadContentItem(val, stream, themeVersion, true)) {
        return false;
    }

    // right item count

    stream >> val;

    if (!LoadContentItem(val, stream, themeVersion, false)) {
        return false;
    }

    return true;
}

Theme::Column::SharedRuntimeData::SharedRuntimeData(bool currentlyVisible, double currentWidth)
    : mReferences(0)
    , mCurrentlyVisible(currentlyVisible)
    , mCurrentWidth(currentWidth)
{
}

Theme::Column::SharedRuntimeData::~SharedRuntimeData()
{
}

void Theme::Column::SharedRuntimeData::addReference()
{
    mReferences++;
}

bool Theme::Column::SharedRuntimeData::deleteReference()
{
    mReferences--;
    Q_ASSERT(mReferences >= 0);
    return mReferences > 0;
}

int Theme::Column::SharedRuntimeData::referenceCount() const
{
    return mReferences;
}

bool Theme::Column::SharedRuntimeData::currentlyVisible() const
{
    return mCurrentlyVisible;
}

void Theme::Column::SharedRuntimeData::setCurrentlyVisible(bool visible)
{
    mCurrentlyVisible = visible;
}

double Theme::Column::SharedRuntimeData::currentWidth() const
{
    return mCurrentWidth;
}

void Theme::Column::SharedRuntimeData::setCurrentWidth(double currentWidth)
{
    mCurrentWidth = currentWidth;
}

void Theme::Column::SharedRuntimeData::save(QDataStream &stream) const
{
    stream << mCurrentlyVisible;
    stream << mCurrentWidth;
}

bool Theme::Column::SharedRuntimeData::load(QDataStream &stream, int /* themeVersion */)
{
    stream >> mCurrentlyVisible;
    stream >> mCurrentWidth;
    if (mCurrentWidth > 10000) {
        qCDebug(MESSAGELIST_LOG) << "Theme has insane column width " << mCurrentWidth << " chopping to 100";
        mCurrentWidth = 100; // avoid really insane values
    }
    return mCurrentWidth >= -1;
}

Theme::Column::Column()
    : mVisibleByDefault(true)
    , mIsSenderOrReceiver(false)
    , mMessageSorting(SortOrder::NoMessageSorting)
{
    mSharedRuntimeData = new SharedRuntimeData(true, -1);
    mSharedRuntimeData->addReference();
}

Theme::Column::Column(const Column &src)
{
    mLabel = src.mLabel;
    mPixmapName = src.mPixmapName;
    mVisibleByDefault = src.mVisibleByDefault;
    mIsSenderOrReceiver = src.mIsSenderOrReceiver;
    mMessageSorting = src.mMessageSorting;

    mSharedRuntimeData = src.mSharedRuntimeData;
    mSharedRuntimeData->addReference();
    for (const auto row : qAsConst(src.mMessageRows)) {
        addMessageRow(new Row(*row));
    }

    for (const auto row : qAsConst(src.mGroupHeaderRows)) {
        addGroupHeaderRow(new Row(*row));
    }
}

Theme::Column::~Column()
{
    removeAllMessageRows();
    removeAllGroupHeaderRows();
    if (!(mSharedRuntimeData->deleteReference())) {
        delete mSharedRuntimeData;
    }
}

const QString &Theme::Column::label() const
{
    return mLabel;
}

void Theme::Column::setLabel(const QString &label)
{
    mLabel = label;
}

const QString &Theme::Column::pixmapName() const
{
    return mPixmapName;
}

void Theme::Column::setPixmapName(const QString &pixmapName)
{
    mPixmapName = pixmapName;
}

bool Theme::Column::isSenderOrReceiver() const
{
    return mIsSenderOrReceiver;
}

void Theme::Column::setIsSenderOrReceiver(bool sor)
{
    mIsSenderOrReceiver = sor;
}

bool Theme::Column::visibleByDefault() const
{
    return mVisibleByDefault;
}

void Theme::Column::setVisibleByDefault(bool vbd)
{
    mVisibleByDefault = vbd;
}

void Theme::Column::detach()
{
    if (mSharedRuntimeData->referenceCount() < 2) {
        return;    // nothing to detach
    }
    mSharedRuntimeData->deleteReference();

    mSharedRuntimeData = new SharedRuntimeData(mVisibleByDefault, -1);
    mSharedRuntimeData->addReference();
}

SortOrder::MessageSorting Theme::Column::messageSorting() const
{
    return mMessageSorting;
}

void Theme::Column::setMessageSorting(SortOrder::MessageSorting ms)
{
    mMessageSorting = ms;
}

bool Theme::Column::currentlyVisible() const
{
    return mSharedRuntimeData->currentlyVisible();
}

void Theme::Column::setCurrentlyVisible(bool currentlyVisible)
{
    mSharedRuntimeData->setCurrentlyVisible(currentlyVisible);
}

double Theme::Column::currentWidth() const
{
    return mSharedRuntimeData->currentWidth();
}

void Theme::Column::setCurrentWidth(double currentWidth)
{
    mSharedRuntimeData->setCurrentWidth(currentWidth);
}

const QList<Theme::Row *> &Theme::Column::messageRows() const
{
    return mMessageRows;
}

void Theme::Column::removeAllMessageRows()
{
    while (!mMessageRows.isEmpty()) {
        delete mMessageRows.takeFirst();
    }
}

void Theme::Column::addMessageRow(Theme::Row *row)
{
    mMessageRows.append(row);
}

void Theme::Column::removeAllGroupHeaderRows()
{
    while (!mGroupHeaderRows.isEmpty()) {
        delete mGroupHeaderRows.takeFirst();
    }
}

void Theme::Column::addGroupHeaderRow(Theme::Row *row)
{
    mGroupHeaderRows.append(row);
}

void Theme::Column::insertMessageRow(int idx, Row *row)
{
    if (idx >= mMessageRows.count()) {
        mMessageRows.append(row);
        return;
    }
    mMessageRows.insert(idx, row);
}

void Theme::Column::removeMessageRow(Theme::Row *row)
{
    mMessageRows.removeAll(row);
}

const QList<Theme::Row *> &Theme::Column::groupHeaderRows() const
{
    return mGroupHeaderRows;
}

void Theme::Column::insertGroupHeaderRow(int idx, Row *row)
{
    if (idx >= mGroupHeaderRows.count()) {
        mGroupHeaderRows.append(row);
        return;
    }
    mGroupHeaderRows.insert(idx, row);
}

void Theme::Column::removeGroupHeaderRow(Theme::Row *row)
{
    mGroupHeaderRows.removeAll(row);
}

bool Theme::Column::containsTextItems() const
{
    for (const auto row : qAsConst(mMessageRows)) {
        if (row->containsTextItems()) {
            return true;
        }
    }
    for (const auto row : qAsConst(mGroupHeaderRows)) {
        if (row->containsTextItems()) {
            return true;
        }
    }
    return false;
}

void Theme::Column::save(QDataStream &stream) const
{
    stream << mLabel;
    stream << mPixmapName;
    stream << mVisibleByDefault;
    stream << mIsSenderOrReceiver;
    stream << static_cast<int>(mMessageSorting);

    stream << static_cast<int>(mGroupHeaderRows.count());

    int cnt = mGroupHeaderRows.count();

    for (int i = 0; i < cnt; ++i) {
        Row *row = mGroupHeaderRows.at(i);
        row->save(stream);
    }

    cnt = mMessageRows.count();
    stream << static_cast<int>(cnt);

    for (int i = 0; i < cnt; ++i) {
        Row *row = mMessageRows.at(i);
        row->save(stream);
    }

    // added in version 0x1014
    mSharedRuntimeData->save(stream);
}

bool Theme::Column::load(QDataStream &stream, int themeVersion)
{
    removeAllGroupHeaderRows();
    removeAllMessageRows();

    stream >> mLabel;

    if (themeVersion >= gThemeMinimumVersionWithColumnIcon) {
        stream >> mPixmapName;
    }

    stream >> mVisibleByDefault;
    stream >> mIsSenderOrReceiver;

    int val;

    stream >> val;
    mMessageSorting = static_cast< SortOrder::MessageSorting >(val);
    if (!SortOrder::isValidMessageSorting(mMessageSorting)) {
        qCDebug(MESSAGELIST_LOG) << "Invalid message sorting";
        return false;
    }

    if (themeVersion < gThemeMinimumVersionWithSortingByUnreadStatusAllowed) {
        // The default "Classic" theme "Unread" column had sorting disabled here.
        // We want to be nice to the existing users and automatically set
        // the new sorting method for this column (so they don't have to make the
        // complex steps to set it by themselves).
        // This piece of code isn't strictly required: it's just a niceness :)
        if ((mMessageSorting == SortOrder::NoMessageSorting) && (mLabel == i18n("Unread"))) {
            mMessageSorting = SortOrder::SortMessagesByUnreadStatus;
        }
    }

    // group header row count
    stream >> val;

    if ((val < 0) || (val > 50)) {
        qCDebug(MESSAGELIST_LOG) << "Invalid group header row count";
        return false; // senseless
    }

    for (int i = 0; i < val; ++i) {
        Row *row = new Row();
        if (!row->load(stream, themeVersion)) {
            qCDebug(MESSAGELIST_LOG) << "Group header row loading failed";
            delete row;
            return false;
        }
        addGroupHeaderRow(row);
    }

    // message row count
    stream >> val;

    if ((val < 0) || (val > 50)) {
        qCDebug(MESSAGELIST_LOG) << "Invalid message row count";
        return false; // senseless
    }

    for (int i = 0; i < val; ++i) {
        Row *row = new Row();
        if (!row->load(stream, themeVersion)) {
            qCDebug(MESSAGELIST_LOG) << "Message row loading failed";
            delete row;
            return false;
        }
        addMessageRow(row);
    }

    if (themeVersion >= gThemeMinimumVersionWithColumnRuntimeData) {
        // starting with version 0x1014 we have runtime data too
        if (!mSharedRuntimeData->load(stream, themeVersion)) {
            qCDebug(MESSAGELIST_LOG) << "Shared runtime data loading failed";
            return false;
        }
    } else {
        // assume default shared data
        mSharedRuntimeData->setCurrentlyVisible(mVisibleByDefault);
        mSharedRuntimeData->setCurrentWidth(-1);
    }

    return true;
}

Theme::Theme()
    : OptionSet()
{
    mGroupHeaderBackgroundMode = AutoColor;
    mViewHeaderPolicy = ShowHeaderAlways;
    mIconSize = gThemeDefaultIconSize;
    mGroupHeaderBackgroundStyle = StyledJoinedRect;
}

Theme::Theme(const QString &name, const QString &description, bool readOnly)
    : OptionSet(name, description, readOnly)
{
    mGroupHeaderBackgroundMode = AutoColor;
    mGroupHeaderBackgroundStyle = StyledJoinedRect;
    mViewHeaderPolicy = ShowHeaderAlways;
    mIconSize = gThemeDefaultIconSize;
}

Theme::Theme(const Theme &src)
    : OptionSet(src)
{
    mGroupHeaderBackgroundMode = src.mGroupHeaderBackgroundMode;
    mGroupHeaderBackgroundColor = src.mGroupHeaderBackgroundColor;
    mGroupHeaderBackgroundStyle = src.mGroupHeaderBackgroundStyle;
    mViewHeaderPolicy = src.mViewHeaderPolicy;
    mIconSize = src.mIconSize;
    for (const auto col : qAsConst(src.mColumns)) {
        addColumn(new Column(*col));
    }
}

Theme::~Theme()
{
    clearPixmapCache();
    removeAllColumns();
}

void Theme::detach()
{
    for (const auto col : qAsConst(mColumns)) {
        col->detach();
    }
}

void Theme::resetColumnState()
{
    for (const auto col : qAsConst(mColumns)) {
        col->setCurrentlyVisible(col->visibleByDefault());
        col->setCurrentWidth(-1);
    }
}

void Theme::resetColumnSizes()
{
    for (const auto col : qAsConst(mColumns)) {
        col->setCurrentWidth(-1);
    }
}

const QList<Theme::Column *> &Theme::columns() const
{
    return mColumns;
}

Theme::Column *Theme::column(int idx) const
{
    return mColumns.count() > idx ? mColumns.at(idx) : nullptr;
}

void Theme::removeAllColumns()
{
    while (!mColumns.isEmpty()) {
        delete mColumns.takeFirst();
    }
}

void Theme::addColumn(Theme::Column *column)
{
    mColumns.append(column);
}

void Theme::insertColumn(int idx, Column *column)
{
    if (idx >= mColumns.count()) {
        mColumns.append(column);
        return;
    }
    mColumns.insert(idx, column);
}

void Theme::removeColumn(Theme::Column *col)
{
    mColumns.removeAll(col);
}

Theme::GroupHeaderBackgroundMode Theme::groupHeaderBackgroundMode() const
{
    return mGroupHeaderBackgroundMode;
}

void Theme::moveColumn(int idx, int newPosition)
{
    if ((newPosition >= mColumns.count()) || newPosition < 0) {
        return;
    }
    mColumns.move(idx, newPosition);
}

void Theme::setGroupHeaderBackgroundMode(GroupHeaderBackgroundMode bm)
{
    mGroupHeaderBackgroundMode = bm;
    if ((bm == CustomColor) && !mGroupHeaderBackgroundColor.isValid()) {
        mGroupHeaderBackgroundColor = QColor(127, 127, 127);    // something neutral
    }
}

const QColor &Theme::groupHeaderBackgroundColor() const
{
    return mGroupHeaderBackgroundColor;
}

void Theme::setGroupHeaderBackgroundColor(const QColor &clr)
{
    mGroupHeaderBackgroundColor = clr;
}

Theme::GroupHeaderBackgroundStyle Theme::groupHeaderBackgroundStyle() const
{
    return mGroupHeaderBackgroundStyle;
}

void Theme::setGroupHeaderBackgroundStyle(Theme::GroupHeaderBackgroundStyle groupHeaderBackgroundStyle)
{
    mGroupHeaderBackgroundStyle = groupHeaderBackgroundStyle;
}

QList<QPair<QString, int> > Theme::enumerateViewHeaderPolicyOptions()
{
    return { {
        i18n("Never Show"), NeverShowHeader
    },
        {
            i18n("Always Show"), ShowHeaderAlways
        } };
}

QList<QPair<QString, int> > Theme::enumerateGroupHeaderBackgroundStyles()
{
    return { {
        i18n("Plain Rectangles"), PlainRect
    },
        {
            i18n("Plain Joined Rectangle"), PlainJoinedRect
        },
        {
            i18n("Rounded Rectangles"), RoundedRect
        },
        {
            i18n("Rounded Joined Rectangle"), RoundedJoinedRect
        },
        {
            i18n("Gradient Rectangles"), GradientRect
        },
        {
            i18n("Gradient Joined Rectangle"), GradientJoinedRect
        },
        {
            i18n("Styled Rectangles"), StyledRect
        },
        {
            i18n("Styled Joined Rectangles"), StyledJoinedRect
        } };
}

Theme::ViewHeaderPolicy Theme::viewHeaderPolicy() const
{
    return mViewHeaderPolicy;
}

void Theme::setViewHeaderPolicy(Theme::ViewHeaderPolicy vhp)
{
    mViewHeaderPolicy = vhp;
}

int Theme::iconSize() const
{
    return mIconSize;
}

void Theme::setIconSize(int iconSize)
{
    if (mIconSize != iconSize) {
        clearPixmapCache();

        mIconSize = iconSize;
        if ((mIconSize < 8) || (mIconSize > 64)) {
            mIconSize = gThemeDefaultIconSize;
        }
    }
}

bool Theme::load(QDataStream &stream)
{
    removeAllColumns();

    int themeVersion;

    stream >> themeVersion;

    // We support themes starting at version gThemeMinimumSupportedVersion (0x1013 actually)

    if (
        (themeVersion > gThemeCurrentVersion)
        || (themeVersion < gThemeMinimumSupportedVersion)
        ) {
        qCDebug(MESSAGELIST_LOG) << "Invalid theme version";
        return false; // b0rken (invalid version)
    }

    int val;

    stream >> val;
    mGroupHeaderBackgroundMode = static_cast<GroupHeaderBackgroundMode>(val);
    switch (mGroupHeaderBackgroundMode) {
    case Transparent:
    case AutoColor:
    case CustomColor:
        // ok
        break;
    default:
        qCDebug(MESSAGELIST_LOG) << "Invalid theme group header background mode";
        return false; // b0rken
    }

    stream >> mGroupHeaderBackgroundColor;

    stream >> val;
    mGroupHeaderBackgroundStyle = static_cast<GroupHeaderBackgroundStyle>(val);
    switch (mGroupHeaderBackgroundStyle) {
    case PlainRect:
    case PlainJoinedRect:
    case RoundedRect:
    case RoundedJoinedRect:
    case GradientRect:
    case GradientJoinedRect:
    case StyledRect:
    case StyledJoinedRect:
        // ok
        break;
    default:
        qCDebug(MESSAGELIST_LOG) << "Invalid theme group header background style";
        return false; // b0rken
    }

    stream >> val;
    mViewHeaderPolicy = (ViewHeaderPolicy)val;
    switch (mViewHeaderPolicy) {
    case ShowHeaderAlways:
    case NeverShowHeader:
        // ok
        break;
    default:
        qCDebug(MESSAGELIST_LOG) << "Invalid theme view header policy";
        return false; // b0rken
    }

    if (themeVersion >= gThemeMinimumVersionWithIconSizeField) {
        // icon size parameter
        stream >> mIconSize;
        if ((mIconSize < 8) || (mIconSize > 64)) {
            mIconSize = gThemeDefaultIconSize;    // limit insane values
        }
    } else {
        mIconSize = gThemeDefaultIconSize;
    }

    // column count
    stream >> val;
    if (val < 1 || val > 50) {
        return false;    // plain b0rken ( negative, zero or more than 50 columns )
    }

    for (int i = 0; i < val; ++i) {
        Column *col = new Column();
        if (!col->load(stream, themeVersion)) {
            qCDebug(MESSAGELIST_LOG) << "Column loading failed";
            delete col;
            return false;
        }
        addColumn(col);
    }

    return true;
}

void Theme::save(QDataStream &stream) const
{
    stream << (int)gThemeCurrentVersion;

    stream << (int)mGroupHeaderBackgroundMode;
    stream << mGroupHeaderBackgroundColor;
    stream << (int)mGroupHeaderBackgroundStyle;
    stream << (int)mViewHeaderPolicy;
    stream << mIconSize;

    const int cnt = mColumns.count();
    stream << (int)cnt;

    for (int i = 0; i < cnt; ++i) {
        Column *col = mColumns.at(i);
        col->save(stream);
    }
}

void Theme::clearPixmapCache() const
{
    qDeleteAll(mPixmaps);
    mPixmaps.clear();
}

void Theme::populatePixmapCache() const
{
    clearPixmapCache();

    mPixmaps.reserve(_IconCount);
    // WARNING: The order of those icons must be in sync with order of the
    // corresponding enum values in ThemeIcon!
    mPixmaps << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-mark-unread-new")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-mark-unread")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-mark-read")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-deleted")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-replied")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-forwarded-replied")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-queued")).pixmap(mIconSize, mIconSize))  // mail-queue ?
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-mark-task")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-sent")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-forwarded")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("emblem-important")).pixmap(mIconSize, mIconSize))  // "flag"
             << new QPixmap(QIcon::fromTheme(QStringLiteral("messagelist/pics/mail-thread-watch.png")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("messagelist/pics/mail-thread-ignored.png")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-mark-junk")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-mark-notjunk")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-signed-verified")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-signed-part")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-signed")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("text-plain")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-encrypted-full")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-encrypted-part")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-encrypted")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("text-plain")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-attachment")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("view-pim-notes")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("mail-invitation")).pixmap(mIconSize, mIconSize))
             << ((QApplication::isRightToLeft())
        ? new QPixmap(QIcon::fromTheme(QStringLiteral("arrow-left")).pixmap(mIconSize, mIconSize))
        : new QPixmap(QIcon::fromTheme(QStringLiteral("arrow-right")).pixmap(mIconSize, mIconSize)))
             << new QPixmap(QIcon::fromTheme(QStringLiteral("arrow-down")).pixmap(mIconSize, mIconSize))
             << new QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("messagelist/pics/mail-vertical-separator-line.png")))
             << new QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("messagelist/pics/mail-horizontal-space.png")));
}
