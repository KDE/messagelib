/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMVIEWERMENU_H
#define DKIMVIEWERMENU_H

#include <QObject>
#include "messageviewer_export.h"
class QMenu;
class QAction;
namespace MessageViewer {
class MESSAGEVIEWER_EXPORT DKIMViewerMenu : public QObject
{
    Q_OBJECT
public:
    explicit DKIMViewerMenu(QObject *parent = nullptr);
    ~DKIMViewerMenu() override;
    QMenu *menu() const;

    void setEnableUpdateDkimKeyMenu(bool enabled);
Q_SIGNALS:
    void recheckSignature();
    void updateDkimKey();
    void showDkimRules();

private:
    void initialize();
    QMenu *mMenu = nullptr;
    QAction *mUpdateDKIMKey = nullptr;
};
}

#endif // DKIMVIEWERMENU_H
