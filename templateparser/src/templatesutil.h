/*
  SPDX-FileCopyrightText: 2011-2021 Laurent Montel <montel@kde.org>

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
TEMPLATEPARSER_EXPORT Q_REQUIRED_RESULT QString getLastNameFromEmail(const QString &str);
TEMPLATEPARSER_EXPORT Q_REQUIRED_RESULT QString getFirstNameFromEmail(const QString &str);
}
}

