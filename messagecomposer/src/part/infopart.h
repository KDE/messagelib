/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagepart.h"

#include <QStringList>

#include <kmime/kmime_headers.h>
#include <kmime/kmime_message.h>
namespace MessageComposer
{
/**
 * @brief The InfoPart class contains the message header.
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
    explicit InfoPart(QObject *parent = nullptr);
    ~InfoPart() override;

    Q_REQUIRED_RESULT QString from() const;
    void setFrom(const QString &from);
    Q_REQUIRED_RESULT QStringList to() const;
    void setTo(const QStringList &to);
    Q_REQUIRED_RESULT QStringList cc() const;
    void setCc(const QStringList &cc);
    Q_REQUIRED_RESULT QStringList bcc() const;
    void setBcc(const QStringList &bcc);

    Q_REQUIRED_RESULT QStringList replyTo() const;
    void setReplyTo(const QStringList &replyTo);

    Q_REQUIRED_RESULT QString subject() const;
    void setSubject(const QString &subject);

    Q_REQUIRED_RESULT QString fcc() const;
    void setFcc(const QString &fcc);

    Q_REQUIRED_RESULT QString userAgent() const;
    void setUserAgent(const QString &userAgent);

    Q_REQUIRED_RESULT bool urgent() const;
    void setUrgent(bool);

    Q_REQUIRED_RESULT QString inReplyTo() const;
    void setInReplyTo(const QString &inReplyTo);

    Q_REQUIRED_RESULT QString references() const;
    void setReferences(const QString &references);

    void setExtraHeaders(const KMime::Headers::Base::List &headers);
    Q_REQUIRED_RESULT KMime::Headers::Base::List extraHeaders() const;

    Q_REQUIRED_RESULT int transportId() const;
    void setTransportId(int tid);

Q_SIGNALS:
    void toChanged();
    void fromChanged();
    void ccChanged();
    void bccChanged();
    void replyToChanged();
    void subjectChanged();
    void urgentChanged();
    void userAgentChanged();
    void inReplyToChanged();
    void fccChanged();

private:
    class Private;
    Private *const d;
};
} // namespace MessageComposer

