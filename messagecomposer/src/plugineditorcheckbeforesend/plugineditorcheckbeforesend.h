/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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

#ifndef PLUGINEDITORCHECKBEFORESEND_H
#define PLUGINEDITORCHECKBEFORESEND_H

#include <QObject>
#include "messagecomposer_export.h"

namespace MessageComposer {
class PluginEditorCheckBeforeSendPrivate;
class PluginEditorCheckBeforeSendInterface;

class MESSAGECOMPOSER_EXPORT PluginEditorCheckBeforeSend : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorCheckBeforeSend(QObject *parent = nullptr);
    ~PluginEditorCheckBeforeSend();

    virtual PluginEditorCheckBeforeSendInterface *createInterface(QObject *parent) = 0;

    virtual bool hasConfigureDialog() const;

    virtual void showConfigureDialog(QWidget *parent = nullptr);

    void emitConfigChanged();

    virtual QString description() const;

    void setIsEnabled(bool enabled);
    bool isEnabled() const;

Q_SIGNALS:
    void configChanged();

private:
    PluginEditorCheckBeforeSendPrivate *const d;
};
}
#endif // PLUGINEDITORCHECKBEFORESEND_H
