/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGINEDITORINITMANAGER_H
#define PLUGINEDITORINITMANAGER_H

#include <QObject>
#include "messagecomposer_export.h"
#include <PimCommon/PluginUtil>
namespace MessageComposer {
class PluginEditorInitManagerPrivate;
class PluginEditorInit;
/**
 * @brief The PluginEditorInitManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorInitManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorInitManager(QObject *parent = nullptr);
    ~PluginEditorInitManager();

    static PluginEditorInitManager *self();

    Q_REQUIRED_RESULT QVector<PluginEditorInit *> pluginsList() const;

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT PluginEditorInit *pluginFromIdentifier(const QString &id);
private:
    PluginEditorInitManagerPrivate *const d;
};
}
#endif // PLUGINEDITORINITMANAGER_H
