/*
   Copyright (C) 2015-2016 Laurent Montel <montel@kde.org>

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

#ifndef PLUGINEDITORMANAGER_H
#define PLUGINEDITORMANAGER_H

#include <QObject>
#include "messagecomposer_export.h"
#include <PimCommonAkonadi/PluginUtil>
namespace MessageComposer
{
class PluginEditorManagerPrivate;
class PluginEditor;
class MESSAGECOMPOSER_EXPORT PluginEditorManager : public QObject
{
    Q_OBJECT
public:

    explicit PluginEditorManager(QObject *parent = nullptr);
    ~PluginEditorManager();

    static PluginEditorManager *self();

    QVector<PluginEditor *> pluginsList() const;

    QVector<PimCommon::PluginUtilData> pluginsDataList() const;

    QString configGroupName() const;
    QString configPrefixSettingKey() const;
    PluginEditor *pluginFromIdentifier(const QString &id);
private:
    PluginEditorManagerPrivate *const d;
};
}
#endif // PLUGINEDITORMANAGER_H
