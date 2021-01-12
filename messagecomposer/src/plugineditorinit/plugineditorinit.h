/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGINEDITORINIT_H
#define PLUGINEDITORINIT_H

#include <QObject>
#include "messagecomposer_export.h"

namespace MessageComposer {
class PluginEditorInitPrivate;
class PluginEditorInitInterface;
/**
 * @brief The PluginEditorInit class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorInit : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorInit(QObject *parent = nullptr);
    ~PluginEditorInit() override;

    virtual PluginEditorInitInterface *createInterface(QObject *parent) = 0;

    Q_REQUIRED_RESULT virtual bool hasConfigureDialog() const;

    virtual void showConfigureDialog(QWidget *parent = nullptr);

    void emitConfigChanged();

    Q_REQUIRED_RESULT virtual QString description() const;

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;

Q_SIGNALS:
    void configChanged();

private:
    PluginEditorInitPrivate *const d;
};
}
#endif // PLUGINEDITORINIT_H
