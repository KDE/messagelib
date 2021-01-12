/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SAVECONTACTPREFERENCEJOB_H
#define SAVECONTACTPREFERENCEJOB_H

#include <KJob>
#include <KContacts/Addressee>
#include "composer/keyresolver.h"

namespace MessageComposer {
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
    QString mEmail;
    Kleo::KeyResolver::ContactPreferences mPref;
};
}
#endif // SAVECONTACTPREFERENCEJOB_H
