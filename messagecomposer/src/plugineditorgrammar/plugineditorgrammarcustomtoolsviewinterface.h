/*
   Copyright (C) 2019 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PLUGINEDITORGRAMMARCUSTOMTOOLSVIEWINTERFACE_H
#define PLUGINEDITORGRAMMARCUSTOMTOOLSVIEWINTERFACE_H

#include <PimCommon/CustomToolsViewInterface>
#include "messagecomposer_export.h"
#include <QDebug>
namespace KPIMTextEdit {
class RichTextComposer;
}
namespace MessageComposer {
class PluginEditorGrammarCustomToolsViewInterfacePrivate;

class MESSAGECOMPOSER_EXPORT PluginGrammarAction
{
public:
    PluginGrammarAction();

    Q_REQUIRED_RESULT QString replacement() const;
    void setReplacement(const QString &replacement);

    Q_REQUIRED_RESULT int start() const;
    void setStart(int start);

    Q_REQUIRED_RESULT int end() const;
    void setEnd(int end);

    Q_REQUIRED_RESULT QStringList suggestions() const;
    void setSuggestions(const QStringList &suggestions);

    Q_REQUIRED_RESULT int blockId() const;
    void setBlockId(int blockId);

private:
    QStringList mSuggestions;
    QString mReplacement;
    int mStart = -1;
    int mEnd = -1;
    int mBlockId = -1;
};

class MESSAGECOMPOSER_EXPORT PluginEditorGrammarCustomToolsViewInterface : public PimCommon::CustomToolsViewInterface
{
    Q_OBJECT
public:
    explicit PluginEditorGrammarCustomToolsViewInterface(QWidget *parent = nullptr);
    ~PluginEditorGrammarCustomToolsViewInterface();


    void setParentWidget(QWidget *parent);
    Q_REQUIRED_RESULT QWidget *parentWidget() const;

    Q_REQUIRED_RESULT KPIMTextEdit::RichTextComposer *richTextEditor() const;
    void setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor);

Q_SIGNALS:
    void replaceText(const MessageComposer::PluginGrammarAction &act);

private:
    PluginEditorGrammarCustomToolsViewInterfacePrivate *const d;
};
}
Q_DECLARE_METATYPE(MessageComposer::PluginGrammarAction)
MESSAGECOMPOSER_EXPORT QDebug operator <<(QDebug d, const MessageComposer::PluginGrammarAction &t);
#endif // PLUGINEDITORGRAMMARCUSTOMTOOLSVIEWINTERFACE_H
