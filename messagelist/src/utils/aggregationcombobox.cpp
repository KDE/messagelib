/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "aggregationcombobox.h"
#include "aggregationcombobox_p.h"

#include "core/aggregation.h"
#include "core/manager.h"
#include "messagelistsettings.h"
#include "storagemodel.h"

using namespace MessageList::Core;
using namespace MessageList::Utils;

AggregationComboBox::AggregationComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(new AggregationComboBoxPrivate(this))
{
    if (Manager::instance()) {
        d->slotLoadAggregations();
    } else {
        setEnabled(false);
    }
}

AggregationComboBox::~AggregationComboBox() = default;

QString AggregationComboBox::currentAggregation() const
{
    return itemData(currentIndex()).toString();
}

void AggregationComboBox::writeDefaultConfig() const
{
    KConfigGroup group(MessageListSettings::self()->config(), QStringLiteral("MessageListView::StorageModelAggregations"));

    const QString aggregationID = currentAggregation();
    group.writeEntry(QStringLiteral("DefaultSet"), aggregationID);

    if (Manager::instance()) {
        Manager::instance()->aggregationsConfigurationCompleted();
    }
}

void AggregationComboBox::writeStorageModelConfig(MessageList::Core::StorageModel *storageModel, bool isPrivateSetting) const
{
    writeStorageModelConfig(storageModel->id(), isPrivateSetting);
}

void AggregationComboBox::writeStorageModelConfig(const QString &id, bool isPrivateSetting) const
{
    if (Manager::instance()) {
        // message list aggregation
        QString aggregationID;
        if (isPrivateSetting) {
            aggregationID = currentAggregation();
        } else { // explicitly use default aggregation id when using default aggregation.
            aggregationID = Manager::instance()->defaultAggregation()->id();
        }
        Manager::instance()->saveAggregationForStorageModel(id, aggregationID, isPrivateSetting);
        Manager::instance()->aggregationsConfigurationCompleted();
    }
}

void AggregationComboBox::readStorageModelConfig(const QString &id, bool &isPrivateSetting)
{
    if (Manager::instance()) {
        const Aggregation *aggregation = Manager::instance()->aggregationForStorageModel(id, &isPrivateSetting);
        d->setCurrentAggregation(aggregation);
    }
}

void AggregationComboBox::readStorageModelConfig(MessageList::Core::StorageModel *storageModel, bool &isPrivateSetting)
{
    readStorageModelConfig(storageModel->id(), isPrivateSetting);
}

void AggregationComboBox::readStorageModelConfig(const Akonadi::Collection &col, bool &isPrivateSetting)
{
    if (col.isValid()) {
        readStorageModelConfig(QString::number(col.id()), isPrivateSetting);
    }
}

void AggregationComboBox::selectDefault()
{
    if (Manager::instance()) {
        const Aggregation *defaultAggregation = Manager::instance()->defaultAggregation();
        d->setCurrentAggregation(defaultAggregation);
    }
}

void AggregationComboBox::slotLoadAggregations()
{
    d->slotLoadAggregations();
}

void AggregationComboBoxPrivate::slotLoadAggregations()
{
    if (!Manager::instance()) {
        return;
    }
    q->clear();

    // Get all message list aggregations and sort them into alphabetical order.
    QList<Aggregation *> aggregations = Manager::instance()->aggregations().values();
    std::sort(aggregations.begin(), aggregations.end(), MessageList::Core::Aggregation::compareName);

    for (const Aggregation *aggregation : std::as_const(aggregations)) {
        q->addItem(aggregation->name(), QVariant(aggregation->id()));
    }
}

void AggregationComboBoxPrivate::setCurrentAggregation(const Aggregation *aggregation)
{
    Q_ASSERT(aggregation != nullptr);

    const QString aggregationID = aggregation->id();
    const int aggregationIndex = q->findData(QVariant(aggregationID));
    q->setCurrentIndex(aggregationIndex);
}

#include "moc_aggregationcombobox.cpp"
