/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#ifndef MESSAGELIST_UTILS_CONFIGUREAGGREGATIONSDIALOG_H
#define MESSAGELIST_UTILS_CONFIGUREAGGREGATIONSDIALOG_H

#include <QDialog>

namespace MessageList {
namespace Core {
class Manager;
} // namespace Core

namespace Utils {
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
    ~ConfigureAggregationsDialog();

    void selectAggregation(const QString &aggregationId);

Q_SIGNALS:
    void okClicked();

private:
    class Private;
    Private *const d;
};
} // namespace Utils
} // namespace MessageList

#endif //!__MESSAGELIST_UTILS_CONFIGUREAGGREGATIONSDIALOG_H
