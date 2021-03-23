/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QDialog>
namespace MessageViewer
{
class RemoteContentConfigureWidget;
/**
 * @brief The RemoteContentConfigureDialog class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT RemoteContentConfigureDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RemoteContentConfigureDialog(QWidget *parent = nullptr);
    ~RemoteContentConfigureDialog() override;

private:
    void readConfig();
    void writeConfig();
    void slotAccept();
    RemoteContentConfigureWidget *const mRemoteContentConfigureWidget;
};
}

