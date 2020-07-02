/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGINEDITORCONVERTTEXTMANAGER_H
#define PLUGINEDITORCONVERTTEXTMANAGER_H

#include <QObject>
#include "messagecomposer_export.h"
#include <PimCommon/PluginUtil>
namespace MessageComposer {
class PluginEditorConvertTextManagerPrivate;
class PluginEditorConvertText;
/**
 * @brief The PluginEditorConvertTextManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConvertTextManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorConvertTextManager(QObject *parent = nullptr);
    ~PluginEditorConvertTextManager();

    static PluginEditorConvertTextManager *self();

    Q_REQUIRED_RESULT QVector<PluginEditorConvertText *> pluginsList() const;

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT PluginEditorConvertText *pluginFromIdentifier(const QString &id);
private:
    PluginEditorConvertTextManagerPrivate *const d;
};
}
#endif // PLUGINEDITORCONVERTTEXTMANAGER_H
