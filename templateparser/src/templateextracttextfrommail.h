/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"
#include <QObject>
namespace TemplateParser
{
class TemplateWebEnginePage;
/**
 * @brief The TemplateExtractTextFromMail class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplateExtractTextFromMail : public QObject
{
    Q_OBJECT
public:
    explicit TemplateExtractTextFromMail(QObject *parent = nullptr);
    ~TemplateExtractTextFromMail() override;

    void setHtmlContent(const QString &html);

    [[nodiscard]] QString plainText() const;

Q_SIGNALS:
    void loadContentDone(bool success);

private:
    TEMPLATEPARSER_NO_EXPORT void slotLoadFinished(bool ok);
    TEMPLATEPARSER_NO_EXPORT void setPlainText(const QString &plainText);

    QString mExtractedPlainText;
    TemplateWebEnginePage *const mPage;
};
}
