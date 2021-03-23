
/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

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
