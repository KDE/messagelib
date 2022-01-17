/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include "messageviewer_export.h"
#include <memory>
class KActionCollection;
class QActionGroup;
class KActionMenu;
namespace MessageViewer
{
class HeaderStyle;
class HeaderStrategy;
class HeaderStylePluginPrivate;
class HeaderStyleInterface;
/**
 * @brief The HeaderStylePlugin class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT HeaderStylePlugin : public QObject
{
    Q_OBJECT
public:
    explicit HeaderStylePlugin(QObject *parent = nullptr);
    ~HeaderStylePlugin() override;

    virtual HeaderStyle *headerStyle() const = 0;
    virtual HeaderStrategy *headerStrategy() const = 0;
    virtual HeaderStyleInterface *createView(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac, QObject *parent = nullptr) = 0;
    Q_REQUIRED_RESULT virtual QString name() const = 0;
    Q_REQUIRED_RESULT virtual bool hasMargin() const;
    Q_REQUIRED_RESULT virtual QString alignment() const;
    Q_REQUIRED_RESULT virtual int elidedTextSize() const;

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;

    Q_REQUIRED_RESULT virtual QString attachmentHtml() const;

    Q_REQUIRED_RESULT virtual bool hasConfigureDialog() const;

    virtual void showConfigureDialog(QWidget *parent);

    Q_REQUIRED_RESULT virtual QString extraScreenCss(const QString &headerFont) const;
    Q_REQUIRED_RESULT virtual QString extraPrintCss(const QString &headerFont) const;
    Q_REQUIRED_RESULT virtual QString extraCommonCss(const QString &headerFont) const;

private:
    std::unique_ptr<HeaderStylePluginPrivate> const d;
};
}
