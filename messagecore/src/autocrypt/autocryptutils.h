/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef AUTOCRYPTUTILS_H
#define AUTOCRYPTUTILS_H

#include "autocryptrecipient.h"

#include "messagecore_export.h"

namespace MessageCore {
QHash<QByteArray,QByteArray> MESSAGECORE_EXPORT paramsFromAutocryptHeader(const KMime::Headers::Base *const header);
void MESSAGECORE_EXPORT processAutocryptfromMail(const HeaderMixupNodeHelper& mixup);

}

#endif
