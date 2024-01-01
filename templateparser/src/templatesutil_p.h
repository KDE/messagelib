/*
  SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "templateparser_private_export.h"
#include <QString>
#include <QStringList>
namespace TemplateParser
{
namespace Util
{
[[nodiscard]] TEMPLATEPARSER_TESTS_EXPORT QStringList keywords();
[[nodiscard]] TEMPLATEPARSER_TESTS_EXPORT QStringList keywordsWithArgs();
[[nodiscard]] TEMPLATEPARSER_TESTS_EXPORT QString removeSpaceAtBegin(const QString &selection);
}
}
