/*
  SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"
#include <QString>

namespace TemplateParser
{
namespace Util
{
TEMPLATEPARSER_EXPORT void deleteTemplate(const QString &id);
[[nodiscard]] TEMPLATEPARSER_EXPORT QString getLastNameFromEmail(const QString &str);
[[nodiscard]] TEMPLATEPARSER_EXPORT QString getFirstNameFromEmail(const QString &str);
}
}
