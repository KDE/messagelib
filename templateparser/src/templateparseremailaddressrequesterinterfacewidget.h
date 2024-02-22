/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] QString text() const;
    void setText(const QString &str);
    void clear();

Q_SIGNALS:
    void textChanged();

private:
    TEMPLATEPARSER_NO_EXPORT void initializeEmailWidget();
    TemplateParser::TemplateParserEmailAddressRequesterBase *mTemplateParserEmailBase = nullptr;
};
}
