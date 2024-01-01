/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QWidget>

namespace MessageViewer
{
class PrintingSettingsPrivate;
/**
 * @brief The PrintingSettings class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT PrintingSettings : public QWidget
{
    Q_OBJECT
public:
    explicit PrintingSettings(QWidget *parent = nullptr);
    ~PrintingSettings() override;

    void save();
    void doLoadFromGlobalSettings();
    void doResetToDefaultsOther();

Q_SIGNALS:
    void changed();

private:
    std::unique_ptr<PrintingSettingsPrivate> const d;
};
}
