/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef AUTOCRYPTRECIPIENT_H
#define AUTOCRYPTRECIPIENT_H

#include <KMime/Headers>
#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/MessagePart>

#include <gpgme++/data.h>

#include <QJsonDocument>

namespace MessageCore {

class AutocryptRecipientPrivate;
class AutocryptStorage;

class HeaderMixupNodeHelper
{
public:
    HeaderMixupNodeHelper(MimeTreeParser::NodeHelper *n, KMime::Content *m);

    Q_REQUIRED_RESULT bool hasMailHeader(const char *header) const;
    KMime::Headers::Base const *mailHeaderAsBase(const char *header) const;
    QSharedPointer<KMime::Headers::Generics::AddressList> mailHeaderAsAddressList(const char *header) const;
    Q_REQUIRED_RESULT QDateTime dateHeader() const;
    Q_REQUIRED_RESULT QVector<MimeTreeParser::MessagePart::Ptr> messagePartsOfMailHeader(const char *header) const;
    Q_REQUIRED_RESULT QVector<KMime::Headers::Base *> headers(const char* headerType) const;

public:
    KMime::Content *message;
private:
    MimeTreeParser::NodeHelper *nodeHelper;
};

class AutocryptRecipient
{
public:
    typedef QSharedPointer<AutocryptRecipient> Ptr;
    AutocryptRecipient();
    void updateFromMessage(const HeaderMixupNodeHelper &mixup, const KMime::Headers::Base *header);
    void updateFromGossip(const HeaderMixupNodeHelper& mixup, const KMime::Headers::Base* header);

    QByteArray toJson(QJsonDocument::JsonFormat format) const;
    void fromJson(const QByteArray &json);

    bool hasChanged() const;
    void setChangedFlag(bool changed);

    GpgME::Key gpgKey() const;
    GpgME::Key gossipKey() const;

private:
    std::unique_ptr<AutocryptRecipientPrivate> d_ptr;
    Q_DECLARE_PRIVATE(AutocryptRecipient)

    friend class AutocryptStorage;
};
}
#endif
