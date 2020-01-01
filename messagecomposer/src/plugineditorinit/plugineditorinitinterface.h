/*
   Copyright (C) 2017-2020 Laurent Montel <montel@kde.org>

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

#ifndef PLUGINEDITORINITINTERFACE_H
#define PLUGINEDITORINITINTERFACE_H

#include <QObject>
#include "messagecomposer_export.h"

namespace KPIMTextEdit {
class RichTextComposer;
}

namespace MessageComposer {
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
    ~PluginEditorInitInterface();

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

#endif // PLUGINEDITORINITINTERFACE_H
