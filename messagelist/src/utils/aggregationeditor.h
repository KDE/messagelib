/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "utils/optionseteditor.h"

class QComboBox;

namespace MessageList
{
namespace Core
{
class Aggregation;
} // namespace Core

namespace Utils
{
/**
 * A widget that allows editing a single MessageList::Aggregation.
 *
 * Used by ConfigureAggregationsDialog.
 */
class AggregationEditor : public OptionSetEditor
{
    Q_OBJECT
public:
    explicit AggregationEditor(QWidget *parent);
    ~AggregationEditor() override;

private:
    Core::Aggregation *mCurrentAggregation = nullptr; // shallow, may be null!

    // Grouping, Threading and Sorting tab
    QComboBox *mGroupingCombo = nullptr;
    QComboBox *mGroupExpandPolicyCombo = nullptr;
    QComboBox *mThreadingCombo = nullptr;
    QComboBox *mThreadLeaderCombo = nullptr;
    QComboBox *mThreadExpandPolicyCombo = nullptr;
    // Advanced tab
    QComboBox *mFillViewStrategyCombo = nullptr;

public:
    /**
     * Sets the Aggregation to be edited.
     * Saves and forgets any previously Aggregation that was being edited.
     * The set parameter may be 0: in this case the editor is simply disabled.
     */
    void editAggregation(Core::Aggregation *set);

    /**
     * Returns the Aggregation currently edited by this AggregationEditor.
     * May be 0.
     */
    Core::Aggregation *editedAggregation() const
    {
        return mCurrentAggregation;
    }

    /**
     * Explicitly commits the changes in the editor to the edited Aggregation, if any.
     */
    void commit();

Q_SIGNALS:
    /**
     * This is triggered when the aggregation name changes in the editor text field.
     * It's connected to the Aggregation configuration dialog which updates
     * the list of aggregations with the new name.
     */
    void aggregationNameChanged();

private:
    // Helpers for filling the various editing elements

    void fillGroupingCombo();
    void fillGroupExpandPolicyCombo();
    void fillThreadingCombo();
    void fillThreadLeaderCombo();
    void fillThreadExpandPolicyCombo();
    void fillFillViewStrategyCombo();
    void setReadOnly(bool readOnly);
    // Internal handlers for editing element interaction

    void groupingComboActivated(int idx);
    void threadingComboActivated(int idx);
    void slotNameEditTextEdited(const QString &newName) override;
};
} // namespace Utils
} // namespace MessageList
