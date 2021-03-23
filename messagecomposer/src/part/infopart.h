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
 * @brief The InfoPart class
 */
class MESSAGECOMPOSER_EXPORT InfoPart : public MessageComposer::MessagePart
{
    Q_OBJECT

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

private:
    class Private;
    Private *const d;
};
} // namespace MessageComposer

