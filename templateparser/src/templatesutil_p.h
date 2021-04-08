/*
  SPDX-FileCopyrightText: 2011-2021 Laurent Montel <montel@kde.org>

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
Q_REQUIRED_RESULT QStringList keywords();
Q_REQUIRED_RESULT QStringList keywordsWithArgs();
TEMPLATEPARSER_TESTS_EXPORT Q_REQUIRED_RESULT QString removeSpaceAtBegin(const QString &selection);
}
}

