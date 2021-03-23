/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <PimCommon/CustomToolsViewInterface>
#include <QDebug>

namespace KPIMTextEdit
{
class RichTextComposer;
}
namespace MessageComposer
{
class PluginEditorGrammarCustomToolsViewInterfacePrivate;
/**
 * @brief The PluginGrammarAction class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginGrammarAction
{
public:
    PluginGrammarAction();

    Q_REQUIRED_RESULT QString replacement() const;
    void setReplacement(const QString &replacement);

    Q_REQUIRED_RESULT int start() const;
    void setStart(int start);

    Q_REQUIRED_RESULT int length() const;
    void setLength(int length);

    Q_REQUIRED_RESULT QStringList suggestions() const;
    void setSuggestions(const QStringList &suggestions);

    Q_REQUIRED_RESULT int blockId() const;
    void setBlockId(int blockId);

    Q_REQUIRED_RESULT QStringList infoUrls() const;
    void setInfoUrls(const QStringList &urls);

private:
    QStringList mSuggestions;
    QStringList mInfoUrls;
    QString mReplacement;
    int mStart = -1;
    int mLength = -1;
    int mBlockId = -1;
};

/**
 * @brief The PluginEditorGrammarCustomToolsViewInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorGrammarCustomToolsViewInterface : public PimCommon::CustomToolsViewInterface
{
    Q_OBJECT
public:
    explicit PluginEditorGrammarCustomToolsViewInterface(QWidget *parent = nullptr);
    ~PluginEditorGrammarCustomToolsViewInterface() override;

    void setParentWidget(QWidget *parent);
    Q_REQUIRED_RESULT QWidget *parentWidget() const;

    Q_REQUIRED_RESULT KPIMTextEdit::RichTextComposer *richTextEditor() const;
    void setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor);

Q_SIGNALS:
    void replaceText(const MessageComposer::PluginGrammarAction &act);

private:
    PluginEditorGrammarCustomToolsViewInterfacePrivate *d = nullptr;
};
}
Q_DECLARE_METATYPE(MessageComposer::PluginGrammarAction)
MESSAGECOMPOSER_EXPORT QDebug operator<<(QDebug d, const MessageComposer::PluginGrammarAction &t);
