/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_private_export.h"
#include <QWidget>
namespace TemplateParser
{
class TemplateParserEmailAddressRequesterBase;
class TEMPLATEPARSER_TESTS_EXPORT TemplateParserEmailAddressRequesterInterfaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateParserEmailAddressRequesterInterfaceWidget(QWidget *parent = nullptr);
    ~TemplateParserEmailAddressRequesterInterfaceWidget() override = default;

    Q_REQUIRED_RESULT QString text() const;
    void setText(const QString &str);
    void clear();

Q_SIGNALS:
    void textChanged();

private:
    void initializeEmailWidget();
    TemplateParser::TemplateParserEmailAddressRequesterBase *mTemplateParserEmailBase = nullptr;
};
}
