/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#ifndef MESSAGELIST_UTILS_AGGREGATIONCOMBOBOX_H
#define MESSAGELIST_UTILS_AGGREGATIONCOMBOBOX_H

#include <messagelist_export.h>
#include <QComboBox>
#include <collection.h>
namespace MessageList {
namespace Core {
class Aggregation;
class StorageModel;
}

namespace Utils {
class AggregationComboBoxPrivate;
/**
 * A specialized QComboBox that lists all message list aggregations.
 */
class MESSAGELIST_EXPORT AggregationComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit AggregationComboBox(QWidget *parent);
    ~AggregationComboBox() override;

    QString currentAggregation() const;

    void writeDefaultConfig() const;

    void writeStorageModelConfig(MessageList::Core::StorageModel *storageModel, bool isPrivateSetting) const;
    void writeStorageModelConfig(const QString &id, bool isPrivateSetting) const;

    void readStorageModelConfig(MessageList::Core::StorageModel *storageModel, bool &isPrivateSetting);
    void readStorageModelConfig(const Akonadi::Collection &col, bool &isPrivateSetting);
    void readStorageModelConfig(const QString &id, bool &isPrivateSetting);

public Q_SLOTS:
    void selectDefault();
    void slotLoadAggregations();

private:
    AggregationComboBoxPrivate *const d;
};
} // namespace Utils
} // namespace MessageList

#endif //!__MESSAGELIST_UTILS_AGGREGATIONCOMBOBOX_H
