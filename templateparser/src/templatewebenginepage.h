/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATEWEBENGINEPAGE_H
#define TEMPLATEWEBENGINEPAGE_H

#include <QWebEnginePage>
namespace TemplateParser
{
class TemplateWebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit TemplateWebEnginePage(QObject *parent = nullptr);
    ~TemplateWebEnginePage() override;
};
}

#endif // TEMPLATEWEBENGINEPAGE_H
