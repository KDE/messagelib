/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
class KActionCollection;

namespace MessageViewer
{
class ViewerPluginPrivate;
class ViewerPluginInterface;
/**
 * @brief The ViewerPlugin class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ViewerPlugin : public QObject
{
    Q_OBJECT
public:
    explicit ViewerPlugin(QObject *parent = nullptr);
    ~ViewerPlugin() override;

    Q_REQUIRED_RESULT virtual MessageViewer::ViewerPluginInterface *createView(QWidget *parent, KActionCollection *ac) = 0;
    Q_REQUIRED_RESULT virtual QString viewerPluginName() const = 0;
    virtual void showConfigureDialog(QWidget *parent = nullptr);
    Q_REQUIRED_RESULT virtual bool hasConfigureDialog() const;

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;

Q_SIGNALS:
    void configChanged();

private:
    ViewerPluginPrivate *const d;
};
}
