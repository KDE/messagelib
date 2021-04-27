/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include "messageviewer_export.h"
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
    virtual Q_REQUIRED_RESULT QString name() const = 0;
    virtual Q_REQUIRED_RESULT bool hasMargin() const;
    virtual Q_REQUIRED_RESULT QString alignment() const;
    virtual Q_REQUIRED_RESULT int elidedTextSize() const;

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;

    virtual Q_REQUIRED_RESULT QString attachmentHtml() const;

    virtual Q_REQUIRED_RESULT bool hasConfigureDialog() const;

    virtual void showConfigureDialog(QWidget *parent);

    virtual Q_REQUIRED_RESULT QString extraScreenCss(const QString &headerFont) const;
    virtual Q_REQUIRED_RESULT QString extraPrintCss(const QString &headerFont) const;
    virtual Q_REQUIRED_RESULT QString extraCommonCss(const QString &headerFont) const;

private:
    HeaderStylePluginPrivate *const d;
};
}
