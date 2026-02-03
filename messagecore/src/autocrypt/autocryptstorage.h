/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

#include "autocryptrecipient.h"

#include <QSharedPointer>

class AutocryptStorageTest;
class AutocryptUtilsTest;

namespace MessageCore
{
class AutocryptStoragePrivate;

/*!
 * \class AutocryptStorage
 * \inmodule MessageCore
 * \inheaderfile MessageCore/AutocryptStorage
 */
class MESSAGECORE_EXPORT AutocryptStorage
{
public:
    /*!
     * Defines a pointer to an AutocryptStorage object.
     */
    using Ptr = QSharedPointer<AutocryptStorage>;
    /*!
     * Returns the singleton instance of AutocryptStorage.
     */
    static AutocryptStorage::Ptr self();

    /*!
     * Returns the AutocryptRecipient for the given email address.
     *
     * \a addr The email address of the recipient.
     */
    AutocryptRecipient::Ptr getRecipient(const QByteArray &addr);
    /*!
     * Saves the storage.
     */
    void save();
    /*!
     * Adds a new recipient to the storage.
     *
     * \a addr The email address of the recipient to add.
     */
    AutocryptRecipient::Ptr addRecipient(const QByteArray &addr);
    /*!
     * Deletes a recipient from the storage.
     *
     * \a addr The email address of the recipient to delete.
     */
    void deleteRecipient(const QByteArray &addr);

    /*!
     * Destroys the AutocryptStorage.
     */
    ~AutocryptStorage();

private:
    MESSAGECORE_NO_EXPORT AutocryptStorage();
    static AutocryptStorage::Ptr mSelf;

private:
    std::unique_ptr<AutocryptStoragePrivate> d_ptr;
    Q_DECLARE_PRIVATE(AutocryptStorage)

    friend class ::AutocryptStorageTest;
    friend class ::AutocryptUtilsTest;
};
}
