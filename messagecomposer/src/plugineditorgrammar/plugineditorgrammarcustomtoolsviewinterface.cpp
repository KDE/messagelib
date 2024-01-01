/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "plugineditorgrammarcustomtoolsviewinterface.h"
#include <KPIMTextEdit/RichTextComposer>
using namespace MessageComposer;
class MessageComposer::PluginEditorGrammarCustomToolsViewInterfacePrivate
{
public:
    QWidget *mParentWidget = nullptr;
    KPIMTextEdit::RichTextComposer *mEditor = nullptr;
};

PluginEditorGrammarCustomToolsViewInterface::PluginEditorGrammarCustomToolsViewInterface(QWidget *parent)
    : PimCommon::CustomToolsViewInterface(parent)
    , d(new PluginEditorGrammarCustomToolsViewInterfacePrivate)
{
}

PluginEditorGrammarCustomToolsViewInterface::~PluginEditorGrammarCustomToolsViewInterface() = default;

void PluginEditorGrammarCustomToolsViewInterface::setParentWidget(QWidget *parent)
{
    d->mParentWidget = parent;
}

QWidget *PluginEditorGrammarCustomToolsViewInterface::parentWidget() const
{
    return d->mParentWidget;
}

KPIMTextEdit::RichTextComposer *PluginEditorGrammarCustomToolsViewInterface::richTextEditor() const
{
    return d->mEditor;
}

void PluginEditorGrammarCustomToolsViewInterface::setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor)
{
    d->mEditor = richTextEditor;
}

PluginGrammarAction::PluginGrammarAction() = default;

QString PluginGrammarAction::replacement() const
{
    return mReplacement;
}

void PluginGrammarAction::setReplacement(const QString &replacement)
{
    mReplacement = replacement;
}

int PluginGrammarAction::start() const
{
    return mStart;
}

void PluginGrammarAction::setStart(int start)
{
    mStart = start;
}

int PluginGrammarAction::length() const
{
    return mLength;
}

void PluginGrammarAction::setLength(int end)
{
    mLength = end;
}

QStringList PluginGrammarAction::suggestions() const
{
    return mSuggestions;
}

void PluginGrammarAction::setSuggestions(const QStringList &suggestions)
{
    mSuggestions = suggestions;
}

int PluginGrammarAction::blockId() const
{
    return mBlockId;
}

void PluginGrammarAction::setBlockId(int blockId)
{
    mBlockId = blockId;
}

QStringList PluginGrammarAction::infoUrls() const
{
    return mInfoUrls;
}

void PluginGrammarAction::setInfoUrls(const QStringList &urls)
{
    mInfoUrls = urls;
}

QDebug operator<<(QDebug d, const PluginGrammarAction &t)
{
    d << "start " << t.start();
    d << "length " << t.length();
    d << "blockId " << t.blockId();
    d << "suggestion " << t.suggestions();
    d << "replacement " << t.replacement();
    d << "urls " << t.infoUrls();
    return d;
}

#include "moc_plugineditorgrammarcustomtoolsviewinterface.cpp"
