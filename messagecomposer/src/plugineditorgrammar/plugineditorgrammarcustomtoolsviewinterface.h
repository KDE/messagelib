/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] QString replacement() const;
    void setReplacement(const QString &replacement);

    [[nodiscard]] int start() const;
    void setStart(int start);

    [[nodiscard]] int length() const;
    void setLength(int length);

    [[nodiscard]] QStringList suggestions() const;
    void setSuggestions(const QStringList &suggestions);

    [[nodiscard]] int blockId() const;
    void setBlockId(int blockId);

    [[nodiscard]] QStringList infoUrls() const;
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
    [[nodiscard]] QWidget *parentWidget() const;

    [[nodiscard]] KPIMTextEdit::RichTextComposer *richTextEditor() const;
    void setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor);

Q_SIGNALS:
    void replaceText(const MessageComposer::PluginGrammarAction &act);

private:
    std::unique_ptr<PluginEditorGrammarCustomToolsViewInterfacePrivate> const d;
};
}
Q_DECLARE_METATYPE(MessageComposer::PluginGrammarAction)
MESSAGECOMPOSER_EXPORT QDebug operator<<(QDebug d, const MessageComposer::PluginGrammarAction &t);
