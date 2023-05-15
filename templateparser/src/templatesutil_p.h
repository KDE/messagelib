/*
  SPDX-FileCopyrightText: 2011-2023 Laurent Montel <montel@kde.org>

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
Q_REQUIRED_RESULT TEMPLATEPARSER_TESTS_EXPORT QStringList keywords();
Q_REQUIRED_RESULT TEMPLATEPARSER_TESTS_EXPORT QStringList keywordsWithArgs();
Q_REQUIRED_RESULT TEMPLATEPARSER_TESTS_EXPORT QString removeSpaceAtBegin(const QString &selection);
}
}
