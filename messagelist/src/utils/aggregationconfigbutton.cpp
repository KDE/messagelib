/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "utils/aggregationconfigbutton.h"

#include "core/manager.h"
#include "utils/aggregationcombobox.h"
#include "utils/configureaggregationsdialog.h"

#include <KLocalizedString>

using namespace MessageList::Core;
using namespace MessageList::Utils;

class MessageList::Utils::AggregationConfigButtonPrivate
{
public:
    AggregationConfigButtonPrivate(AggregationConfigButton *owner)
        : q(owner)
    {
    }

    AggregationConfigButton *const q;

    const AggregationComboBox *mAggregationComboBox = nullptr;

    void slotConfigureAggregations();
};

AggregationConfigButton::AggregationConfigButton(QWidget *parent, const AggregationComboBox *aggregationComboBox)
    : QPushButton(i18nc("@action:button", "Configure..."), parent)
    , d(new AggregationConfigButtonPrivate(this))
{
    d->mAggregationComboBox = aggregationComboBox;
    connect(this, &AggregationConfigButton::pressed, this, [this]() {
        d->slotConfigureAggregations();
    });

    // Keep aggregation combo up-to-date with any changes made in the configure dialog.
    if (d->mAggregationComboBox) {
        connect(this, &AggregationConfigButton::configureDialogCompleted, d->mAggregationComboBox, &AggregationComboBox::slotLoadAggregations);
    }
    setEnabled(Manager::instance());
}

AggregationConfigButton::~AggregationConfigButton() = default;

void AggregationConfigButtonPrivate::slotConfigureAggregations()
{
    QString currentAggregationID;
    if (mAggregationComboBox) {
        currentAggregationID = mAggregationComboBox->currentAggregation();
    }

    auto dialog = new ConfigureAggregationsDialog(q->window());
    dialog->selectAggregation(currentAggregationID);
    if (dialog->exec()) {
        Q_EMIT q->configureDialogCompleted();
    }
}

#include "moc_aggregationconfigbutton.cpp"
