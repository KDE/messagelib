
/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef AUTOCRYPTSTORAGE_P_H
#define AUTOCRYPTSTORAGE_P_H

#include "autocryptrecipient.h"

#include <QDir>
#include <QHash>

namespace MessageCore
{
class AutocryptStoragePrivate
{
public:
    AutocryptStoragePrivate();
    QHash<QByteArray, AutocryptRecipient::Ptr> recipients;
    QDir basePath;
};

}
#endif
