/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "utils/aggregationconfigbutton.h"

#include "core/manager.h"
#include "utils/aggregationcombobox.h"
#include "utils/aggregationcombobox_p.h"
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
    : QPushButton(i18n("Configure..."), parent)
    , d(new AggregationConfigButtonPrivate(this))
{
    d->mAggregationComboBox = aggregationComboBox;
    connect(this, &AggregationConfigButton::pressed, this, [this]() {
        d->slotConfigureAggregations();
    });

    // Keep aggregation combo up-to-date with any changes made in the configure dialog.
    if (d->mAggregationComboBox != nullptr) {
        connect(this, &AggregationConfigButton::configureDialogCompleted, d->mAggregationComboBox, &AggregationComboBox::slotLoadAggregations);
    }
    setEnabled(Manager::instance());
}

AggregationConfigButton::~AggregationConfigButton()
{
    delete d;
}

void AggregationConfigButtonPrivate::slotConfigureAggregations()
{
    QString currentAggregationID;
    if (mAggregationComboBox) {
        currentAggregationID = mAggregationComboBox->currentAggregation();
    }

    auto dialog = new ConfigureAggregationsDialog(q->window());
    dialog->selectAggregation(currentAggregationID);

    QObject::connect(dialog, &ConfigureAggregationsDialog::okClicked, q, &AggregationConfigButton::configureDialogCompleted);

    dialog->show();
}

#include "moc_aggregationconfigbutton.cpp"
