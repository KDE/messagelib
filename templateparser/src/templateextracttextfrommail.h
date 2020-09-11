/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATEEXTRACTTEXTFROMMAIL_H
#define TEMPLATEEXTRACTTEXTFROMMAIL_H

#include "templateparser_export.h"
#include <QObject>
namespace TemplateParser {
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
    ~TemplateExtractTextFromMail();

    void setHtmlContent(const QString &html);

    Q_REQUIRED_RESULT QString plainText() const;

Q_SIGNALS:
    void loadContentDone(bool success);

private:
    void slotLoadFinished(bool ok);
    void setPlainText(const QString &plainText);

    QString mExtractedPlainText;
    TemplateWebEnginePage *const mPage;
};
}
#endif // TEMPLATEEXTRACTTEXTFROMMAIL_H
