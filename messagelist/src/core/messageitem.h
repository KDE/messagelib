/*!****************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "core/item.h"
#include "core/md5hash.h"
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
/*!
 * \class MessageList::Core::MessageItem
 * \inmodule MessageList
 * \inheaderfile MessageList/MessageItem
 *
 * \brief The MessageItem class
 */
class MESSAGELIST_EXPORT MessageItem : public Item, public ModelInvariantIndex
{
public:
    /*!
     * \class MessageList::Core::MessageItem::Tag
     * \inmodule MessageList
     * \inheaderfile MessageList/MessageItem
     *
     * \brief Represents a message tag.
     */
    class MESSAGELIST_EXPORT Tag
    {
    public:
        /*!
         * \brief Constructs a tag.
         * \param pix The icon pixmap for the tag.
         * \param tagName The name of the tag.
         * \param tagId The unique identifier of the tag.
         */
        explicit Tag(const QPixmap &pix, const QString &tagName, const QString &tagId);
        /*!
         * \brief Destructor.
         */
        ~Tag();
        /*!
         * \brief Returns the pixmap associated with this tag.
         * \return The tag pixmap.
         */
        const QPixmap &pixmap() const;
        /*!
         * \brief Returns the name of this tag.
         * \return The tag name.
         */
        const QString &name() const;
        /*!
         * \brief Returns the ID of this tag.
         * \return The tag ID.
         */
        const QString &id() const;
        /*!
         * \brief Returns the text color for this tag.
         * \return The text color.
         */
        const QColor &textColor() const;
        /*!
         * \brief Returns the background color for this tag.
         * \return The background color.
         */
        const QColor &backgroundColor() const;
        /*!
         * \brief Returns the font for this tag.
         * \return The font.
         */
        const QFont &font() const;
        /*!
         * \brief Returns the priority of this tag.
         * \return The tag priority.
         */
        int priority() const;

        /*!
         * \brief Sets the text color for this tag.
         * \param textColor The text color to set.
         */
        void setTextColor(const QColor &textColor);
        /*!
         * \brief Sets the background color for this tag.
         * \param backgroundColor The background color to set.
         */
        void setBackgroundColor(const QColor &backgroundColor);
        /*!
         * \brief Sets the font for this tag.
         * \param font The font to set.
         */
        void setFont(const QFont &font);
        /*!
         * \brief Sets the priority for this tag.
         * \param priority The priority to set.
         */
        void setPriority(int priority);

    private:
        class TagPrivate;
        std::unique_ptr<TagPrivate> const d;
    };

    /*!
     * \enum ThreadingStatus
     * \brief The threading status of the message.
     */
    enum ThreadingStatus {
        PerfectParentFound, ///< this message found a perfect parent to attach to
        ImperfectParentFound, ///< this message found an imperfect parent to attach to (might be fixed later)
        ParentMissing, ///< this message might belong to a thread but its parent is actually missing
        NonThreadable ///< this message does not look as being threadable
    };

    /*!
     * \enum EncryptionState
     * \brief The encryption state of the message.
     */
    enum EncryptionState {
        NotEncrypted,
        PartiallyEncrypted,
        FullyEncrypted,
        EncryptionStateUnknown
    };

    /*!
     * \enum SignatureState
     * \brief The signature state of the message.
     */
    enum SignatureState {
        NotSigned,
        PartiallySigned,
        FullySigned,
        SignatureStateUnknown
    };

    /*!
     * \brief Constructs a MessageItem.
     */
    explicit MessageItem();
    /*!
     * \brief Destructor.
     */
    ~MessageItem() override;

public:
    /*!
     * \brief Returns the list of tags for this item.
     * \return The list of tags.
     */
    virtual QList<Tag *> tagList() const;

    /*!
     * \brief Returns Tag associated to this message that has the specified id or 0
     * if no such tag exists. mTagList will be 0 in 99% of the cases.
     * \param szTagId The tag ID to search for.
     * \return The tag with the given ID or nullptr.
     */
    const Tag *findTag(const QString &szTagId) const;

    [[nodiscard]] QString tagListDescription() const;

    /// Deletes all cached tags. The next time someone asks this item for the tags, they are
    /// fetched again
    void invalidateTagCache();

    /*!
     */
    [[nodiscard]] const QColor &textColor() const;

    /*!
     */
    [[nodiscard]] const QColor &backgroundColor() const;

    /*!
     */
    [[nodiscard]] bool isBold() const
    {
        return font().bold();
    }

    /*!
     */
    [[nodiscard]] bool isItalic() const
    {
        return font().italic();
    }

    /*!
     */
    [[nodiscard]] SignatureState signatureState() const;

    /*!
     */
    void setSignatureState(SignatureState state);

    /*!
     */
    [[nodiscard]] EncryptionState encryptionState() const;

    /*!
     */
    void setEncryptionState(EncryptionState state);

    /*!
     */
    [[nodiscard]] MD5Hash messageIdMD5() const;

    /*!
     */
    void setMessageIdMD5(MD5Hash md5);

    /*!
     */
    [[nodiscard]] MD5Hash inReplyToIdMD5() const;

    /*!
     */
    void setInReplyToIdMD5(MD5Hash md5);

    /*!
     */
    [[nodiscard]] MD5Hash referencesIdMD5() const;

    /*!
     */
    void setReferencesIdMD5(MD5Hash md5);

    /*!
     */
    void setSubjectIsPrefixed(bool subjectIsPrefixed);

    /*!
     */
    [[nodiscard]] bool subjectIsPrefixed() const;

    /*!
     */
    [[nodiscard]] MD5Hash strippedSubjectMD5() const;

    /*!
     */
    void setStrippedSubjectMD5(MD5Hash md5);

    /*!
     */
    [[nodiscard]] bool aboutToBeRemoved() const;

    /*!
     */
    void setAboutToBeRemoved(bool aboutToBeRemoved);

    /*!
     */
    [[nodiscard]] ThreadingStatus threadingStatus() const;

    /*!
     */
    void setThreadingStatus(ThreadingStatus threadingStatus);

    /*!
     */
    [[nodiscard]] unsigned long uniqueId() const;

    /*!
     */
    [[nodiscard]] Akonadi::Item akonadiItem() const;
    /*!
     */
    void setAkonadiItem(const Akonadi::Item &item);

    /*!
     */
    [[nodiscard]] MessageItem *topmostMessage();

    /*!
     */
    [[nodiscard]] QString accessibleText(const MessageList::Core::Theme *theme, int columnIndex);

    /*!
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
    /*!
     */
    static void setUnreadMessageColor(const QColor &color);
    /*!
     */
    static void setImportantMessageColor(const QColor &color);
    /*!
     */
    static void setToDoMessageColor(const QColor &color);
    /*!
     */
    static void setGeneralFont(const QFont &font);
    /*!
     */
    static void setUnreadMessageFont(const QFont &font);
    /*!
     */
    static void setImportantMessageFont(const QFont &font);
    /*!
     */
    static void setToDoMessageFont(const QFont &font);

protected:
    explicit MessageItem(MessageItemPrivate *dd);

private:
    MESSAGELIST_NO_EXPORT const QFont &font() const;

    [[nodiscard]] MESSAGELIST_NO_EXPORT QString accessibleTextForField(Theme::ContentItem::Type field);

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
    [[nodiscard]] QList<Tag *> tagList() const override;

    /// Sets a list of fake tags for this item
    void setFakeTags(const QList<Tag *> &tagList);

private:
    Q_DECLARE_PRIVATE(FakeItem)
};
} // namespace Core
} // namespace MessageList
