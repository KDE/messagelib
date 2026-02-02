/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"
#include <QWidget>
namespace TemplateParser
{
/*!
 * \class TemplateParser::TemplateParserEmailAddressRequesterBase
 * \inmodule TemplateParser
 * \inheaderfile TemplateParser/TemplateParserEmailAddressRequesterBase
 * \brief The TemplateParserEmailAddressRequesterBase class
 * \author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplateParserEmailAddressRequesterBase : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent parent widget
     */
    explicit TemplateParserEmailAddressRequesterBase(QWidget *parent = nullptr);
    /*!
     * \brief Destructor
     */
    ~TemplateParserEmailAddressRequesterBase() override;

    /*!
     */
    /*!
     * \brief Returns the text content
     * \return email address text
     */
    [[nodiscard]] virtual QString text() const;
    /*!
     * \brief Sets the text content
     * \param str email address text
     */
    virtual void setText(const QString &str);
    /*!
     * \brief Clears the content
     */
    virtual void clear();
Q_SIGNALS:
    /*!
     * \brief Emitted when text changes
     */
    void textChanged();
};
}
