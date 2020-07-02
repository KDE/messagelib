/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef HEADERSTYLEMENU_H
#define HEADERSTYLEMENU_H

#include "messageviewer_export.h"

#include <QObject>
class KActionMenu;
class KActionCollection;
namespace MessageViewer {
class HeaderStylePlugin;
class HeaderStyleMenuManagerPrivate;
/**
 * @brief The HeaderStyleMenuManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT HeaderStyleMenuManager : public QObject
{
    Q_OBJECT
public:
    explicit HeaderStyleMenuManager(KActionCollection *ac, QObject *parent = nullptr);
    ~HeaderStyleMenuManager();

    Q_REQUIRED_RESULT KActionMenu *menu() const;

    void setPluginName(const QString &pluginName);
    void readConfig();

public Q_SLOTS:
    void slotStyleChanged(MessageViewer::HeaderStylePlugin *plugin);
Q_SIGNALS:
    void styleChanged(MessageViewer::HeaderStylePlugin *plugin);
    void styleUpdated();

private:
    HeaderStyleMenuManagerPrivate *const d;
};
}
#endif // HEADERSTYLEMENU_H
