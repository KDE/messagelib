/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>

namespace KPIMTextEdit
{
class RichTextComposer;
}

namespace MessageComposer
{
class PluginEditorInitInterfacePrivate;
/**
 * @brief The PluginEditorInitInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorInitInterface : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorInitInterface(QObject *parent = nullptr);
    ~PluginEditorInitInterface() override;

    virtual bool exec() = 0;

    void setParentWidget(QWidget *parent);
    Q_REQUIRED_RESULT QWidget *parentWidget() const;

    Q_REQUIRED_RESULT KPIMTextEdit::RichTextComposer *richTextEditor() const;
    void setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor);

public Q_SLOTS:
    virtual void reloadConfig();

private:
    PluginEditorInitInterfacePrivate *const d;
};
}

