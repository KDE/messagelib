/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagepart.h"

#include <QStringList>

#include <KMime/Headers>
#include <KMime/Message>
#include <memory>
namespace MessageComposer
{
/*!
 \class MessageComposer::InfoPart
 \inmodule MessageComposer
 \inheaderfile MessageComposer/InfoPart

 The InfoPart class contains the message header.
 */
class MESSAGECOMPOSER_EXPORT InfoPart : public MessageComposer::MessagePart
{
    Q_OBJECT

    /// The email address and optionally the name of the author of the mail.
    Q_PROPERTY(QString from READ from WRITE setFrom NOTIFY fromChanged)

    /// The email address and optionally the name of the primary recipients.
    Q_PROPERTY(QStringList to READ to WRITE setTo NOTIFY toChanged)

    /// Carbon copy: The email address and optionally the name of the secondary recipients.
    Q_PROPERTY(QStringList cc READ cc WRITE setCc NOTIFY ccChanged)

    /// Blind Carbon copy: The email address and optionally the name of the secondary recipients.
    /// Only specified during SMTP delivery but not in the final mail delivery.
    Q_PROPERTY(QStringList bcc READ bcc WRITE setBcc NOTIFY bccChanged)

    /// Reply-To: Email address that should be used to reply to this mail.
    Q_PROPERTY(QStringList replyTo READ replyTo WRITE setReplyTo NOTIFY replyToChanged)

    /// Subject of the message.
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY subjectChanged)

    /// The name of a file, to which a copy of the sent message should be appended.
    Q_PROPERTY(QString fcc READ fcc WRITE setFcc NOTIFY fccChanged)

    /// User agent of the sender.
    Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged)

    /// Set urgency of the message.
    Q_PROPERTY(bool urgent READ urgent WRITE setUrgent NOTIFY urgentChanged)

    /// In-Reply-To: Id of the message this message is a reply to.
    Q_PROPERTY(QString inReplyTo READ inReplyTo WRITE setInReplyTo NOTIFY inReplyToChanged)

public:
    /*! \brief Constructs an InfoPart.
        \param parent The parent object.
    */
    explicit InfoPart(QObject *parent = nullptr);
    /*! \brief Destroys the InfoPart. */
    ~InfoPart() override;

    /*! \brief Returns the sender's email address. */
    [[nodiscard]] QString from() const;
    /*! \brief Sets the sender's email address.
        \param from The sender's email address.
    */
    void setFrom(const QString &from);
    /*! \brief Returns the list of primary recipients. */
    [[nodiscard]] QStringList to() const;
    /*! \brief Sets the list of primary recipients.
        \param to The recipient email addresses.
    */
    void setTo(const QStringList &to);
    /*! \brief Returns the list of carbon copy recipients. */
    [[nodiscard]] QStringList cc() const;
    /*! \brief Sets the list of carbon copy recipients.
        \param cc The CC recipient email addresses.
    */
    void setCc(const QStringList &cc);
    /*! \brief Returns the list of blind carbon copy recipients. */
    [[nodiscard]] QStringList bcc() const;
    /*! \brief Sets the list of blind carbon copy recipients.
        \param bcc The BCC recipient email addresses.
    */
    void setBcc(const QStringList &bcc);

    /*! \brief Returns the list of reply-to addresses. */
    [[nodiscard]] QStringList replyTo() const;
    /*! \brief Sets the list of reply-to addresses.
        \param replyTo The reply-to email addresses.
    */
    void setReplyTo(const QStringList &replyTo);

    /*! \brief Returns the message subject. */
    [[nodiscard]] QString subject() const;
    /*! \brief Sets the message subject.
        \param subject The subject text.
    */
    void setSubject(const QString &subject);

    /*! \brief Returns the FCC (File Carbon Copy) folder path. */
    [[nodiscard]] QString fcc() const;
    /*! \brief Sets the FCC folder path.
        \param fcc The folder path to save a copy of the message.
    */
    void setFcc(const QString &fcc);

    /*! \brief Returns the user agent string. */
    [[nodiscard]] QString userAgent() const;
    /*! \brief Sets the user agent string.
        \param userAgent The user agent to identify the mail client.
    */
    void setUserAgent(const QString &userAgent);

    /*! \brief Returns whether the message is marked as urgent. */
    [[nodiscard]] bool urgent() const;
    /*! \brief Sets the urgent flag for the message.
        \param urgent True to mark the message as urgent.
    */
    void setUrgent(bool);

    /*! \brief Returns the In-Reply-To header value. */
    [[nodiscard]] QString inReplyTo() const;
    /*! \brief Sets the In-Reply-To header value.
        \param inReplyTo The ID of the message this is a reply to.
    */
    void setInReplyTo(const QString &inReplyTo);
    /*! \brief Returns the References header value. */
    [[nodiscard]] QString references() const;
    /*! \brief Sets the References header value.
        \param references The references chain for message threading.
    */
    void setReferences(const QString &references);

    /*! \brief Sets extra MIME headers for the message.
        \param headers The extra header list.
    */
    void setExtraHeaders(const QList<KMime::Headers::Base *> &headers);
    /*! \brief Returns the extra MIME headers. */
    [[nodiscard]] QList<KMime::Headers::Base *> extraHeaders() const;

    /*! \brief Returns the ID of the mail transport to use for sending. */
    [[nodiscard]] int transportId() const;
    /*! \brief Sets the mail transport ID.
        \param tid The transport ID.
    */
    void setTransportId(int tid);

Q_SIGNALS:
    /*! \brief Emitted when the to-recipients list changes. */
    void toChanged();
    /*! \brief Emitted when the sender changes. */
    void fromChanged();
    /*! \brief Emitted when the CC recipients list changes. */
    void ccChanged();
    /*! \brief Emitted when the BCC recipients list changes. */
    void bccChanged();
    /*! \brief Emitted when the reply-to list changes. */
    void replyToChanged();
    /*! \brief Emitted when the subject changes. */
    void subjectChanged();
    /*! \brief Emitted when the urgent flag changes. */
    void urgentChanged();
    /*! \brief Emitted when the user agent changes. */
    void userAgentChanged();
    /*! \brief Emitted when the In-Reply-To header changes. */
    void inReplyToChanged();
    /*! \brief Emitted when the FCC folder changes. */
    void fccChanged();

private:
    class InfoPartPrivate;
    std::unique_ptr<InfoPartPrivate> const d;
};
} // namespace MessageComposer
