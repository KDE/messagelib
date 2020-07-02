/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_SENDLATERUTIL_H
#define MESSAGECOMPOSER_SENDLATERUTIL_H

#include "messagecomposer_export.h"

#include <QtGlobal>

namespace MessageComposer {
/** Send later utilities. */
namespace SendLaterUtil {
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool sentLaterAgentWasRegistered();

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool sentLaterAgentEnabled();
}
}
#endif // SENDLATERUTIL_H
