/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "composer/keyresolver.h"
#include <KContacts/Addressee>
class KJob;

namespace MessageComposer
{
class SaveContactPreferenceJob : public QObject
{
    Q_OBJECT
public:
    explicit SaveContactPreferenceJob(const QString &email, const Kleo::KeyResolver::ContactPreferences &pref, QObject *parent = nullptr);
    ~SaveContactPreferenceJob() override;
    void start();

private Q_SLOTS:
    void slotSearchContact(KJob *job);

    void slotModifyCreateItem(KJob *job);

private:
    void writeCustomContactProperties(KContacts::Addressee &contact, const Kleo::KeyResolver::ContactPreferences &pref) const;
    const QString mEmail;
    const Kleo::KeyResolver::ContactPreferences mPref;
};
}
