/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"
#include <QObject>
namespace TemplateParser
{
class TemplateWebEnginePage;
/*!
 * \class TemplateParser::TemplateExtractTextFromMail
 * \inmodule TemplateParser
 * \inheaderfile TemplateParser/TemplateExtractTextFromMail
 * \brief The TemplateExtractTextFromMail class
 * \author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplateExtractTextFromMail : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent parent object
     */
    explicit TemplateExtractTextFromMail(QObject *parent = nullptr);
    /*!
     * \brief Destructor
     */
    ~TemplateExtractTextFromMail() override;

    /*!
     * \brief Sets the HTML content to extract from
     * \param html HTML content string
     */
    void setHtmlContent(const QString &html);

    /*!
     */
    /*!
     * \brief Returns the extracted plain text
     * \return plain text string
     */
    [[nodiscard]] QString plainText() const;

Q_SIGNALS:
    /*!
     * \brief Emitted when content loading is done
     * \param success whether the operation was successful
     */
    void loadContentDone(bool success);

private:
    TEMPLATEPARSER_NO_EXPORT void slotLoadFinished(bool ok);
    TEMPLATEPARSER_NO_EXPORT void setPlainText(const QString &plainText);

    QString mExtractedPlainText;
    TemplateWebEnginePage *const mPage;
};
}
