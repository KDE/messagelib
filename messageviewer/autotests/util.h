/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMime/Message>

namespace MessageViewer
{
namespace Test
{
std::shared_ptr<KMime::Message> readAndParseMail(const QString &mailFile);
}
}
