/*
   SPDX-FileCopyrightText: 2014-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <MessageComposer/ContactPreference>

#include <QObject>

#include <memory>

#include "messagecomposer_export.h"

class KJob;

namespace MessageComposer
{
class SaveContactPreferenceJobPrivate;

/*! \class SaveContactPreferenceJob
    \inheaderfile savecontactpreferencejob.h
    \brief Asynchronous job to save a contact's cryptographic preferences.
    \inmodule MessageComposer

    This job saves cryptographic preferences (encryption and signing settings) for a contact
    asynchronously, first searching for the contact in Akonadi and then modifying or creating
    the contact with the preference data.
*/
class MESSAGECOMPOSER_EXPORT SaveContactPreferenceJob : public QObject
{
    Q_OBJECT
public:
    /*! \brief Constructs a SaveContactPreferenceJob to save preferences for a contact.
        \param email The email address of the contact.
        \param pref The cryptographic preferences to save.
        \param parent The parent QObject.
    */
    explicit SaveContactPreferenceJob(const QString &email, const ContactPreference &pref, QObject *parent = nullptr);

    /*! \brief Destroys the SaveContactPreferenceJob. */
    ~SaveContactPreferenceJob() override;

    /*! \brief Starts the asynchronous operation to save the contact preferences. */
    void start();

private:
    MESSAGECOMPOSER_NO_EXPORT void slotSearchContact(KJob *job);
    MESSAGECOMPOSER_NO_EXPORT void slotModifyCreateItem(KJob *job);

    std::unique_ptr<SaveContactPreferenceJobPrivate> d_ptr;
    Q_DECLARE_PRIVATE(SaveContactPreferenceJob)
};
}
