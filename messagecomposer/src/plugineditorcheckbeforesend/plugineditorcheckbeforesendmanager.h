/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGINEDITORCHECKBEFORESENDMANAGER_H
#define PLUGINEDITORCHECKBEFORESENDMANAGER_H

#include <QObject>
#include "messagecomposer_export.h"
#include <PimCommon/PluginUtil>
namespace MessageComposer {
class PluginEditorCheckBeforeSendManagerPrivate;
class PluginEditorCheckBeforeSend;
/**
 * @brief The PluginEditorCheckBeforeSendManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorCheckBeforeSendManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorCheckBeforeSendManager(QObject *parent = nullptr);
    ~PluginEditorCheckBeforeSendManager();

    static PluginEditorCheckBeforeSendManager *self();

    Q_REQUIRED_RESULT QVector<PluginEditorCheckBeforeSend *> pluginsList() const;

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT PluginEditorCheckBeforeSend *pluginFromIdentifier(const QString &id);
private:
    PluginEditorCheckBeforeSendManagerPrivate *const d;
};
}
#endif // PLUGINEDITORCHECKBEFORESENDMANAGER_H
