/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>

namespace MessageComposer
{
class PluginEditorBasePrivate;
class PluginEditorBaseInterface;
/**
 * @brief The PluginEditorBase class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorBase : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorBase(QObject *parent = nullptr);
    ~PluginEditorBase() override;

    Q_REQUIRED_RESULT virtual bool hasConfigureDialog() const;

    virtual void showConfigureDialog(QWidget *parent = nullptr);

    void emitConfigChanged();

    Q_REQUIRED_RESULT virtual QString description() const;

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;

Q_SIGNALS:
    void configChanged();

private:
    PluginEditorBasePrivate *const d;
};
}
