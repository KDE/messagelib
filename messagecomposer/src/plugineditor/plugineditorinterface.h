/*
   Copyright (C) 2015-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PLUGINEDITORINTERFACE_H
#define PLUGINEDITORINTERFACE_H

#include <QObject>
#include <PimCommon/AbstractGenericPluginInterface>
#include <MessageComposer/PluginActionType>
#include "messagecomposer_export.h"

class QKeyEvent;
namespace KPIMTextEdit {
class RichTextEditor;
}

namespace MessageComposer {
class PluginEditorInterfacePrivate;
class PluginEditor;
class PluginComposerInterface;
/**
 * @brief The PluginEditorInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorInterface : public PimCommon::AbstractGenericPluginInterface
{
    Q_OBJECT
public:
    explicit PluginEditorInterface(QObject *parent = nullptr);
    ~PluginEditorInterface();

    void setActionType(PluginActionType type);
    Q_REQUIRED_RESULT PluginActionType actionType() const;

    Q_REQUIRED_RESULT KPIMTextEdit::RichTextEditor *richTextEditor() const;
    void setRichTextEditor(KPIMTextEdit::RichTextEditor *richTextEditor);

    void setNeedSelectedText(bool b);
    Q_REQUIRED_RESULT bool needSelectedText() const;

    void setStatusBarWidget(QWidget *w);
    Q_REQUIRED_RESULT QWidget *statusBarWidget() const;

    Q_REQUIRED_RESULT MessageComposer::PluginComposerInterface *composerInterface() const;
    void setComposerInterface(MessageComposer::PluginComposerInterface *w);

    virtual bool processProcessKeyEvent(QKeyEvent *event);

Q_SIGNALS:
    void emitPluginActivated(MessageComposer::PluginEditorInterface *interface);
    void insertText(const QString &str);

private:
    PluginEditorInterfacePrivate *const d;
};
}

#endif // PLUGINEDITORINTERFACE_H
