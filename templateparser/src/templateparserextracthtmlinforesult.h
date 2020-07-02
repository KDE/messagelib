/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATEPARSEREXTRACTHTMLINFORESULT_H
#define TEMPLATEPARSEREXTRACTHTMLINFORESULT_H

#include "templateparser_export.h"
#include <QString>
#include <QObject>
struct TEMPLATEPARSER_EXPORT TemplateParserExtractHtmlInfoResult {
    void clear();
    QString mBodyElement;
    QString mHeaderElement;
    QString mHtmlElement;
    QString mPlainText;
    QString mTemplate;
};
Q_DECLARE_METATYPE(TemplateParserExtractHtmlInfoResult)
#endif // TEMPLATEPARSEREXTRACTHTMLINFORESULT_H
