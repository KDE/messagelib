/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"
#include <QWidget>
namespace TemplateParser
{
/**
 * @brief The TemplateParserEmailAddressRequesterBase class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplateParserEmailAddressRequesterBase : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateParserEmailAddressRequesterBase(QWidget *parent = nullptr);
    ~TemplateParserEmailAddressRequesterBase() override;

    [[nodiscard]] virtual QString text() const;
    virtual void setText(const QString &str);
    virtual void clear();
Q_SIGNALS:
    void textChanged();
};
}
