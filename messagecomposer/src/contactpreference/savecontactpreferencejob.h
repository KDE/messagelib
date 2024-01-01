/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

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

class MESSAGECOMPOSER_EXPORT SaveContactPreferenceJob : public QObject
{
    Q_OBJECT
public:
    explicit SaveContactPreferenceJob(const QString &email, const ContactPreference &pref, QObject *parent = nullptr);
    ~SaveContactPreferenceJob() override;
    void start();

private:
    MESSAGECOMPOSER_NO_EXPORT void slotSearchContact(KJob *job);
    MESSAGECOMPOSER_NO_EXPORT void slotModifyCreateItem(KJob *job);

    std::unique_ptr<SaveContactPreferenceJobPrivate> d_ptr;
    Q_DECLARE_PRIVATE(SaveContactPreferenceJob)
};
}
