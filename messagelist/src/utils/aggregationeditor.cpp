/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "utils/aggregationeditor.h"
#include "core/aggregation.h"
#include "utils/comboboxutils.h"

#include <KTextEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <KLocalizedString>
#include <QComboBox>

using namespace MessageList::Utils;
using namespace MessageList::Core;

AggregationEditor::AggregationEditor(QWidget *parent)
    : OptionSetEditor(parent)
{
    // Grouping and Threading tab
    auto tab = new QWidget(this);
    addTab(tab, i18n("Groups && Threading"));

    auto tabg = new QGridLayout(tab);

    tabg->addWidget(new QLabel(i18nc("@label:textbox", "Grouping:"), tab), 0, 0);
    mGroupingCombo = new QComboBox(tab);
    tabg->addWidget(mGroupingCombo, 0, 1);

    connect(mGroupingCombo, &QComboBox::activated, this, &AggregationEditor::groupingComboActivated);

    tabg->addWidget(new QLabel(i18nc("@label:textbox", "Group expand policy:"), tab), 3, 0);
    mGroupExpandPolicyCombo = new QComboBox(tab);
    tabg->addWidget(mGroupExpandPolicyCombo, 3, 1);

    tabg->addWidget(new QLabel(i18nc("@label:textbox", "Threading:"), tab), 4, 0);
    mThreadingCombo = new QComboBox(tab);
    tabg->addWidget(mThreadingCombo, 4, 1);

    connect(mThreadingCombo, &QComboBox::activated, this, &AggregationEditor::threadingComboActivated);

    tabg->addWidget(new QLabel(i18nc("@label:textbox", "Thread leader:"), tab), 5, 0);
    mThreadLeaderCombo = new QComboBox(tab);
    tabg->addWidget(mThreadLeaderCombo, 5, 1);

    tabg->addWidget(new QLabel(i18nc("@label:textbox", "Thread expand policy:"), tab), 6, 0);
    mThreadExpandPolicyCombo = new QComboBox(tab);
    tabg->addWidget(mThreadExpandPolicyCombo, 6, 1);

    tabg->setColumnStretch(1, 1);
    tabg->setRowStretch(9, 1);

    // Advanced tab
    tab = new QWidget(this);
    addTab(tab, i18nc("@title:tab Advanced settings tab for aggregation mode", "Advanced"));

    tabg = new QGridLayout(tab);

    tabg->addWidget(new QLabel(i18nc("@label:textbox", "Fill view strategy:"), tab), 0, 0);
    mFillViewStrategyCombo = new QComboBox(tab);
    tabg->addWidget(mFillViewStrategyCombo, 0, 1);

    tabg->setColumnStretch(1, 1);
    tabg->setRowStretch(1, 1);
    fillGroupingCombo();
    fillThreadingCombo();
    fillFillViewStrategyCombo();

    fillThreadLeaderCombo();
    fillThreadExpandPolicyCombo();
    fillGroupExpandPolicyCombo();
}

AggregationEditor::~AggregationEditor() = default;

void AggregationEditor::editAggregation(Aggregation *set)
{
    mCurrentAggregation = set;

    if (!mCurrentAggregation) {
        setEnabled(false);
        return;
    }
    setEnabled(true);
    nameEdit()->setText(set->name());
    descriptionEdit()->setText(set->description());

    ComboBoxUtils::setIntegerOptionComboValue(mGroupingCombo, (int)mCurrentAggregation->grouping());
    ComboBoxUtils::setIntegerOptionComboValue(mThreadingCombo, (int)mCurrentAggregation->threading());
    ComboBoxUtils::setIntegerOptionComboValue(mFillViewStrategyCombo, (int)mCurrentAggregation->fillViewStrategy());

    // Necessary to fill after apply mGroupingCombo/mThreadingCombo/mFillViewStrategyCombo otherwise other combo are not filled.
    fillThreadLeaderCombo();
    fillThreadExpandPolicyCombo();
    fillGroupExpandPolicyCombo();

    ComboBoxUtils::setIntegerOptionComboValue(mThreadLeaderCombo, (int)mCurrentAggregation->threadLeader());

    ComboBoxUtils::setIntegerOptionComboValue(mThreadExpandPolicyCombo, (int)mCurrentAggregation->threadExpandPolicy());

    ComboBoxUtils::setIntegerOptionComboValue(mGroupExpandPolicyCombo, (int)mCurrentAggregation->groupExpandPolicy());
    fillThreadLeaderCombo();
    fillThreadExpandPolicyCombo();
    fillGroupExpandPolicyCombo();

    setReadOnly(mCurrentAggregation->readOnly());
}

void AggregationEditor::setReadOnly(bool readOnly)
{
    mGroupingCombo->setEnabled(!readOnly);
    mGroupExpandPolicyCombo->setEnabled(!readOnly);
    mThreadingCombo->setEnabled(!readOnly);
    mThreadLeaderCombo->setEnabled(!readOnly);
    mThreadExpandPolicyCombo->setEnabled(!readOnly);
    mFillViewStrategyCombo->setEnabled(!readOnly);

    OptionSetEditor::setReadOnly(readOnly);
}

void AggregationEditor::commit()
{
    mCurrentAggregation->setName(nameEdit()->text());
    mCurrentAggregation->setDescription(descriptionEdit()->toPlainText());

    mCurrentAggregation->setGrouping(static_cast<Aggregation::Grouping>(ComboBoxUtils::getIntegerOptionComboValue(mGroupingCombo, 0)));

    mCurrentAggregation->setGroupExpandPolicy(
        static_cast<Aggregation::GroupExpandPolicy>(ComboBoxUtils::getIntegerOptionComboValue(mGroupExpandPolicyCombo, 0)));

    mCurrentAggregation->setThreading(static_cast<Aggregation::Threading>(ComboBoxUtils::getIntegerOptionComboValue(mThreadingCombo, 0)));

    mCurrentAggregation->setThreadLeader(static_cast<Aggregation::ThreadLeader>(ComboBoxUtils::getIntegerOptionComboValue(mThreadLeaderCombo, 0)));

    mCurrentAggregation->setThreadExpandPolicy(
        static_cast<Aggregation::ThreadExpandPolicy>(ComboBoxUtils::getIntegerOptionComboValue(mThreadExpandPolicyCombo, 0)));

    mCurrentAggregation->setFillViewStrategy(static_cast<Aggregation::FillViewStrategy>(ComboBoxUtils::getIntegerOptionComboValue(mFillViewStrategyCombo, 0)));
}

void AggregationEditor::slotNameEditTextEdited(const QString &newName)
{
    if (!mCurrentAggregation) {
        return;
    }
    mCurrentAggregation->setName(newName);
    Q_EMIT aggregationNameChanged();
}

void AggregationEditor::fillGroupingCombo()
{
    ComboBoxUtils::fillIntegerOptionCombo(mGroupingCombo, Aggregation::enumerateGroupingOptions());
}

void AggregationEditor::groupingComboActivated(int)
{
    fillGroupExpandPolicyCombo();
    fillThreadLeaderCombo();
}

void AggregationEditor::fillGroupExpandPolicyCombo()
{
    ComboBoxUtils::fillIntegerOptionCombo(mGroupExpandPolicyCombo,
                                          Aggregation::enumerateGroupExpandPolicyOptions(
                                              (Aggregation::Grouping)ComboBoxUtils::getIntegerOptionComboValue(mGroupingCombo, Aggregation::NoGrouping)));
}

void AggregationEditor::fillThreadingCombo()
{
    ComboBoxUtils::fillIntegerOptionCombo(mThreadingCombo, Aggregation::enumerateThreadingOptions());
}

void AggregationEditor::threadingComboActivated(int)
{
    fillThreadLeaderCombo();
    fillThreadExpandPolicyCombo();
}

void AggregationEditor::fillThreadLeaderCombo()
{
    ComboBoxUtils::fillIntegerOptionCombo(mThreadLeaderCombo,
                                          Aggregation::enumerateThreadLeaderOptions(
                                              (Aggregation::Grouping)ComboBoxUtils::getIntegerOptionComboValue(mGroupingCombo, Aggregation::NoGrouping),
                                              (Aggregation::Threading)ComboBoxUtils::getIntegerOptionComboValue(mThreadingCombo, Aggregation::NoThreading)));
}

void AggregationEditor::fillThreadExpandPolicyCombo()
{
    ComboBoxUtils::fillIntegerOptionCombo(mThreadExpandPolicyCombo,
                                          Aggregation::enumerateThreadExpandPolicyOptions(
                                              (Aggregation::Threading)ComboBoxUtils::getIntegerOptionComboValue(mThreadingCombo, Aggregation::NoThreading)));
}

void AggregationEditor::fillFillViewStrategyCombo()
{
    ComboBoxUtils::fillIntegerOptionCombo(mFillViewStrategyCombo, Aggregation::enumerateFillViewStrategyOptions());
}

#include "moc_aggregationeditor.cpp"
