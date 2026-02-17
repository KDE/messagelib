/*
   SPDX-FileCopyrightText: 2013-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"

namespace MessageComposer
{
/*! \namespace SendLaterUtil
    \inmodule MessageComposer
    \brief Utility functions for delayed message sending.

    Provides functions to check if the send-later feature is available and enabled.
*/
namespace SendLaterUtil
{
/*! \brief Checks if the send-later agent has been registered.
    \return True if the send-later agent is registered, false otherwise.
*/
[[nodiscard]] MESSAGECOMPOSER_EXPORT bool sentLaterAgentWasRegistered();

/*! \brief Checks if the send-later agent is enabled.
    \return True if the send-later agent is enabled, false otherwise.
*/
[[nodiscard]] MESSAGECOMPOSER_EXPORT bool sentLaterAgentEnabled();
}
}
