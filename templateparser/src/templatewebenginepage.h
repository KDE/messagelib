/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

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
