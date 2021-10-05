/*
   SPDX-FileCopyrightText: 2020 Sandro Kanuß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autocryptstorage.h"
#include "autocryptrecipient_p.h"
#include "autocryptstorage_p.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QUrl>
#include <autocrypt_debug.h>

using namespace MessageCore;

AutocryptStoragePrivate::AutocryptStoragePrivate()
    : basePath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/autocrypt"))
{
}

AutocryptStorage::Ptr AutocryptStorage::mSelf = nullptr;

AutocryptStorage::Ptr AutocryptStorage::self()
{
    if (!mSelf) {
        mSelf = AutocryptStorage::Ptr(new AutocryptStorage());
    }

    return mSelf;
}

QString address2Filename(const QByteArray &addr)
{
    const auto url = QUrl::toPercentEncoding(QString::fromUtf8(addr));
    return QString::fromLatin1(url + ".json");
}

AutocryptStorage::AutocryptStorage()
    : d_ptr(new AutocryptStoragePrivate())
{
}

AutocryptRecipient::Ptr AutocryptStorage::getRecipient(const QByteArray &addr)
{
    Q_D(AutocryptStorage);
    if (d->recipients.contains(addr)) {
        return d->recipients.value(addr);
    }

    const QString fileName(address2Filename(addr));
    if (d->basePath.exists(fileName)) {
        QFile file(d->basePath.filePath(fileName));
        auto recipient = AutocryptRecipient::Ptr(new AutocryptRecipient);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return nullptr;
        }
        recipient->fromJson(file.readAll());
        d->recipients[addr] = recipient;
        return recipient;
    }
    return nullptr;
}

AutocryptRecipient::Ptr AutocryptStorage::addRecipient(const QByteArray &addr)
{
    Q_D(AutocryptStorage);

    auto recipient = getRecipient(addr);
    if (recipient) {
        return recipient;
    }

    recipient = AutocryptRecipient::Ptr(new AutocryptRecipient);
    recipient->d_func()->addr = addr;
    d->recipients[addr] = recipient;
    return recipient;
}

void AutocryptStorage::deleteRecipient(const QByteArray &addr)
{
    Q_D(AutocryptStorage);
    const QString fileName(address2Filename(addr));
    d->basePath.remove(fileName);
    d->recipients.remove(addr);
}

void AutocryptStorage::save()
{
    Q_D(AutocryptStorage);
    if (!d->basePath.exists()) {
        QDir parent = d->basePath;
        if (!parent.cdUp()) {
            qCWarning(AUTOCRYPT_LOG) << parent.absolutePath() << "does not exist. Cancel saving Autocrypt storage.";
            return;
        }

        if (!parent.mkdir(d->basePath.dirName())) {
            qCWarning(AUTOCRYPT_LOG) << "Cancel saving Autocrypt storage, because failed to create" << d->basePath.absolutePath();
            return;
        }
    }
    const auto keys = d->recipients.keys();
    for (const auto &addr : keys) {
        const auto recipient = d->recipients.value(addr);
        const QString fileName(address2Filename(addr));
        if (recipient->hasChanged() || !d->basePath.exists(fileName)) {
            QTemporaryFile file(d->basePath.path() + QLatin1Char('/'));
            if (!file.open()) {
                continue;
            }
            file.write(recipient->toJson(QJsonDocument::Compact));
            file.close();
            d->basePath.remove(fileName);
            file.rename(d->basePath.filePath(fileName));
            file.setAutoRemove(false);
            recipient->setChangedFlag(false);
        }
    }
}
