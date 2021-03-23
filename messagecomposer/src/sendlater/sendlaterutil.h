/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"

#include <QtGlobal>

namespace MessageComposer
{
/** Send later utilities. */
namespace SendLaterUtil
{
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool sentLaterAgentWasRegistered();

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool sentLaterAgentEnabled();
}
}
