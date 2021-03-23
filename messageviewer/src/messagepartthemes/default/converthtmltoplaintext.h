/*
  SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include <QString>
namespace MimeTreeParser
{
class ConvertHtmlToPlainText
{
public:
    ConvertHtmlToPlainText();
    ~ConvertHtmlToPlainText();

    Q_REQUIRED_RESULT QString generatePlainText();

    Q_REQUIRED_RESULT QString htmlString() const;
    void setHtmlString(const QString &htmlString);

private:
    static void toCleanPlainText(QString &text);
    QString mHtmlString;
};
}

