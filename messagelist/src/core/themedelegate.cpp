/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/themedelegate.h"
using namespace Qt::Literals::StringLiterals;

#include "core/groupheaderitem.h"
#include "core/messageitem.h"
#include "messagelistsettings.h"

#include "MessageCore/MessageCoreSettings"
#include "MessageCore/StringUtil"

#include <KColorScheme>
#include <QAbstractItemView>
#include <QFont>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QPainter>
#include <QPixmap>
#include <QStyle>

using namespace MessageList::Core;

static const int gGroupHeaderOuterVerticalMargin = 1;
static const int gGroupHeaderOuterHorizontalMargin = 1;
static const int gGroupHeaderInnerVerticalMargin = 1;
static const int gGroupHeaderInnerHorizontalMargin = 1;
static const int gMessageVerticalMargin = 2;
static const int gMessageHorizontalMargin = 2;
static const int gHorizontalItemSpacing = 2;

ThemeDelegate::ThemeDelegate(QAbstractItemView *parent)
    : QStyledItemDelegate(parent)
    , mItemView(parent)
{
}

ThemeDelegate::~ThemeDelegate() = default;

void ThemeDelegate::setTheme(const Theme *theme)
{
    mTheme = theme;

    if (!mTheme) {
        return; // hum
    }

    // Rebuild the group header background color cache
    switch (mTheme->groupHeaderBackgroundMode()) {
    case Theme::Transparent:
        mGroupHeaderBackgroundColor = QColor(); // invalid
        break;
    case Theme::CustomColor:
        mGroupHeaderBackgroundColor = mTheme->groupHeaderBackgroundColor();
        break;
    case Theme::AutoColor: {
        QPalette pal = mItemView->palette();
        QColor txt = pal.color(QPalette::Normal, QPalette::Text);
        QColor bck = pal.color(QPalette::Normal, QPalette::Base);
        mGroupHeaderBackgroundColor = QColor((txt.red() + (bck.red() * 3)) / 4, (txt.green() + (bck.green() * 3)) / 4, (txt.blue() + (bck.blue() * 3)) / 4);
        break;
    }
    }

    generalFontChanged();

    mItemView->reset();
}

enum FontType : uint8_t {
    Normal,
    Bold,
    Italic,
    BoldItalic,

    FontTypesCount,
};

static QFont sFontCache[FontTypesCount];
static QFontMetrics sFontMetricsCache[FontTypesCount] = {QFontMetrics(QFont()), QFontMetrics(QFont()), QFontMetrics(QFont()), QFontMetrics(QFont())};
static int sFontHeightCache = 0;

static inline const QFontMetrics &cachedFontMetrics(const Theme::ContentItem *ci)
{
    return (!ci->isBold() && !ci->isItalic()) ? sFontMetricsCache[Normal]
        : (ci->isBold() && !ci->isItalic())   ? sFontMetricsCache[Bold]
        : (!ci->isBold() && ci->isItalic())   ? sFontMetricsCache[Italic]
                                              : sFontMetricsCache[BoldItalic];
}

static inline const QFont &cachedFont(const Theme::ContentItem *ci)
{
    return (!ci->isBold() && !ci->isItalic()) ? sFontCache[Normal]
        : (ci->isBold() && !ci->isItalic())   ? sFontCache[Bold]
        : (!ci->isBold() && ci->isItalic())   ? sFontCache[Italic]
                                              : sFontCache[BoldItalic];
}

static inline const QFont &cachedFont(const Theme::ContentItem *ci, const Item *i)
{
    if (i->type() != Item::Message) {
        return cachedFont(ci);
    }

    const auto mi = static_cast<const MessageItem *>(i);
    const bool bold = ci->isBold() || mi->isBold();
    const bool italic = ci->isItalic() || mi->isItalic();
    return (!bold && !italic) ? sFontCache[Normal] : (bold && !italic) ? sFontCache[Bold] : (!bold && italic) ? sFontCache[Italic] : sFontCache[BoldItalic];
}

static inline void paint_right_aligned_elided_text(const QString &text,
                                                   Theme::ContentItem *ci,
                                                   QPainter *painter,
                                                   int &left,
                                                   int top,
                                                   int &right,
                                                   Qt::LayoutDirection layoutDir,
                                                   const QFont &font)
{
    painter->setFont(font);
    const QFontMetrics &fontMetrics = cachedFontMetrics(ci);
    const int w = right - left;
    const QString elidedText = fontMetrics.elidedText(text, layoutDir == Qt::LeftToRight ? Qt::ElideLeft : Qt::ElideRight, w);
    const QRect rct(left, top, w, sFontHeightCache);
    QRect outRct;

    if (ci->softenByBlending()) {
        qreal oldOpacity = painter->opacity();
        painter->setOpacity(0.6);
        painter->drawText(rct, Qt::AlignTop | Qt::AlignRight | Qt::TextSingleLine, elidedText, &outRct);
        painter->setOpacity(oldOpacity);
    } else {
        painter->drawText(rct, Qt::AlignTop | Qt::AlignRight | Qt::TextSingleLine, elidedText, &outRct);
    }
    if (layoutDir == Qt::LeftToRight) {
        right -= outRct.width() + gHorizontalItemSpacing;
    } else {
        left += outRct.width() + gHorizontalItemSpacing;
    }
}

static inline void compute_bounding_rect_for_right_aligned_elided_text(const QString &text,
                                                                       Theme::ContentItem *ci,
                                                                       int &left,
                                                                       int top,
                                                                       int &right,
                                                                       QRect &outRect,
                                                                       Qt::LayoutDirection layoutDir,
                                                                       const QFont &font)
{
    Q_UNUSED(font)
    const QFontMetrics &fontMetrics = cachedFontMetrics(ci);
    const int w = right - left;
    const QString elidedText = fontMetrics.elidedText(text, layoutDir == Qt::LeftToRight ? Qt::ElideLeft : Qt::ElideRight, w);
    const QRect rct(left, top, w, sFontHeightCache);
    const Qt::AlignmentFlag af = layoutDir == Qt::LeftToRight ? Qt::AlignRight : Qt::AlignLeft;
    outRect = fontMetrics.boundingRect(rct, Qt::AlignTop | af | Qt::TextSingleLine, elidedText);
    if (layoutDir == Qt::LeftToRight) {
        right -= outRect.width() + gHorizontalItemSpacing;
    } else {
        left += outRect.width() + gHorizontalItemSpacing;
    }
}

static inline void paint_left_aligned_elided_text(const QString &text,
                                                  Theme::ContentItem *ci,
                                                  QPainter *painter,
                                                  int &left,
                                                  int top,
                                                  int &right,
                                                  Qt::LayoutDirection layoutDir,
                                                  const QFont &font)
{
    painter->setFont(font);
    const QFontMetrics &fontMetrics = cachedFontMetrics(ci);
    const int w = right - left;
    const QString elidedText = fontMetrics.elidedText(text, layoutDir == Qt::LeftToRight ? Qt::ElideRight : Qt::ElideLeft, w);
    const QRect rct(left, top, w, sFontHeightCache);
    QRect outRct;
    if (ci->softenByBlending()) {
        qreal oldOpacity = painter->opacity();
        painter->setOpacity(0.6);
        painter->drawText(rct, Qt::AlignTop | Qt::AlignLeft | Qt::TextSingleLine, elidedText, &outRct);
        painter->setOpacity(oldOpacity);
    } else {
        painter->drawText(rct, Qt::AlignTop | Qt::AlignLeft | Qt::TextSingleLine, elidedText, &outRct);
    }
    if (layoutDir == Qt::LeftToRight) {
        left += outRct.width() + gHorizontalItemSpacing;
    } else {
        right -= outRct.width() + gHorizontalItemSpacing;
    }
}

static inline void compute_bounding_rect_for_left_aligned_elided_text(const QString &text,
                                                                      Theme::ContentItem *ci,
                                                                      int &left,
                                                                      int top,
                                                                      int &right,
                                                                      QRect &outRect,
                                                                      Qt::LayoutDirection layoutDir,
                                                                      const QFont &font)
{
    Q_UNUSED(font)
    const QFontMetrics &fontMetrics = cachedFontMetrics(ci);
    const int w = right - left;
    const QString elidedText = fontMetrics.elidedText(text, layoutDir == Qt::LeftToRight ? Qt::ElideRight : Qt::ElideLeft, w);
    const QRect rct(left, top, w, sFontHeightCache);
    const Qt::AlignmentFlag af = layoutDir == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight;
    outRect = fontMetrics.boundingRect(rct, Qt::AlignTop | af | Qt::TextSingleLine, elidedText);
    if (layoutDir == Qt::LeftToRight) {
        left += outRect.width() + gHorizontalItemSpacing;
    } else {
        right -= outRect.width() + gHorizontalItemSpacing;
    }
}

static inline const QPixmap *get_read_state_icon(const Theme *theme, Item *item)
{
    if (item->status().isQueued()) {
        return theme->pixmap(Theme::IconQueued);
    } else if (item->status().isSent()) {
        return theme->pixmap(Theme::IconSent);
    } else if (item->status().isRead()) {
        return theme->pixmap(Theme::IconRead);
    } else if (!item->status().isRead()) {
        return theme->pixmap(Theme::IconUnread);
    } else if (item->status().isDeleted()) {
        return theme->pixmap(Theme::IconDeleted);
    }

    // Uhm... should never happen.. but fallback to "read"...
    return theme->pixmap(Theme::IconRead);
}

static inline const QPixmap *get_combined_read_replied_state_icon(const Theme *theme, MessageItem *messageItem)
{
    if (messageItem->status().isReplied()) {
        if (messageItem->status().isForwarded()) {
            return theme->pixmap(Theme::IconRepliedAndForwarded);
        }
        return theme->pixmap(Theme::IconReplied);
    }
    if (messageItem->status().isForwarded()) {
        return theme->pixmap(Theme::IconForwarded);
    }

    return get_read_state_icon(theme, messageItem);
}

static inline const QPixmap *get_encryption_state_icon(const Theme *theme, MessageItem *messageItem, bool *treatAsEnabled)
{
    switch (messageItem->encryptionState()) {
    case MessageItem::FullyEncrypted:
        *treatAsEnabled = true;
        return theme->pixmap(Theme::IconFullyEncrypted);
    case MessageItem::PartiallyEncrypted:
        *treatAsEnabled = true;
        return theme->pixmap(Theme::IconPartiallyEncrypted);
    case MessageItem::EncryptionStateUnknown:
        *treatAsEnabled = false;
        return theme->pixmap(Theme::IconUndefinedEncrypted);
    case MessageItem::NotEncrypted:
        *treatAsEnabled = false;
        return theme->pixmap(Theme::IconNotEncrypted);
    default:
        // should never happen
        Q_ASSERT(false);
        break;
    }

    *treatAsEnabled = false;
    return theme->pixmap(Theme::IconUndefinedEncrypted);
}

static inline const QPixmap *get_signature_state_icon(const Theme *theme, MessageItem *messageItem, bool *treatAsEnabled)
{
    switch (messageItem->signatureState()) {
    case MessageItem::FullySigned:
        *treatAsEnabled = true;
        return theme->pixmap(Theme::IconFullySigned);
    case MessageItem::PartiallySigned:
        *treatAsEnabled = true;
        return theme->pixmap(Theme::IconPartiallySigned);
    case MessageItem::SignatureStateUnknown:
        *treatAsEnabled = false;
        return theme->pixmap(Theme::IconUndefinedSigned);
    case MessageItem::NotSigned:
        *treatAsEnabled = false;
        return theme->pixmap(Theme::IconNotSigned);
    default:
        // should never happen
        Q_ASSERT(false);
        break;
    }

    *treatAsEnabled = false;
    return theme->pixmap(Theme::IconUndefinedSigned);
}

static inline const QPixmap *get_replied_state_icon(const Theme *theme, MessageItem *messageItem)
{
    if (messageItem->status().isReplied()) {
        if (messageItem->status().isForwarded()) {
            return theme->pixmap(Theme::IconRepliedAndForwarded);
        }
        return theme->pixmap(Theme::IconReplied);
    }
    if (messageItem->status().isForwarded()) {
        return theme->pixmap(Theme::IconForwarded);
    }

    return nullptr;
}

static inline const QPixmap *get_spam_ham_state_icon(const Theme *theme, MessageItem *messageItem)
{
    if (messageItem->status().isSpam()) {
        return theme->pixmap(Theme::IconSpam);
    } else if (messageItem->status().isHam()) {
        return theme->pixmap(Theme::IconHam);
    }
    return nullptr;
}

static inline const QPixmap *get_watched_ignored_state_icon(const Theme *theme, MessageItem *messageItem)
{
    if (messageItem->status().isIgnored()) {
        return theme->pixmap(Theme::IconIgnored);
    } else if (messageItem->status().isWatched()) {
        return theme->pixmap(Theme::IconWatched);
    }
    return nullptr;
}

static inline void paint_vertical_line(QPainter *painter, int &left, int top, int &right, int bottom, bool alignOnRight)
{
    if (alignOnRight) {
        right -= 1;
        if (right < 0) {
            return;
        }
        painter->drawLine(right, top, right, bottom);
        right -= 2;
        right -= gHorizontalItemSpacing;
    } else {
        left += 1;
        if (left > right) {
            return;
        }
        painter->drawLine(left, top, left, bottom);
        left += 2 + gHorizontalItemSpacing;
    }
}

static inline void compute_bounding_rect_for_vertical_line(int &left, int top, int &right, int bottom, QRect &outRect, bool alignOnRight)
{
    if (alignOnRight) {
        right -= 3;
        outRect = QRect(right, top, 3, bottom - top);
        right -= gHorizontalItemSpacing;
    } else {
        outRect = QRect(left, top, 3, bottom - top);
        left += 3 + gHorizontalItemSpacing;
    }
}

static inline void paint_horizontal_spacer(int &left, int, int &right, int, bool alignOnRight)
{
    if (alignOnRight) {
        right -= 3 + gHorizontalItemSpacing;
    } else {
        left += 3 + gHorizontalItemSpacing;
    }
}

static inline void compute_bounding_rect_for_horizontal_spacer(int &left, int top, int &right, int bottom, QRect &outRect, bool alignOnRight)
{
    if (alignOnRight) {
        right -= 3;
        outRect = QRect(right, top, 3, bottom - top);
        right -= gHorizontalItemSpacing;
    } else {
        outRect = QRect(left, top, 3, bottom - top);
        left += 3 + gHorizontalItemSpacing;
    }
}

static inline void
paint_permanent_icon(const QPixmap *pix, Theme::ContentItem *, QPainter *painter, int &left, int top, int &right, bool alignOnRight, int iconSize)
{
    if (alignOnRight) {
        right -= iconSize; // this icon is always present
        if (right < 0) {
            return;
        }
        painter->drawPixmap(right, top, iconSize, iconSize, *pix);
        right -= gHorizontalItemSpacing;
    } else {
        if (left > (right - iconSize)) {
            return;
        }
        painter->drawPixmap(left, top, iconSize, iconSize, *pix);
        left += iconSize + gHorizontalItemSpacing;
    }
}

static inline void
compute_bounding_rect_for_permanent_icon(Theme::ContentItem *, int &left, int top, int &right, QRect &outRect, bool alignOnRight, int iconSize)
{
    if (alignOnRight) {
        right -= iconSize; // this icon is always present
        outRect = QRect(right, top, iconSize, iconSize);
        right -= gHorizontalItemSpacing;
    } else {
        outRect = QRect(left, top, iconSize, iconSize);
        left += iconSize + gHorizontalItemSpacing;
    }
}

static inline void paint_boolean_state_icon(bool enabled,
                                            const QPixmap *pix,
                                            Theme::ContentItem *ci,
                                            QPainter *painter,
                                            int &left,
                                            int top,
                                            int &right,
                                            bool alignOnRight,
                                            int iconSize)
{
    if (enabled) {
        paint_permanent_icon(pix, ci, painter, left, top, right, alignOnRight, iconSize);
        return;
    }

    // off -> icon disabled
    if (ci->hideWhenDisabled()) {
        return; // doesn't even take space
    }

    if (ci->softenByBlendingWhenDisabled()) {
        // still paint, but very soft
        qreal oldOpacity = painter->opacity();
        painter->setOpacity(0.1);
        paint_permanent_icon(pix, ci, painter, left, top, right, alignOnRight, iconSize);
        painter->setOpacity(oldOpacity);
        return;
    }

    // just takes space
    if (alignOnRight) {
        right -= iconSize + gHorizontalItemSpacing;
    } else {
        left += iconSize + gHorizontalItemSpacing;
    }
}

static inline void compute_bounding_rect_for_boolean_state_icon(bool enabled,
                                                                Theme::ContentItem *ci,
                                                                int &left,
                                                                int top,
                                                                int &right,
                                                                QRect &outRect,
                                                                bool alignOnRight,
                                                                int iconSize)
{
    if ((!enabled) && ci->hideWhenDisabled()) {
        outRect = QRect();
        return; // doesn't even take space
    }

    compute_bounding_rect_for_permanent_icon(ci, left, top, right, outRect, alignOnRight, iconSize);
}

static inline void paint_tag_list(const QList<MessageItem::Tag *> &tagList, QPainter *painter, int &left, int top, int &right, bool alignOnRight, int iconSize)
{
    if (alignOnRight) {
        for (const MessageItem::Tag *tag : tagList) {
            right -= iconSize; // this icon is always present
            if (right < 0) {
                return;
            }
            painter->drawPixmap(right, top, iconSize, iconSize, tag->pixmap());
            right -= gHorizontalItemSpacing;
        }
    } else {
        for (const MessageItem::Tag *tag : tagList) {
            if (left > right - iconSize) {
                return;
            }
            painter->drawPixmap(left, top, iconSize, iconSize, tag->pixmap());
            left += iconSize + gHorizontalItemSpacing;
        }
    }
}

static inline void
compute_bounding_rect_for_tag_list(const QList<MessageItem::Tag *> &tagList, int &left, int top, int &right, QRect &outRect, bool alignOnRight, int iconSize)
{
    int width = tagList.count() * (iconSize + gHorizontalItemSpacing);
    if (alignOnRight) {
        right -= width;
        outRect = QRect(right, top, width, iconSize);
    } else {
        outRect = QRect(left, top, width, iconSize);
        left += width;
    }
}

static inline void compute_size_hint_for_item(Theme::ContentItem *ci, int &maxh, int &totalw, int iconSize, const Item *item)
{
    Q_UNUSED(item)
    if (ci->displaysText()) {
        if (sFontHeightCache > maxh) {
            maxh = sFontHeightCache;
        }
        totalw += ci->displaysLongText() ? 128 : 64;
        return;
    }

    if (ci->isIcon()) {
        totalw += iconSize + gHorizontalItemSpacing;
        if (maxh < iconSize) {
            maxh = iconSize;
        }
        return;
    }

    if (ci->isSpacer()) {
        if (18 > maxh) {
            maxh = 18;
        }
        totalw += 3 + gHorizontalItemSpacing;
        return;
    }

    // should never be reached
    if (18 > maxh) {
        maxh = 18;
    }
    totalw += gHorizontalItemSpacing;
}

static inline QSize compute_size_hint_for_row(const Theme::Row *r, int iconSize, const Item *item)
{
    int maxh = 8; // at least 8 pixels for a pixmap
    int totalw = 0;

    // right aligned stuff first
    auto items = r->rightItems();
    for (const auto it : std::as_const(items)) {
        compute_size_hint_for_item(const_cast<Theme::ContentItem *>(it), maxh, totalw, iconSize, item);
    }

    // then left aligned stuff
    items = r->leftItems();
    for (const auto it : std::as_const(items)) {
        compute_size_hint_for_item(const_cast<Theme::ContentItem *>(it), maxh, totalw, iconSize, item);
    }

    return {totalw, maxh};
}

void ThemeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return; // bleah
    }

    Item *item = itemFromIndex(index);
    if (!item) {
        return; // hm...
    }

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    opt.text.clear(); // draw no text for me, please.. I'll do it in a while

    // Set background color of control if necessary
    if (item->type() == Item::Message) {
        auto msgItem = static_cast<MessageItem *>(item);
        if (msgItem->backgroundColor().isValid()) {
            opt.backgroundBrush = QBrush(msgItem->backgroundColor());
        }
    }

    QStyle *style = mItemView->style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, mItemView);

    if (!mTheme) {
        return; // hm hm...
    }

    const Theme::Column *skcolumn = mTheme->column(index.column());
    if (!skcolumn) {
        return; // bleah
    }

    const QList<MessageList::Core::Theme::Row *> *rows;

    MessageItem *messageItem = nullptr;
    GroupHeaderItem *groupHeaderItem = nullptr;

    int top = opt.rect.top();
    int right = opt.rect.left() + opt.rect.width(); // don't use opt.rect.right() since it's screwed
    int left = opt.rect.left();

    // Storing the changed members one by one is faster than saving the painter state
    QFont oldFont = painter->font();
    QPen oldPen = painter->pen();
    qreal oldOpacity = painter->opacity();

    QPen defaultPen;
    bool usingNonDefaultTextColor = false;

    switch (item->type()) {
    case Item::Message:
        rows = &(skcolumn->messageRows());
        messageItem = static_cast<MessageItem *>(item);

        if ((!(opt.state & QStyle::State_Enabled)) || messageItem->aboutToBeRemoved() || (!messageItem->isValid())) {
            painter->setOpacity(0.5);
            defaultPen = QPen(opt.palette.brush(QPalette::Disabled, QPalette::Text), 0);
        } else {
            QPalette::ColorGroup cg;

            if (opt.state & QStyle::State_Active) {
                cg = QPalette::Normal;
            } else {
                cg = QPalette::Inactive;
            }

            if (opt.state & QStyle::State_Selected) {
                defaultPen = QPen(opt.palette.brush(cg, QPalette::HighlightedText), 0);
            } else {
                if (messageItem->textColor().isValid()) {
                    usingNonDefaultTextColor = true;
                    defaultPen = QPen(messageItem->textColor(), 0);
                } else {
                    defaultPen = QPen(opt.palette.brush(cg, QPalette::Text), 0);
                }
            }
        }

        top += gMessageVerticalMargin;
        right -= gMessageHorizontalMargin;
        left += gMessageHorizontalMargin;
        break;
    case Item::GroupHeader: {
        rows = &(skcolumn->groupHeaderRows());
        groupHeaderItem = static_cast<GroupHeaderItem *>(item);

        QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;

        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active)) {
            cg = QPalette::Inactive;
        }

        QPalette::ColorRole cr;

        top += gGroupHeaderOuterVerticalMargin;
        right -= gGroupHeaderOuterHorizontalMargin;
        left += gGroupHeaderOuterHorizontalMargin;

        switch (mTheme->groupHeaderBackgroundMode()) {
        case Theme::Transparent:
            cr = (opt.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text;
            defaultPen = QPen(opt.palette.brush(cg, cr), 0);
            break;
        case Theme::AutoColor:
        case Theme::CustomColor:
            switch (mTheme->groupHeaderBackgroundStyle()) {
            case Theme::PlainRect:
                painter->fillRect(QRect(left, top, right - left, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)),
                                  QBrush(mGroupHeaderBackgroundColor));
                break;
            case Theme::PlainJoinedRect: {
                int rleft = (opt.viewItemPosition == QStyleOptionViewItem::Beginning) || (opt.viewItemPosition == QStyleOptionViewItem::OnlyOne)
                    ? left
                    : opt.rect.left();
                int rright = (opt.viewItemPosition == QStyleOptionViewItem::End) || (opt.viewItemPosition == QStyleOptionViewItem::OnlyOne)
                    ? right
                    : opt.rect.left() + opt.rect.width();
                painter->fillRect(QRect(rleft, top, rright - rleft, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)),
                                  QBrush(mGroupHeaderBackgroundColor));
                break;
            }
            case Theme::RoundedJoinedRect:
                if (opt.viewItemPosition == QStyleOptionViewItem::Middle) {
                    painter->fillRect(QRect(opt.rect.left(), top, opt.rect.width(), opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)),
                                      QBrush(mGroupHeaderBackgroundColor));
                    break; // don't fall through
                }
                if (opt.viewItemPosition == QStyleOptionViewItem::Beginning) {
                    painter->fillRect(QRect(opt.rect.left() + opt.rect.width() - 10, top, 10, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)),
                                      QBrush(mGroupHeaderBackgroundColor));
                } else if (opt.viewItemPosition == QStyleOptionViewItem::End) {
                    painter->fillRect(QRect(opt.rect.left(), top, 10, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)),
                                      QBrush(mGroupHeaderBackgroundColor));
                }
                // fall through anyway
                [[fallthrough]];
            case Theme::RoundedRect: {
                painter->setPen(Qt::NoPen);
                bool hadAntialiasing = painter->renderHints() & QPainter::Antialiasing;
                if (!hadAntialiasing) {
                    painter->setRenderHint(QPainter::Antialiasing, true);
                }
                painter->setBrush(QBrush(mGroupHeaderBackgroundColor));
                painter->setBackgroundMode(Qt::OpaqueMode);
                int w = right - left;
                if (w > 0) {
                    painter->drawRoundedRect(QRect(left, top, w, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)), 4.0, 4.0);
                }
                if (!hadAntialiasing) {
                    painter->setRenderHint(QPainter::Antialiasing, false);
                }
                painter->setBackgroundMode(Qt::TransparentMode);
                break;
            }
            case Theme::GradientJoinedRect: {
                // FIXME: Could cache this brush
                QLinearGradient gradient(0, top, 0, top + opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2));
                gradient.setColorAt(0.0, KColorScheme::shade(mGroupHeaderBackgroundColor, KColorScheme::LightShade, 0.3));
                gradient.setColorAt(1.0, mGroupHeaderBackgroundColor);
                if (opt.viewItemPosition == QStyleOptionViewItem::Middle) {
                    painter->fillRect(QRect(opt.rect.left(), top, opt.rect.width(), opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)),
                                      QBrush(gradient));
                    break; // don't fall through
                }
                if (opt.viewItemPosition == QStyleOptionViewItem::Beginning) {
                    painter->fillRect(QRect(opt.rect.left() + opt.rect.width() - 10, top, 10, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)),
                                      QBrush(gradient));
                } else if (opt.viewItemPosition == QStyleOptionViewItem::End) {
                    painter->fillRect(QRect(opt.rect.left(), top, 10, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)), QBrush(gradient));
                }
                // fall through anyway
                [[fallthrough]];
            }
            case Theme::GradientRect: {
                // FIXME: Could cache this brush
                QLinearGradient gradient(0, top, 0, top + opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2));
                gradient.setColorAt(0.0, KColorScheme::shade(mGroupHeaderBackgroundColor, KColorScheme::LightShade, 0.3));
                gradient.setColorAt(1.0, mGroupHeaderBackgroundColor);
                painter->setPen(Qt::NoPen);
                bool hadAntialiasing = painter->renderHints() & QPainter::Antialiasing;
                if (!hadAntialiasing) {
                    painter->setRenderHint(QPainter::Antialiasing, true);
                }
                painter->setBrush(QBrush(gradient));
                painter->setBackgroundMode(Qt::OpaqueMode);
                int w = right - left;
                if (w > 0) {
                    painter->drawRoundedRect(QRect(left, top, w, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2)), 4.0, 4.0);
                }
                if (!hadAntialiasing) {
                    painter->setRenderHint(QPainter::Antialiasing, false);
                }
                painter->setBackgroundMode(Qt::TransparentMode);
                break;
            }
            case Theme::StyledRect:
                // oxygen, for instance, has a nice graphics for selected items
                opt.rect = QRect(left, top, right - left, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2));
                opt.state |= QStyle::State_Selected;
                opt.viewItemPosition = QStyleOptionViewItem::OnlyOne;
                opt.palette.setColor(cg, QPalette::Highlight, mGroupHeaderBackgroundColor);
                style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, mItemView);
                break;
            case Theme::StyledJoinedRect: {
                int rleft = (opt.viewItemPosition == QStyleOptionViewItem::Beginning) || (opt.viewItemPosition == QStyleOptionViewItem::OnlyOne)
                    ? left
                    : opt.rect.left();
                int rright = (opt.viewItemPosition == QStyleOptionViewItem::End) || (opt.viewItemPosition == QStyleOptionViewItem::OnlyOne)
                    ? right
                    : opt.rect.left() + opt.rect.width();
                opt.rect = QRect(rleft, top, rright - rleft, opt.rect.height() - (gGroupHeaderInnerVerticalMargin * 2));
                opt.state |= QStyle::State_Selected;
                opt.palette.setColor(cg, QPalette::Highlight, mGroupHeaderBackgroundColor);
                style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, mItemView);
                break;
            }
            }

            defaultPen = QPen(opt.palette.brush(cg, QPalette::Text), 0);
            break;
        }
        top += gGroupHeaderInnerVerticalMargin;
        right -= gGroupHeaderInnerHorizontalMargin;
        left += gGroupHeaderInnerHorizontalMargin;
        break;
    }
    default:
        Q_ASSERT(false);
        return; // bug
    }

    Qt::LayoutDirection layoutDir = mItemView->layoutDirection();

    for (const auto row : std::as_const(*rows)) {
        QSize rowSizeHint = compute_size_hint_for_row(row, mTheme->iconSize(), item);

        int bottom = top + rowSizeHint.height();

        // paint right aligned stuff first
        int r = right;
        int l = left;
        const auto rightItems = row->rightItems();
        for (const auto itemit : rightItems) {
            auto ci = const_cast<Theme::ContentItem *>(itemit);

            if (ci->canUseCustomColor()) {
                if (ci->useCustomColor() && (!(opt.state & QStyle::State_Selected))) {
                    if (usingNonDefaultTextColor) {
                        // merge the colors
                        QColor nonDefault = defaultPen.color();
                        QColor custom = ci->customColor();
                        QColor merged((nonDefault.red() + custom.red()) >> 1,
                                      (nonDefault.green() + custom.green()) >> 1,
                                      (nonDefault.blue() + custom.blue()) >> 1);
                        painter->setPen(QPen(merged));
                    } else {
                        painter->setPen(QPen(ci->customColor()));
                    }
                } else {
                    painter->setPen(defaultPen);
                }
            } // otherwise setting a pen is useless at this time

            const QFont &font = cachedFont(ci, item);

            switch (ci->type()) {
            case Theme::ContentItem::Subject:
                paint_right_aligned_elided_text(item->subject(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::SenderOrReceiver:
                paint_right_aligned_elided_text(item->displaySenderOrReceiver(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Receiver:
                paint_right_aligned_elided_text(item->displayReceiver(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Sender:
                paint_right_aligned_elided_text(item->displaySender(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Date:
                paint_right_aligned_elided_text(item->formattedDate(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::MostRecentDate:
                paint_right_aligned_elided_text(item->formattedMaxDate(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Size:
                paint_right_aligned_elided_text(item->formattedSize(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Folder:
                paint_right_aligned_elided_text(item->folder(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::GroupHeaderLabel:
                if (groupHeaderItem) {
                    paint_right_aligned_elided_text(groupHeaderItem->label(), ci, painter, l, top, r, layoutDir, font);
                }
                break;
            case Theme::ContentItem::ReadStateIcon:
                paint_permanent_icon(get_read_state_icon(mTheme, item), ci, painter, l, top, r, layoutDir == Qt::LeftToRight, mTheme->iconSize());
                break;
            case Theme::ContentItem::CombinedReadRepliedStateIcon:
                if (messageItem) {
                    paint_permanent_icon(get_combined_read_replied_state_icon(mTheme, messageItem),
                                         ci,
                                         painter,
                                         l,
                                         top,
                                         r,
                                         layoutDir == Qt::LeftToRight,
                                         mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ExpandedStateIcon: {
                const QPixmap *pix =
                    item->childItemCount() > 0 ? mTheme->pixmap((option.state & QStyle::State_Open) ? Theme::IconShowLess : Theme::IconShowMore) : nullptr;
                paint_boolean_state_icon(pix != nullptr,
                                         pix ? pix : mTheme->pixmap(Theme::IconShowMore),
                                         ci,
                                         painter,
                                         l,
                                         top,
                                         r,
                                         layoutDir == Qt::LeftToRight,
                                         mTheme->iconSize());
                break;
            }
            case Theme::ContentItem::RepliedStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_replied_state_icon(mTheme, messageItem);
                    paint_boolean_state_icon(pix != nullptr,
                                             pix ? pix : mTheme->pixmap(Theme::IconReplied),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir == Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::EncryptionStateIcon:
                if (messageItem) {
                    bool enabled;
                    const QPixmap *pix = get_encryption_state_icon(mTheme, messageItem, &enabled);
                    paint_boolean_state_icon(enabled, pix, ci, painter, l, top, r, layoutDir == Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::SignatureStateIcon:
                if (messageItem) {
                    bool enabled;
                    const QPixmap *pix = get_signature_state_icon(mTheme, messageItem, &enabled);
                    paint_boolean_state_icon(enabled, pix, ci, painter, l, top, r, layoutDir == Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::SpamHamStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_spam_ham_state_icon(mTheme, messageItem);
                    paint_boolean_state_icon(pix != nullptr,
                                             pix ? pix : mTheme->pixmap(Theme::IconSpam),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir == Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::WatchedIgnoredStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_watched_ignored_state_icon(mTheme, messageItem);
                    paint_boolean_state_icon(pix != nullptr,
                                             pix ? pix : mTheme->pixmap(Theme::IconWatched),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir == Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::AttachmentStateIcon:
                if (messageItem) {
                    paint_boolean_state_icon(messageItem->status().hasAttachment(),
                                             mTheme->pixmap(Theme::IconAttachment),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir == Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::InvitationIcon:
                if (messageItem) {
                    paint_boolean_state_icon(messageItem->status().hasInvitation(),
                                             mTheme->pixmap(Theme::IconInvitation),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir == Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ActionItemStateIcon:
                if (messageItem) {
                    paint_boolean_state_icon(messageItem->status().isToAct(),
                                             mTheme->pixmap(Theme::IconActionItem),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir == Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ImportantStateIcon:
                if (messageItem) {
                    paint_boolean_state_icon(messageItem->status().isImportant(),
                                             mTheme->pixmap(Theme::IconImportant),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir == Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::VerticalLine:
                paint_vertical_line(painter, l, top, r, bottom, layoutDir == Qt::LeftToRight);
                break;
            case Theme::ContentItem::HorizontalSpacer:
                paint_horizontal_spacer(l, top, r, bottom, layoutDir == Qt::LeftToRight);
                break;
            case Theme::ContentItem::TagList:
                if (messageItem) {
                    const QList<MessageItem::Tag *> tagList = messageItem->tagList();
                    paint_tag_list(tagList, painter, l, top, r, layoutDir == Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            }
        }

        // then paint left aligned stuff
        const auto leftItems = row->leftItems();
        for (const auto itemit : leftItems) {
            auto ci = const_cast<Theme::ContentItem *>(itemit);

            if (ci->canUseCustomColor()) {
                if (ci->useCustomColor() && (!(opt.state & QStyle::State_Selected))) {
                    if (usingNonDefaultTextColor) {
                        // merge the colors
                        QColor nonDefault = defaultPen.color();
                        QColor custom = ci->customColor();
                        QColor merged((nonDefault.red() + custom.red()) >> 1,
                                      (nonDefault.green() + custom.green()) >> 1,
                                      (nonDefault.blue() + custom.blue()) >> 1);
                        painter->setPen(QPen(merged));
                    } else {
                        painter->setPen(QPen(ci->customColor()));
                    }
                } else {
                    painter->setPen(defaultPen);
                }
            } // otherwise setting a pen is useless at this time

            const QFont &font = cachedFont(ci, item);

            switch (ci->type()) {
            case Theme::ContentItem::Subject:
                paint_left_aligned_elided_text(item->subject(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::SenderOrReceiver:
                paint_left_aligned_elided_text(item->displaySenderOrReceiver(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Receiver:
                paint_left_aligned_elided_text(item->displayReceiver(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Sender:
                paint_left_aligned_elided_text(item->displaySender(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Date:
                paint_left_aligned_elided_text(item->formattedDate(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::MostRecentDate:
                paint_left_aligned_elided_text(item->formattedMaxDate(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Size:
                paint_left_aligned_elided_text(item->formattedSize(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::Folder:
                paint_left_aligned_elided_text(item->folder(), ci, painter, l, top, r, layoutDir, font);
                break;
            case Theme::ContentItem::GroupHeaderLabel:
                if (groupHeaderItem) {
                    paint_left_aligned_elided_text(groupHeaderItem->label(), ci, painter, l, top, r, layoutDir, font);
                }
                break;
            case Theme::ContentItem::ReadStateIcon:
                paint_permanent_icon(get_read_state_icon(mTheme, item), ci, painter, l, top, r, layoutDir != Qt::LeftToRight, mTheme->iconSize());
                break;
            case Theme::ContentItem::CombinedReadRepliedStateIcon:
                if (messageItem) {
                    paint_permanent_icon(get_combined_read_replied_state_icon(mTheme, messageItem),
                                         ci,
                                         painter,
                                         l,
                                         top,
                                         r,
                                         layoutDir != Qt::LeftToRight,
                                         mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ExpandedStateIcon: {
                const QPixmap *pix =
                    item->childItemCount() > 0 ? mTheme->pixmap((option.state & QStyle::State_Open) ? Theme::IconShowLess : Theme::IconShowMore) : nullptr;
                paint_boolean_state_icon(pix != nullptr,
                                         pix ? pix : mTheme->pixmap(Theme::IconShowMore),
                                         ci,
                                         painter,
                                         l,
                                         top,
                                         r,
                                         layoutDir != Qt::LeftToRight,
                                         mTheme->iconSize());
                break;
            }
            case Theme::ContentItem::RepliedStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_replied_state_icon(mTheme, messageItem);
                    paint_boolean_state_icon(pix != nullptr,
                                             pix ? pix : mTheme->pixmap(Theme::IconReplied),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir != Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::EncryptionStateIcon:
                if (messageItem) {
                    bool enabled;
                    const QPixmap *pix = get_encryption_state_icon(mTheme, messageItem, &enabled);
                    paint_boolean_state_icon(enabled, pix, ci, painter, l, top, r, layoutDir != Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::SignatureStateIcon:
                if (messageItem) {
                    bool enabled;
                    const QPixmap *pix = get_signature_state_icon(mTheme, messageItem, &enabled);
                    paint_boolean_state_icon(enabled, pix, ci, painter, l, top, r, layoutDir != Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::SpamHamStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_spam_ham_state_icon(mTheme, messageItem);
                    paint_boolean_state_icon(pix != nullptr,
                                             pix ? pix : mTheme->pixmap(Theme::IconSpam),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir != Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::WatchedIgnoredStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_watched_ignored_state_icon(mTheme, messageItem);
                    paint_boolean_state_icon(pix != nullptr,
                                             pix ? pix : mTheme->pixmap(Theme::IconWatched),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir != Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::AttachmentStateIcon:
                if (messageItem) {
                    paint_boolean_state_icon(messageItem->status().hasAttachment(),
                                             mTheme->pixmap(Theme::IconAttachment),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir != Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::InvitationIcon:
                if (messageItem) {
                    paint_boolean_state_icon(messageItem->status().hasInvitation(),
                                             mTheme->pixmap(Theme::IconInvitation),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir != Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ActionItemStateIcon:
                if (messageItem) {
                    paint_boolean_state_icon(messageItem->status().isToAct(),
                                             mTheme->pixmap(Theme::IconActionItem),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir != Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ImportantStateIcon:
                if (messageItem) {
                    paint_boolean_state_icon(messageItem->status().isImportant(),
                                             mTheme->pixmap(Theme::IconImportant),
                                             ci,
                                             painter,
                                             l,
                                             top,
                                             r,
                                             layoutDir != Qt::LeftToRight,
                                             mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::VerticalLine:
                paint_vertical_line(painter, l, top, r, bottom, layoutDir != Qt::LeftToRight);
                break;
            case Theme::ContentItem::HorizontalSpacer:
                paint_horizontal_spacer(l, top, r, bottom, layoutDir != Qt::LeftToRight);
                break;
            case Theme::ContentItem::TagList:
                if (messageItem) {
                    const QList<MessageItem::Tag *> tagList = messageItem->tagList();
                    paint_tag_list(tagList, painter, l, top, r, layoutDir != Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            }
        }

        top = bottom;
    }

    painter->setFont(oldFont);
    painter->setPen(oldPen);
    painter->setOpacity(oldOpacity);
}

bool ThemeDelegate::hitTest(const QPoint &viewportPoint, bool exact)
{
    mHitItem = nullptr;
    mHitColumn = nullptr;
    mHitRow = nullptr;
    mHitContentItem = nullptr;

    if (!mTheme) {
        return false; // hm hm...
    }

    mHitIndex = mItemView->indexAt(viewportPoint);

    if (!mHitIndex.isValid()) {
        return false; // bleah
    }

    mHitItem = itemFromIndex(mHitIndex);
    if (!mHitItem) {
        return false; // hm...
    }

    mHitItemRect = mItemView->visualRect(mHitIndex);

    mHitColumn = mTheme->column(mHitIndex.column());
    if (!mHitColumn) {
        return false; // bleah
    }

    const QList<Theme::Row *> *rows; // I'd like to have it as reference, but gcc complains...

    MessageItem *messageItem = nullptr;
    GroupHeaderItem *groupHeaderItem = nullptr;

    int top = mHitItemRect.top();
    int right = mHitItemRect.right();
    int left = mHitItemRect.left();

    mHitRow = nullptr;
    mHitRowIndex = -1;
    mHitContentItem = nullptr;

    switch (mHitItem->type()) {
    case Item::Message:
        mHitRowIsMessageRow = true;
        rows = &(mHitColumn->messageRows());
        messageItem = static_cast<MessageItem *>(mHitItem);
        // FIXME: paint eventual background here

        top += gMessageVerticalMargin;
        right -= gMessageHorizontalMargin;
        left += gMessageHorizontalMargin;
        break;
    case Item::GroupHeader:
        mHitRowIsMessageRow = false;
        rows = &(mHitColumn->groupHeaderRows());
        groupHeaderItem = static_cast<GroupHeaderItem *>(mHitItem);

        top += gGroupHeaderOuterVerticalMargin + gGroupHeaderInnerVerticalMargin;
        right -= gGroupHeaderOuterHorizontalMargin + gGroupHeaderInnerHorizontalMargin;
        left += gGroupHeaderOuterHorizontalMargin + gGroupHeaderInnerHorizontalMargin;
        break;
    default:
        return false; // bug
        break;
    }

    int rowIdx = 0;
    int bestInexactDistance = 0xffffff;
    bool bestInexactItemRight = false;
    QRect bestInexactRect;
    const Theme::ContentItem *bestInexactContentItem = nullptr;

    Qt::LayoutDirection layoutDir = mItemView->layoutDirection();

    for (const auto row : std::as_const(*rows)) {
        QSize rowSizeHint = compute_size_hint_for_row(row, mTheme->iconSize(), mHitItem);

        if ((viewportPoint.y() < top) && (rowIdx > 0)) {
            break; // not this row (tough we should have already found it... probably clicked upper margin)
        }

        int bottom = top + rowSizeHint.height();

        if (viewportPoint.y() > bottom) {
            top += rowSizeHint.height();
            rowIdx++;
            continue; // not this row
        }

        bestInexactItemRight = false;
        bestInexactDistance = 0xffffff;
        bestInexactContentItem = nullptr;

        // this row!
        mHitRow = row;
        mHitRowIndex = rowIdx;
        mHitRowRect = QRect(left, top, right - left, bottom - top);

        // check right aligned stuff first
        mHitContentItemRight = true;

        int r = right;
        int l = left;
        const auto rightItems = mHitRow->rightItems();
        for (const auto itemit : rightItems) {
            auto ci = const_cast<Theme::ContentItem *>(itemit);

            mHitContentItemRect = QRect();

            const QFont &font = cachedFont(ci, mHitItem);

            switch (ci->type()) {
            case Theme::ContentItem::Subject:
                compute_bounding_rect_for_right_aligned_elided_text(mHitItem->subject(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::SenderOrReceiver:
                compute_bounding_rect_for_right_aligned_elided_text(mHitItem->displaySenderOrReceiver(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Receiver:
                compute_bounding_rect_for_right_aligned_elided_text(mHitItem->displayReceiver(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Sender:
                compute_bounding_rect_for_right_aligned_elided_text(mHitItem->displaySender(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Date:
                compute_bounding_rect_for_right_aligned_elided_text(mHitItem->formattedDate(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::MostRecentDate:
                compute_bounding_rect_for_right_aligned_elided_text(mHitItem->formattedMaxDate(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Size:
                compute_bounding_rect_for_right_aligned_elided_text(mHitItem->formattedSize(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Folder:
                compute_bounding_rect_for_right_aligned_elided_text(mHitItem->folder(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::GroupHeaderLabel:
                if (groupHeaderItem) {
                    compute_bounding_rect_for_right_aligned_elided_text(groupHeaderItem->label(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                }
                break;
            case Theme::ContentItem::ReadStateIcon:
                compute_bounding_rect_for_permanent_icon(ci, l, top, r, mHitContentItemRect, layoutDir == Qt::LeftToRight, mTheme->iconSize());
                break;
            case Theme::ContentItem::CombinedReadRepliedStateIcon:
                compute_bounding_rect_for_permanent_icon(ci, l, top, r, mHitContentItemRect, layoutDir == Qt::LeftToRight, mTheme->iconSize());
                break;
            case Theme::ContentItem::ExpandedStateIcon:
                compute_bounding_rect_for_boolean_state_icon(mHitItem->childItemCount() > 0,
                                                             ci,
                                                             l,
                                                             top,
                                                             r,
                                                             mHitContentItemRect,
                                                             layoutDir == Qt::LeftToRight,
                                                             mTheme->iconSize());
                break;
            case Theme::ContentItem::RepliedStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_replied_state_icon(mTheme, messageItem);
                    compute_bounding_rect_for_boolean_state_icon(pix != nullptr,
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir == Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::EncryptionStateIcon:
                if (messageItem) {
                    bool enabled;
                    get_encryption_state_icon(mTheme, messageItem, &enabled);
                    compute_bounding_rect_for_boolean_state_icon(enabled, ci, l, top, r, mHitContentItemRect, layoutDir == Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::SignatureStateIcon:
                if (messageItem) {
                    bool enabled;
                    get_signature_state_icon(mTheme, messageItem, &enabled);
                    compute_bounding_rect_for_boolean_state_icon(enabled, ci, l, top, r, mHitContentItemRect, layoutDir == Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::SpamHamStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_spam_ham_state_icon(mTheme, messageItem);
                    compute_bounding_rect_for_boolean_state_icon(pix != nullptr,
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir == Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::WatchedIgnoredStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_watched_ignored_state_icon(mTheme, messageItem);
                    compute_bounding_rect_for_boolean_state_icon(pix != nullptr,
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir == Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::AttachmentStateIcon:
                if (messageItem) {
                    compute_bounding_rect_for_boolean_state_icon(messageItem->status().hasAttachment(),
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir == Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::InvitationIcon:
                if (messageItem) {
                    compute_bounding_rect_for_boolean_state_icon(messageItem->status().hasInvitation(),
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir == Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ActionItemStateIcon:
                if (messageItem) {
                    compute_bounding_rect_for_boolean_state_icon(messageItem->status().isToAct(),
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir == Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ImportantStateIcon:
                if (messageItem) {
                    compute_bounding_rect_for_boolean_state_icon(messageItem->status().isImportant(),
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir == Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::VerticalLine:
                compute_bounding_rect_for_vertical_line(l, top, r, bottom, mHitContentItemRect, layoutDir == Qt::LeftToRight);
                break;
            case Theme::ContentItem::HorizontalSpacer:
                compute_bounding_rect_for_horizontal_spacer(l, top, r, bottom, mHitContentItemRect, layoutDir == Qt::LeftToRight);
                break;
            case Theme::ContentItem::TagList:
                if (messageItem) {
                    const QList<MessageItem::Tag *> tagList = messageItem->tagList();
                    compute_bounding_rect_for_tag_list(tagList, l, top, r, mHitContentItemRect, layoutDir == Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            }

            if (mHitContentItemRect.isValid()) {
                if (mHitContentItemRect.contains(viewportPoint)) {
                    // caught!
                    mHitContentItem = ci;
                    return true;
                }
                if (!exact) {
                    QRect inexactRect(mHitContentItemRect.left(), mHitRowRect.top(), mHitContentItemRect.width(), mHitRowRect.height());
                    if (inexactRect.contains(viewportPoint)) {
                        mHitContentItem = ci;
                        return true;
                    }

                    int inexactDistance =
                        viewportPoint.x() > inexactRect.right() ? viewportPoint.x() - inexactRect.right() : inexactRect.left() - viewportPoint.x();
                    if (inexactDistance < bestInexactDistance) {
                        bestInexactDistance = inexactDistance;
                        bestInexactRect = mHitContentItemRect;
                        bestInexactItemRight = true;
                        bestInexactContentItem = ci;
                    }
                }
            }
        }

        // then check left aligned stuff
        mHitContentItemRight = false;

        const auto leftItems = mHitRow->leftItems();
        for (const auto itemit : leftItems) {
            auto ci = const_cast<Theme::ContentItem *>(itemit);

            mHitContentItemRect = QRect();

            const QFont &font = cachedFont(ci, mHitItem);

            switch (ci->type()) {
            case Theme::ContentItem::Subject:
                compute_bounding_rect_for_left_aligned_elided_text(mHitItem->subject(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::SenderOrReceiver:
                compute_bounding_rect_for_left_aligned_elided_text(mHitItem->displaySenderOrReceiver(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Receiver:
                compute_bounding_rect_for_left_aligned_elided_text(mHitItem->displayReceiver(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Sender:
                compute_bounding_rect_for_left_aligned_elided_text(mHitItem->displaySender(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Date:
                compute_bounding_rect_for_left_aligned_elided_text(mHitItem->formattedDate(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::MostRecentDate:
                compute_bounding_rect_for_left_aligned_elided_text(mHitItem->formattedMaxDate(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Size:
                compute_bounding_rect_for_left_aligned_elided_text(mHitItem->formattedSize(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::Folder:
                compute_bounding_rect_for_left_aligned_elided_text(mHitItem->folder(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                break;
            case Theme::ContentItem::GroupHeaderLabel:
                if (groupHeaderItem) {
                    compute_bounding_rect_for_left_aligned_elided_text(groupHeaderItem->label(), ci, l, top, r, mHitContentItemRect, layoutDir, font);
                }
                break;
            case Theme::ContentItem::ReadStateIcon:
                compute_bounding_rect_for_permanent_icon(ci, l, top, r, mHitContentItemRect, layoutDir != Qt::LeftToRight, mTheme->iconSize());
                break;
            case Theme::ContentItem::CombinedReadRepliedStateIcon:
                compute_bounding_rect_for_permanent_icon(ci, l, top, r, mHitContentItemRect, layoutDir != Qt::LeftToRight, mTheme->iconSize());
                break;
            case Theme::ContentItem::ExpandedStateIcon:
                compute_bounding_rect_for_boolean_state_icon(mHitItem->childItemCount() > 0,
                                                             ci,
                                                             l,
                                                             top,
                                                             r,
                                                             mHitContentItemRect,
                                                             layoutDir != Qt::LeftToRight,
                                                             mTheme->iconSize());
                break;
            case Theme::ContentItem::RepliedStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_replied_state_icon(mTheme, messageItem);
                    compute_bounding_rect_for_boolean_state_icon(pix != nullptr,
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir != Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::EncryptionStateIcon:
                if (messageItem) {
                    bool enabled;
                    get_encryption_state_icon(mTheme, messageItem, &enabled);
                    compute_bounding_rect_for_boolean_state_icon(enabled, ci, l, top, r, mHitContentItemRect, layoutDir != Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::SignatureStateIcon:
                if (messageItem) {
                    bool enabled;
                    get_signature_state_icon(mTheme, messageItem, &enabled);
                    compute_bounding_rect_for_boolean_state_icon(enabled, ci, l, top, r, mHitContentItemRect, layoutDir != Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::SpamHamStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_spam_ham_state_icon(mTheme, messageItem);
                    compute_bounding_rect_for_boolean_state_icon(pix != nullptr,
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir != Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::WatchedIgnoredStateIcon:
                if (messageItem) {
                    const QPixmap *pix = get_watched_ignored_state_icon(mTheme, messageItem);
                    compute_bounding_rect_for_boolean_state_icon(pix != nullptr,
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir != Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::AttachmentStateIcon:
                if (messageItem) {
                    compute_bounding_rect_for_boolean_state_icon(messageItem->status().hasAttachment(),
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir != Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::InvitationIcon:
                if (messageItem) {
                    compute_bounding_rect_for_boolean_state_icon(messageItem->status().hasInvitation(),
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir != Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ActionItemStateIcon:
                if (messageItem) {
                    compute_bounding_rect_for_boolean_state_icon(messageItem->status().isToAct(),
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir != Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::ImportantStateIcon:
                if (messageItem) {
                    compute_bounding_rect_for_boolean_state_icon(messageItem->status().isImportant(),
                                                                 ci,
                                                                 l,
                                                                 top,
                                                                 r,
                                                                 mHitContentItemRect,
                                                                 layoutDir != Qt::LeftToRight,
                                                                 mTheme->iconSize());
                }
                break;
            case Theme::ContentItem::VerticalLine:
                compute_bounding_rect_for_vertical_line(l, top, r, bottom, mHitContentItemRect, layoutDir != Qt::LeftToRight);
                break;
            case Theme::ContentItem::HorizontalSpacer:
                compute_bounding_rect_for_horizontal_spacer(l, top, r, bottom, mHitContentItemRect, layoutDir != Qt::LeftToRight);
                break;
            case Theme::ContentItem::TagList:
                if (messageItem) {
                    const QList<MessageItem::Tag *> tagList = messageItem->tagList();
                    compute_bounding_rect_for_tag_list(tagList, l, top, r, mHitContentItemRect, layoutDir != Qt::LeftToRight, mTheme->iconSize());
                }
                break;
            }

            if (mHitContentItemRect.isValid()) {
                if (mHitContentItemRect.contains(viewportPoint)) {
                    // caught!
                    mHitContentItem = ci;
                    return true;
                }
                if (!exact) {
                    QRect inexactRect(mHitContentItemRect.left(), mHitRowRect.top(), mHitContentItemRect.width(), mHitRowRect.height());
                    if (inexactRect.contains(viewportPoint)) {
                        mHitContentItem = ci;
                        return true;
                    }

                    int inexactDistance =
                        viewportPoint.x() > inexactRect.right() ? viewportPoint.x() - inexactRect.right() : inexactRect.left() - viewportPoint.x();
                    if (inexactDistance < bestInexactDistance) {
                        bestInexactDistance = inexactDistance;
                        bestInexactRect = mHitContentItemRect;
                        bestInexactItemRight = false;
                        bestInexactContentItem = ci;
                    }
                }
            }
        }

        top += rowSizeHint.height();
        rowIdx++;
    }

    mHitContentItem = bestInexactContentItem;
    mHitContentItemRight = bestInexactItemRight;
    mHitContentItemRect = bestInexactRect;
    return true;
}

const QModelIndex &ThemeDelegate::hitIndex() const
{
    return mHitIndex;
}

Item *ThemeDelegate::hitItem() const
{
    return mHitItem;
}

QRect ThemeDelegate::hitItemRect() const
{
    return mHitItemRect;
}

const Theme::Column *ThemeDelegate::hitColumn() const
{
    return mHitColumn;
}

int ThemeDelegate::hitColumnIndex() const
{
    return mHitIndex.column();
}

const Theme::Row *ThemeDelegate::hitRow() const
{
    return mHitRow;
}

int ThemeDelegate::hitRowIndex() const
{
    return mHitRowIndex;
}

QRect ThemeDelegate::hitRowRect() const
{
    return mHitRowRect;
}

bool ThemeDelegate::hitRowIsMessageRow() const
{
    return mHitRowIsMessageRow;
}

const Theme::ContentItem *ThemeDelegate::hitContentItem() const
{
    return mHitContentItem;
}

bool ThemeDelegate::hitContentItemRight() const
{
    return mHitContentItemRight;
}

QRect ThemeDelegate::hitContentItemRect() const
{
    return mHitContentItemRect;
}

QSize ThemeDelegate::sizeHintForItemTypeAndColumn(Item::Type type, int column, const Item *item) const
{
    if (!mTheme) {
        return {16, 16}; // bleah
    }

    const Theme::Column *skcolumn = mTheme->column(column);
    if (!skcolumn) {
        return {16, 16}; // bleah
    }

    const QList<Theme::Row *> *rows; // I'd like to have it as reference, but gcc complains...

    // The sizeHint() is layout direction independent.

    int marginw;
    int marginh;

    switch (type) {
    case Item::Message:
        rows = &(skcolumn->messageRows());

        marginh = gMessageVerticalMargin << 1;
        marginw = gMessageHorizontalMargin << 1;
        break;
    case Item::GroupHeader:
        rows = &(skcolumn->groupHeaderRows());

        marginh = (gGroupHeaderOuterVerticalMargin + gGroupHeaderInnerVerticalMargin) << 1;
        marginw = (gGroupHeaderOuterVerticalMargin + gGroupHeaderInnerVerticalMargin) << 1;
        break;
    default:
        return {16, 16}; // bug
        break;
    }

    int totalh = 0;
    int maxw = 0;

    for (QList<Theme::Row *>::ConstIterator rowit = rows->constBegin(), endRowIt = rows->constEnd(); rowit != endRowIt; ++rowit) {
        const QSize sh = compute_size_hint_for_row((*rowit), mTheme->iconSize(), item);
        totalh += sh.height();
        if (sh.width() > maxw) {
            maxw = sh.width();
        }
    }

    return {maxw + marginw, totalh + marginh};
}

QSize ThemeDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (!mTheme || !index.isValid()) {
        return {16, 16}; // hm hm...
    }

    Item *item = itemFromIndex(index);
    if (!item) {
        return {16, 16}; // hm...
    }

    const Item::Type type = item->type();
    if (type == Item::Message) {
        if (!mCachedMessageItemSizeHint.isValid()) {
            mCachedMessageItemSizeHint = sizeHintForItemTypeAndColumn(Item::Message, index.column(), item);
        }
        return mCachedMessageItemSizeHint;
    } else if (type == Item::GroupHeader) {
        if (!mCachedGroupHeaderItemSizeHint.isValid()) {
            mCachedGroupHeaderItemSizeHint = sizeHintForItemTypeAndColumn(Item::GroupHeader, index.column(), item);
        }
        return mCachedGroupHeaderItemSizeHint;
    } else {
        Q_ASSERT(false);
        return {};
    }
}

// Store the new fonts when the generalFont changes and flush sizeHint cache
void ThemeDelegate::generalFontChanged()
{
    mCachedMessageItemSizeHint = QSize();
    mCachedGroupHeaderItemSizeHint = QSize();

    QFont font;
    if (MessageCore::MessageCoreSettings::self()->useDefaultFonts()) {
        font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    } else {
        font = MessageListSettings::self()->messageListFont();
    }
    sFontCache[Normal] = font;
    sFontMetricsCache[Normal] = QFontMetrics(font);
    font.setBold(true);
    sFontCache[Bold] = font;
    sFontMetricsCache[Bold] = QFontMetrics(font);
    font.setBold(false);
    font.setItalic(true);
    sFontCache[Italic] = font;
    sFontMetricsCache[Italic] = QFontMetrics(font);
    font.setBold(true);
    font.setItalic(true);
    sFontCache[BoldItalic] = font;
    sFontMetricsCache[BoldItalic] = QFontMetrics(font);

    sFontHeightCache = sFontMetricsCache[Normal].height();
}

const Theme *ThemeDelegate::theme() const
{
    return mTheme;
}

#include "moc_themedelegate.cpp"
