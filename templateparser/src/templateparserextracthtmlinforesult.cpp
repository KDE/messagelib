/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparserextracthtmlinforesult.h"

void TemplateParserExtractHtmlInfoResult::clear()
{
    mPlainText.clear();
    mBodyElement.clear();
    mHeaderElement.clear();
    mHtmlElement.clear();
    mTemplate.clear();
}
