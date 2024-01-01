/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
class KToggleAction;
class KActionCollection;
class QAction;
class QActionGroup;
class KActionMenu;
namespace MessageViewer
{
class HeaderStyle;
class HeaderStylePlugin;
/**
 * @brief The HeaderStyleInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT HeaderStyleInterface : public QObject
{
    Q_OBJECT
public:
    explicit HeaderStyleInterface(MessageViewer::HeaderStylePlugin *, QObject *parent = nullptr);
    ~HeaderStyleInterface() override;
    [[nodiscard]] QList<KToggleAction *> action() const;
    virtual void createAction(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac) = 0;
    virtual void activateAction() = 0;
    [[nodiscard]] HeaderStylePlugin *headerStylePlugin() const;

Q_SIGNALS:
    void styleChanged(MessageViewer::HeaderStylePlugin *plugin);
    void styleUpdated();

protected Q_SLOTS:
    void slotStyleChanged();

protected:
    void addHelpTextAction(QAction *act, const QString &text);
    void addActionToMenu(KActionMenu *menu, QActionGroup *actionGroup);
    QList<KToggleAction *> mAction;
    HeaderStylePlugin *const mHeaderStylePlugin;
};
}
