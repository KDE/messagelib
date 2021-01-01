/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGINEDITORCHECKBEFORESEND_H
#define PLUGINEDITORCHECKBEFORESEND_H

#include <QObject>
#include "messagecomposer_export.h"

namespace MessageComposer {
class PluginEditorCheckBeforeSendPrivate;
class PluginEditorCheckBeforeSendInterface;
/**
 * @brief The PluginEditorCheckBeforeSend class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorCheckBeforeSend : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorCheckBeforeSend(QObject *parent = nullptr);
    ~PluginEditorCheckBeforeSend();

    virtual PluginEditorCheckBeforeSendInterface *createInterface(QObject *parent) = 0;

    Q_REQUIRED_RESULT virtual bool hasConfigureDialog() const;

    virtual void showConfigureDialog(QWidget *parent = nullptr);

    void emitConfigChanged();

    Q_REQUIRED_RESULT virtual QString description() const;

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;

Q_SIGNALS:
    void configChanged();

private:
    PluginEditorCheckBeforeSendPrivate *const d;
};
}
#endif // PLUGINEDITORCHECKBEFORESEND_H
