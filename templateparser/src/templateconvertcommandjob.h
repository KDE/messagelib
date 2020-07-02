/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATECONVERTCOMMANDJOB_H
#define TEMPLATECONVERTCOMMANDJOB_H

#include <QObject>
#include "templateparser_export.h"
#include <KMime/Message>

namespace TemplateParser {
/**
 * @brief The TemplateConvertCommandJob class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplateConvertCommandJob : public QObject
{
    Q_OBJECT
public:
    explicit TemplateConvertCommandJob(QObject *parent = nullptr);
    ~TemplateConvertCommandJob();
    Q_REQUIRED_RESULT QString convertText();

    Q_REQUIRED_RESULT QString currentText() const;
    void setCurrentText(const QString &currentText);

    Q_REQUIRED_RESULT KMime::Message::Ptr originalMessage() const;
    void setOriginalMessage(const KMime::Message::Ptr &originalMessage);

private:
    QString mCurrentText;
    KMime::Message::Ptr mOriginalMessage;
};
}

#endif // TEMPLATECONVERTCOMMANDJOB_H
