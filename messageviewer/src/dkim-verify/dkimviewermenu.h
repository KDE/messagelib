/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
class QMenu;
class QAction;
namespace MessageViewer
{
/**
 * @brief The DKIMViewerMenu class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMViewerMenu : public QObject
{
    Q_OBJECT
public:
    explicit DKIMViewerMenu(QObject *parent = nullptr);
    ~DKIMViewerMenu() override;
    [[nodiscard]] QMenu *menu() const;

    void setEnableUpdateDkimKeyMenu(bool enabled);
Q_SIGNALS:
    void recheckSignature();
    void updateDkimKey();
    void showDkimRules();

private:
    MESSAGEVIEWER_NO_EXPORT void initialize();
    QMenu *const mMenu;
    QAction *mUpdateDKIMKey = nullptr;
};
}
