/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"
#include <KMime/Message>
#include <QObject>

namespace TemplateParser
{
/**
 * @brief The TemplateConvertCommandJob class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplateConvertCommandJob : public QObject
{
    Q_OBJECT
public:
    explicit TemplateConvertCommandJob(QObject *parent = nullptr);
    ~TemplateConvertCommandJob() override;
    [[nodiscard]] QString convertText();

    [[nodiscard]] QString currentText() const;
    void setCurrentText(const QString &currentText);

    [[nodiscard]] std::shared_ptr<KMime::Message> originalMessage() const;
    void setOriginalMessage(const std::shared_ptr<KMime::Message> &originalMessage);

private:
    QString mCurrentText;
    std::shared_ptr<KMime::Message> mOriginalMessage;
};
}
