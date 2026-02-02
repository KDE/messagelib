/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"
#include <KMime/Message>
#include <QObject>

namespace TemplateParser
{
/*!
 * \class TemplateParser::TemplateConvertCommandJob
 * \inmodule TemplateParser
 * \inheaderfile TemplateParser/TemplateConvertCommandJob
 * \brief The TemplateConvertCommandJob class
 * \author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplateConvertCommandJob : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent parent object
     */
    explicit TemplateConvertCommandJob(QObject *parent = nullptr);
    /*!
     * \brief Destructor
     */
    ~TemplateConvertCommandJob() override;
    /*!
     */
    /*!
     * \brief Converts the current text
     * \return converted text
     */
    [[nodiscard]] QString convertText();

    /*!
     * \brief Returns the current text
     * \return current text
     */
    [[nodiscard]] QString currentText() const;
    /*!
     * \brief Sets the current text
     * \param currentText text to set
     */
    void setCurrentText(const QString &currentText);

    /*!
     */
    /*!
     * \brief Returns the original message
     * \return KMime::Message shared pointer
     */
    [[nodiscard]] std::shared_ptr<KMime::Message> originalMessage() const;
    /*!
     * \brief Sets the original message
     * \param originalMessage KMime::Message shared pointer
     */
    void setOriginalMessage(const std::shared_ptr<KMime::Message> &originalMessage);

private:
    QString mCurrentText;
    std::shared_ptr<KMime::Message> mOriginalMessage;
};
}
