/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] KMime::Message::Ptr originalMessage() const;
    void setOriginalMessage(const KMime::Message::Ptr &originalMessage);

private:
    QString mCurrentText;
    KMime::Message::Ptr mOriginalMessage;
};
}
