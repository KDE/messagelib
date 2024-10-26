/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "core/item.h"
#include "core/modelinvariantindex.h"

#include "messagelist_export.h"
#include "theme.h"
#include <QColor>
#include <QFont>
#include <QPixmap>
#include <QString>

namespace Akonadi
{
class Item;
}

namespace MessageList
{
namespace Core
{
class MessageItemPrivate;
/**
 * @brief The MessageItem class
 */
class MESSAGELIST_EXPORT MessageItem : public Item, public ModelInvariantIndex
{
public:
    class MESSAGELIST_EXPORT Tag
    {
    public:
        explicit Tag(const QPixmap &pix, const QString &tagName, const QString &tagId);
        ~Tag();
        const QPixmap &pixmap() const;
        const QString &name() const;
        const QString &id() const;
        const QColor &textColor() const;
        const QColor &backgroundColor() const;
        const QFont &font() const;
        int priority() const;

        void setTextColor(const QColor &textColor);
        void setBackgroundColor(const QColor &backgroundColor);
        void setFont(const QFont &font);
        void setPriority(int priority);

    private:
        class TagPrivate;
        std::unique_ptr<TagPrivate> const d;
    };

    enum ThreadingStatus {
        PerfectParentFound, ///< this message found a perfect parent to attach to
        ImperfectParentFound, ///< this message found an imperfect parent to attach to (might be fixed later)
        ParentMissing, ///< this message might belong to a thread but its parent is actually missing
        NonThreadable ///< this message does not look as being threadable
    };

    enum EncryptionState {
        NotEncrypted,
        PartiallyEncrypted,
        FullyEncrypted,
        EncryptionStateUnknown
    };

    enum SignatureState {
        NotSigned,
        PartiallySigned,
        FullySigned,
        SignatureStateUnknown
    };

    explicit MessageItem();
    ~MessageItem() override;

public:
    /// Returns the list of tags for this item.
    virtual QList<Tag *> tagList() const;

    /**
     * Returns Tag associated to this message that has the specified id or 0
     * if no such tag exists. mTagList will be 0 in 99% of the cases.
     */
    const Tag *findTag(const QString &szTagId) const;

    [[nodiscard]] QString tagListDescription() const;

    /// Deletes all cached tags. The next time someone asks this item for the tags, they are
    /// fetched again
    void invalidateTagCache();

    const QColor &textColor() const;

    const QColor &backgroundColor() const;

    [[nodiscard]] bool isBold() const
    {
        return font().bold();
    }

    [[nodiscard]] bool isItalic() const
    {
        return font().italic();
    }

    [[nodiscard]] SignatureState signatureState() const;

    void setSignatureState(SignatureState state);

    [[nodiscard]] EncryptionState encryptionState() const;

    void setEncryptionState(EncryptionState state);

    [[nodiscard]] QByteArray messageIdMD5() const;

    void setMessageIdMD5(const QByteArray &md5);

    [[nodiscard]] QByteArray inReplyToIdMD5() const;

    void setInReplyToIdMD5(const QByteArray &md5);

    [[nodiscard]] QByteArray referencesIdMD5() const;

    void setReferencesIdMD5(const QByteArray &md5);

    void setSubjectIsPrefixed(bool subjectIsPrefixed);

    [[nodiscard]] bool subjectIsPrefixed() const;

    [[nodiscard]] QByteArray strippedSubjectMD5() const;

    void setStrippedSubjectMD5(const QByteArray &md5);

    [[nodiscard]] bool aboutToBeRemoved() const;

    void setAboutToBeRemoved(bool aboutToBeRemoved);

    [[nodiscard]] ThreadingStatus threadingStatus() const;

    void setThreadingStatus(ThreadingStatus threadingStatus);

    [[nodiscard]] unsigned long uniqueId() const;

    Akonadi::Item akonadiItem() const;
    void setAkonadiItem(const Akonadi::Item &item);

    MessageItem *topmostMessage();

    QString accessibleText(const MessageList::Core::Theme *theme, int columnIndex);

    /**
     * Appends the whole subtree originating at this item
     * to the specified list. This item is included!
     */
    void subTreeToList(QList<MessageItem *> &list);

    //
    // Colors and fonts shared by all message items.
    // textColor() and font() will take the message status into account and return
    // one of these.
    // Call these setters only once when reading the colors from the config file.
    //
    static void setUnreadMessageColor(const QColor &color);
    static void setImportantMessageColor(const QColor &color);
    static void setToDoMessageColor(const QColor &color);
    static void setGeneralFont(const QFont &font);
    static void setUnreadMessageFont(const QFont &font);
    static void setImportantMessageFont(const QFont &font);
    static void setToDoMessageFont(const QFont &font);

protected:
    explicit MessageItem(MessageItemPrivate *dd);

private:
    MESSAGELIST_NO_EXPORT const QFont &font() const;

    MESSAGELIST_NO_EXPORT QString accessibleTextForField(Theme::ContentItem::Type field);

    Q_DECLARE_PRIVATE(MessageItem)
};

class FakeItemPrivate;

/// A message item that can have a fake tag list and a fake annotation
class FakeItem : public MessageItem
{
public:
    explicit FakeItem();
    ~FakeItem() override;

    /// Reimplemented to return the fake tag list
    QList<Tag *> tagList() const override;

    /// Sets a list of fake tags for this item
    void setFakeTags(const QList<Tag *> &tagList);

private:
    Q_DECLARE_PRIVATE(FakeItem)
};
} // namespace Core
} // namespace MessageList
