/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERKEYDIALOG_H
#define DKIMMANAGERKEYDIALOG_H

#include <QDialog>
#include "messageviewer_export.h"

namespace MessageViewer {
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
    ~DKIMManagerKeyDialog();
private:
    void readConfig();
    void writeConfig();
    void loadKeys();
    void slotAccept();
    DKIMManagerKeyWidget *mManagerWidget = nullptr;
};
}

#endif // DKIMMANAGERKEYDIALOG_H
