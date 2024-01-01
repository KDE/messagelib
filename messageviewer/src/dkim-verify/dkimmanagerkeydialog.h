/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QDialog>

namespace MessageViewer
{
class DKIMManagerKeyWidget;
/**
 * @brief The DKIMManagerKeyDialog class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManagerKeyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyDialog(QWidget *parent = nullptr);
    ~DKIMManagerKeyDialog() override;

private:
    MESSAGEVIEWER_NO_EXPORT void readConfig();
    MESSAGEVIEWER_NO_EXPORT void writeConfig();
    MESSAGEVIEWER_NO_EXPORT void loadKeys();
    MESSAGEVIEWER_NO_EXPORT void slotAccept();
    DKIMManagerKeyWidget *const mManagerWidget;
};
}
