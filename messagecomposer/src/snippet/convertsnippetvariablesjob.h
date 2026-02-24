/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
namespace MessageComposer
{
class ComposerViewInterface;
/*!
 * \class MessageComposer::ConvertSnippetVariablesJob
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/ConvertSnippetVariablesJob
 * \brief The ConvertSnippetVariablesJob class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT ConvertSnippetVariablesJob : public QObject
{
    Q_OBJECT
public:
    explicit ConvertSnippetVariablesJob(QObject *parent = nullptr);
    ~ConvertSnippetVariablesJob() override;
    /*! \brief Starts the conversion of snippet variables. */
    void start();

    /*! \brief Sets the text containing snippet variables to convert.
        \param str The text with variables to convert.
    */
    void setText(const QString &str);
    /*! \brief Returns the text that was converted. */
    [[nodiscard]] QString text() const;

    /*! \brief Returns the composer view interface used for variable conversion. */
    [[nodiscard]] MessageComposer::ComposerViewInterface *composerViewInterface() const;
    /*! \brief Sets the composer view interface for variable conversion.
        \param composerViewInterface The interface to use.
    */
    void setComposerViewInterface(MessageComposer::ComposerViewInterface *composerViewInterface);
    /*! \brief Converts snippet variables in the given text.
        \param composerView The composer view for variable context.
        \param text The text containing variables to convert.
        \return The converted text.
    */
    [[nodiscard]] static QString convertVariables(MessageComposer::ComposerViewInterface *composerView, const QString &text);

    /*! \brief Returns whether the job can be started. */
    [[nodiscard]] bool canStart() const;
Q_SIGNALS:
    void textConverted(const QString &str);

private:
    [[nodiscard]] MESSAGECOMPOSER_NO_EXPORT static QString convertVariables(const QString &cmd, qsizetype &i, QChar c);
    [[nodiscard]] MESSAGECOMPOSER_NO_EXPORT static QString getFirstNameFromEmail(const QString &address);
    [[nodiscard]] MESSAGECOMPOSER_NO_EXPORT static QString getLastNameFromEmail(const QString &address);
    [[nodiscard]] MESSAGECOMPOSER_NO_EXPORT static QString getNameFromEmail(const QString &address);
    QString mText;
    MessageComposer::ComposerViewInterface *mComposerViewInterface = nullptr;
};
}
