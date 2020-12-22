/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATEEXTRACTHTMLELEMENTFROMMAIL_H
#define TEMPLATEEXTRACTHTMLELEMENTFROMMAIL_H

#include "templateparser_private_export.h"
#include <QObject>
#include <QString>
namespace TemplateParser {
class TemplateWebEnginePage;
class TEMPLATEPARSER_TESTS_EXPORT TemplateExtractHtmlElementFromMail : public QObject
{
    Q_OBJECT
public:
    explicit TemplateExtractHtmlElementFromMail(QObject *parent = nullptr);
    ~TemplateExtractHtmlElementFromMail() override;

    Q_REQUIRED_RESULT QString bodyElement() const;

    Q_REQUIRED_RESULT QString headerElement() const;

    Q_REQUIRED_RESULT QString htmlElement() const;

    void setHtmlContent(const QString &html);

Q_SIGNALS:
    void loadContentDone(bool success);

private:
    void clear();
    void slotLoadFinished(bool success);
    void handleHtmlInfo(const QVariant &result);

    QString mBodyElement;
    QString mHeaderElement;
    QString mHtmlElement;

    TemplateWebEnginePage *mPage = nullptr;
};
}
#endif // TEMPLATEEXTRACTHTMLELEMENTFROMMAIL_H
