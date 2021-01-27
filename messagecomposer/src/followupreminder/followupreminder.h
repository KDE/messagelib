/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_FOLLOWUPREMINDER_H_
#define MESSAGECOMPOSER_FOLLOWUPREMINDER_H_

#include "messagecomposer_export.h"

#include <QtGlobal>

namespace MessageComposer
{
namespace FollowUpReminder
{
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool isAvailableAndEnabled();
}
}

#endif
