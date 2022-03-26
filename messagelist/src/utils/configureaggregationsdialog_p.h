/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "utils/configureaggregationsdialog.h"
#include <QListWidgetItem>
namespace MessageList
{
namespace Core
{
class Aggregation;
} // namespace Core

namespace Utils
{
class AggregationEditor;
class AggregationListWidget;
class AggregationListWidgetItem;

/**
 * The dialog used for configuring MessageList::Aggregation sets.
 *
 * This is managed by MessageList::Manager. Take a look at it first
 * if you want to display this dialog.
 */
class Q_DECL_HIDDEN ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate
{
public:
    ConfigureAggregationsDialogPrivate(ConfigureAggregationsDialog *owner)
        : q(owner)
    {
    }

    // Private implementation

    void fillAggregationList();
    QString uniqueNameForAggregation(const QString &baseName, Core::Aggregation *skipAggregation = nullptr);
    AggregationListWidgetItem *findAggregationItemByName(const QString &name, Core::Aggregation *skipAggregation = nullptr);
    AggregationListWidgetItem *findAggregationItemByAggregation(Core::Aggregation *set);
    AggregationListWidgetItem *findAggregationItemById(const QString &aggregationId);
    void commitEditor();

    // Private slots

    void aggregationListItemClicked(QListWidgetItem *cur);
    void newAggregationButtonClicked();
    void cloneAggregationButtonClicked();
    void deleteAggregationButtonClicked();
    void editedAggregationNameChanged();
    void okButtonClicked();
    void exportAggregationButtonClicked();
    void importAggregationButtonClicked();
    void updateButton(QListWidgetItem *cur);

    ConfigureAggregationsDialog *const q;

    AggregationListWidget *mAggregationList = nullptr;
    AggregationEditor *mEditor = nullptr;
    QPushButton *mNewAggregationButton = nullptr;
    QPushButton *mCloneAggregationButton = nullptr;
    QPushButton *mDeleteAggregationButton = nullptr;
    QPushButton *mExportAggregationButton = nullptr;
    QPushButton *mImportAggregationButton = nullptr;
};
} // namespace Utils
} // namespace MessageList
