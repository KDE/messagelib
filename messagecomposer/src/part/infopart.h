/*
  Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#ifndef MESSAGECOMPOSER_INFOPART_H
#define MESSAGECOMPOSER_INFOPART_H

#include "messagepart.h"

#include <QStringList>

#include <kmime/kmime_message.h>
#include <kmime/kmime_headers.h>
namespace MessageComposer {
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

#endif // MESSAGECOMPOSER_INFOPART_H
