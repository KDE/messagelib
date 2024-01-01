/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"
#include <QObject>
#include <QString>
struct TEMPLATEPARSER_EXPORT TemplateParserExtractHtmlInfoResult {
    void clear();
    QString mBodyElement;
    QString mHeaderElement;
    QString mHtmlElement;
    QString mPlainText;
    QString mTemplate;
};
Q_DECLARE_METATYPE(TemplateParserExtractHtmlInfoResult)
