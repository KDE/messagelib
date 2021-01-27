/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGINEDITORINTERFACE_H
#define PLUGINEDITORINTERFACE_H

#include "messagecomposer_export.h"
#include <MessageComposer/PluginActionType>
#include <PimCommon/AbstractGenericPluginInterface>
#include <QObject>

class QKeyEvent;
namespace KPIMTextEdit
{
class RichTextEditor;
}

namespace MessageComposer
{
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
    ~PluginEditorInterface() override;

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
