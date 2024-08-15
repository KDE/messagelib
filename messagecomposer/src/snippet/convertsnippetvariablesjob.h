/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

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
    [[nodiscard]] QString text() const;

    [[nodiscard]] MessageComposer::ComposerViewInterface *composerViewInterface() const;
    void setComposerViewInterface(MessageComposer::ComposerViewInterface *composerViewInterface);
    [[nodiscard]] static QString convertVariables(MessageComposer::ComposerViewInterface *composerView, const QString &text);

    [[nodiscard]] bool canStart() const;
Q_SIGNALS:
    void textConverted(const QString &str);

private:
    [[nodiscard]] static QString convertVariables(const QString &cmd, int &i, QChar c);
    [[nodiscard]] static QString getFirstNameFromEmail(const QString &address);
    [[nodiscard]] static QString getLastNameFromEmail(const QString &address);
    [[nodiscard]] static QString getNameFromEmail(const QString &address);
    QString mText;
    MessageComposer::ComposerViewInterface *mComposerViewInterface = nullptr;
};
}
