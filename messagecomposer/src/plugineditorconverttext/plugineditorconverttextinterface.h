/*
   Copyright (C) 2018 Laurent Montel <montel@kde.org>

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

#ifndef PLUGINEDITORCONVERTTEXTINTERFACE_H
#define PLUGINEDITORCONVERTTEXTINTERFACE_H

#include <QObject>
#include "messagecomposer_export.h"

namespace KPIMTextEdit {
class RichTextComposer;
}

namespace MessageComposer {
class PluginEditorConvertTextInterfacePrivate;
class MESSAGECOMPOSER_EXPORT PluginEditorConvertTextInterface : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorConvertTextInterface(QObject *parent = nullptr);
    ~PluginEditorConvertTextInterface();

    virtual bool exec() = 0;

    void setParentWidget(QWidget *parent);
    QWidget *parentWidget() const;

    KPIMTextEdit::RichTextComposer *richTextEditor() const;
    void setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor);

public Q_SLOTS:
    virtual void reloadConfig();

private:
    PluginEditorConvertTextInterfacePrivate *const d;
};
}

#endif // PLUGINEDITORCONVERTTEXTINTERFACE_H
