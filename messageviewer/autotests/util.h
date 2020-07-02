/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGEVIEWER_TESTS_UTIL_H
#define MESSAGEVIEWER_TESTS_UTIL_H

#include <KMime/Message>

namespace MessageViewer {
namespace Test {
KMime::Message::Ptr readAndParseMail(const QString &mailFile);
}
}

#endif //__MESSAGEVIEWER_TESTS_UTIL_H
