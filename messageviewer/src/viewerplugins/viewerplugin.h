/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
#include <memory>
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

    [[nodiscard]] virtual MessageViewer::ViewerPluginInterface *createView(QWidget *parent, KActionCollection *ac) = 0;
    [[nodiscard]] virtual QString viewerPluginName() const = 0;
    virtual void showConfigureDialog(QWidget *parent = nullptr);
    [[nodiscard]] virtual bool hasConfigureDialog() const;

    void setIsEnabled(bool enabled);
    [[nodiscard]] bool isEnabled() const;

Q_SIGNALS:
    void configChanged();

private:
    std::unique_ptr<ViewerPluginPrivate> const d;
};
}
