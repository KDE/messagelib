/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef AUTOCRYPTSTORAGE_H
#define AUTOCRYPTSTORAGE_H

#include "messagecore_export.h"

#include "autocryptrecipient.h"

#include <QSharedPointer>

class AutocryptStorageTest;
class AutocryptUtilsTest;

namespace MessageCore
{
class AutocryptStoragePrivate;

class MESSAGECORE_EXPORT AutocryptStorage
{
public:
    typedef QSharedPointer<AutocryptStorage> Ptr;
    static AutocryptStorage::Ptr self();

    AutocryptRecipient::Ptr getRecipient(const QByteArray &addr);
    void save();
    AutocryptRecipient::Ptr addRecipient(const QByteArray &addr);
    void deleteRecipient(const QByteArray &addr);

private:
    AutocryptStorage();
    static AutocryptStorage::Ptr mSelf;

private:
    std::unique_ptr<AutocryptStoragePrivate> d_ptr;
    Q_DECLARE_PRIVATE(AutocryptStorage)

    friend class ::AutocryptStorageTest;
    friend class ::AutocryptUtilsTest;
};
}

#endif
