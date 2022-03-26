/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QDialog>

namespace MessageList
{
namespace Core
{
class Manager;
} // namespace Core

namespace Utils
{
/**
 * The dialog used for configuring MessageList::Aggregation sets.
 *
 * This is managed by MessageList::Manager. Take a look at it first
 * if you want to display this dialog.
 */
class ConfigureAggregationsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureAggregationsDialog(QWidget *parent = nullptr);
    ~ConfigureAggregationsDialog() override;

    void selectAggregation(const QString &aggregationId);

Q_SIGNALS:
    void okClicked();

private:
    class ConfigureAggregationsDialogPrivate;
    std::unique_ptr<ConfigureAggregationsDialogPrivate> const d;
};
} // namespace Utils
} // namespace MessageList
