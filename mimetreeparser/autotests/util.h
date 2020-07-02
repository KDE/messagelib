/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef AUTOTESTS_UTIL_H
#define AUTOTESTS_UTIL_H

#include <KMime/Message>

KMime::Message::Ptr readAndParseMail(const QString &mailFile);

#endif
