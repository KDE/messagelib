/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
namespace MessageComposer
{
class ComposerViewInterface;
/**
 * @brief The ConvertSnippetVariablesJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT ConvertSnippetVariablesJob : public QObject
{
    Q_OBJECT
public:
    explicit ConvertSnippetVariablesJob(QObject *parent = nullptr);
    ~ConvertSnippetVariablesJob() override;
    void start();

    void setText(const QString &str);
    Q_REQUIRED_RESULT QString text() const;

    MessageComposer::ComposerViewInterface *composerViewInterface() const;
    void setComposerViewInterface(MessageComposer::ComposerViewInterface *composerViewInterface);
    static Q_REQUIRED_RESULT QString convertVariables(MessageComposer::ComposerViewInterface *composerView, const QString &text);

    Q_REQUIRED_RESULT bool canStart() const;
Q_SIGNALS:
    void textConverted(const QString &str);

private:
    static Q_REQUIRED_RESULT QString convertVariables(const QString &cmd, int &i, QChar c);
    static Q_REQUIRED_RESULT QString getFirstNameFromEmail(const QString &address);
    static Q_REQUIRED_RESULT QString getLastNameFromEmail(const QString &address);
    static Q_REQUIRED_RESULT QString getNameFromEmail(const QString &address);
    QString mText;
    MessageComposer::ComposerViewInterface *mComposerViewInterface = nullptr;
};
}
