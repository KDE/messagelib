/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef AUTOCRYPTRECIPIENT_H
#define AUTOCRYPTRECIPIENT_H

#include <KMime/Headers>
#include <MimeTreeParser/NodeHelper>

#include <gpgme++/data.h>

#include <QJsonDocument>

class AutocryptRecipientPrivate;
class AutocryptRecipientTest;

namespace MessageCore {

class HeaderMixupNodeHelper
{
public:
    HeaderMixupNodeHelper(MimeTreeParser::NodeHelper *n, KMime::Content *m);

    Q_REQUIRED_RESULT bool hasMailHeader(const char *header) const;
    KMime::Headers::Base const *mailHeaderAsBase(const char *header) const;
    QSharedPointer<KMime::Headers::Generics::AddressList> mailHeaderAsAddressList(const char *header) const;
    Q_REQUIRED_RESULT QDateTime dateHeader() const;

public:
    KMime::Content *message;
private:
    MimeTreeParser::NodeHelper *nodeHelper;
};

class AutocryptRecipient
{
public:
    AutocryptRecipient();
    void updateFromMessage(const HeaderMixupNodeHelper &mixup);
    QByteArray toJson(QJsonDocument::JsonFormat format) const;

    GpgME::Key gpgKey();

private:
    AutocryptRecipientPrivate *d_ptr;
    Q_DECLARE_PRIVATE(AutocryptRecipient)

    friend class AutocryptRecipientTest;
};
}
#endif
