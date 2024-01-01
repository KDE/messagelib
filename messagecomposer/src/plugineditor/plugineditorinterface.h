/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <MessageComposer/PluginActionType>
#include <PimCommon/AbstractGenericPluginInterface>
#include <QObject>

class QKeyEvent;
namespace TextCustomEditor
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
    enum ApplyOnFieldType {
        Composer = 1,
        EmailFields = 2,
        SubjectField = 4,
        All = Composer | EmailFields | SubjectField,
    };
    Q_FLAG(ApplyOnFieldType)
    Q_DECLARE_FLAGS(ApplyOnFieldTypes, ApplyOnFieldType)

    explicit PluginEditorInterface(QObject *parent = nullptr);
    ~PluginEditorInterface() override;

    void setActionType(PluginActionType type);
    [[nodiscard]] PluginActionType actionType() const;

    [[nodiscard]] TextCustomEditor::RichTextEditor *richTextEditor() const;
    void setRichTextEditor(TextCustomEditor::RichTextEditor *richTextEditor);

    void setNeedSelectedText(bool b);
    [[nodiscard]] bool needSelectedText() const;

    void setStatusBarWidget(QWidget *w);
    [[nodiscard]] QWidget *statusBarWidget() const;

    [[nodiscard]] MessageComposer::PluginComposerInterface *composerInterface() const;
    void setComposerInterface(MessageComposer::PluginComposerInterface *w);

    virtual bool processProcessKeyEvent(QKeyEvent *event);

    [[nodiscard]] PluginEditorInterface::ApplyOnFieldTypes applyOnFieldTypes() const;

    void setApplyOnFieldTypes(PluginEditorInterface::ApplyOnFieldTypes types);
Q_SIGNALS:
    void emitPluginActivated(MessageComposer::PluginEditorInterface *interface);
    void insertText(const QString &str);

    void errorMessage(const QString &message);
    void successMessage(const QString &message);

private:
    std::unique_ptr<PluginEditorInterfacePrivate> const d;
};
}
