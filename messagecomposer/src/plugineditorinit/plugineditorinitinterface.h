/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
#include <memory>
namespace KPIMTextEdit
{
class RichTextComposer;
}

namespace MessageComposer
{
class PluginEditorInitInterfacePrivate;
/*!
 \class MessageComposer::PluginEditorInitInterface
 \inmodule MessageComposer
 \inheaderfile MessageComposer/PluginEditorInitInterface
 \author Laurent Montel <montel@kde.org>

 The PluginEditorInitInterface class.
 */
class MESSAGECOMPOSER_EXPORT PluginEditorInitInterface : public QObject
{
    Q_OBJECT
public:
    /*! \brief Constructs a PluginEditorInitInterface.
        \param parent The parent object.
    */
    explicit PluginEditorInitInterface(QObject *parent = nullptr);
    /*! \brief Destroys the PluginEditorInitInterface. */
    ~PluginEditorInitInterface() override;

    /*! \brief Executes the editor initialization interface.
        \return True if successful, false otherwise.
    */
    [[nodiscard]] virtual bool exec() = 0;

    /*! \brief Sets the parent widget for dialogs or UI elements.
        \param parent The parent widget.
    */
    void setParentWidget(QWidget *parent);
    /*! \brief Returns the parent widget. */
    [[nodiscard]] QWidget *parentWidget() const;

    /*! \brief Returns the rich text editor widget. */
    [[nodiscard]] KPIMTextEdit::RichTextComposer *richTextEditor() const;
    /*! \brief Sets the rich text editor widget.
        \param richTextEditor The editor to use.
    */
    void setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor);

public Q_SLOTS:
    /*! \brief Reloads the configuration for the editor initialization. */
    virtual void reloadConfig();

private:
    std::unique_ptr<PluginEditorInitInterfacePrivate> const d;
};
}
