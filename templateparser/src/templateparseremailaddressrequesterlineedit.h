/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_private_export.h"
#include <TemplateParser/TemplateParserEmailAddressRequesterBase>
class QLineEdit;
namespace TemplateParser
{
class TEMPLATEPARSER_TESTS_EXPORT TemplateParserEmailAddressRequesterLineEdit : public TemplateParser::TemplateParserEmailAddressRequesterBase
{
    Q_OBJECT
public:
    explicit TemplateParserEmailAddressRequesterLineEdit(QWidget *parent = nullptr);
    ~TemplateParserEmailAddressRequesterLineEdit() override;

    [[nodiscard]] QString text() const override;
    void setText(const QString &str) override;
    void clear() override;

private:
    QLineEdit *const mLineEdit;
};
}
