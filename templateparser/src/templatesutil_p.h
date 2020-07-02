/*
  SPDX-FileCopyrightText: 2011-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TEMPLATEPARSER_TEMPLATESUTIL_P_H
#define TEMPLATEPARSER_TEMPLATESUTIL_P_H

#include "templateparser_private_export.h"
#include <QString>
class QString;
class QStringList;
namespace TemplateParser {
namespace Util {
Q_REQUIRED_RESULT QStringList keywords();
Q_REQUIRED_RESULT QStringList keywordsWithArgs();
TEMPLATEPARSER_TESTS_EXPORT Q_REQUIRED_RESULT QString removeSpaceAtBegin(const QString &selection);
}
}

#endif
