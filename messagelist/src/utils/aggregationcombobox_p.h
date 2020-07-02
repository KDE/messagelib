/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#ifndef MESSAGELIST_UTILS_AGGREGATIONCOMBOBOX_P_H
#define MESSAGELIST_UTILS_AGGREGATIONCOMBOBOX_P_H

namespace MessageList {
namespace Core {
class Aggregation;
} // namespace Core

namespace Utils {
class AggregationComboBox;

class AggregationComboBoxPrivate
{
public:
    explicit AggregationComboBoxPrivate(AggregationComboBox *owner)
        : q(owner)
    {
    }

    AggregationComboBox *const q;

    /**
     * Refresh list of aggregations in the combobox.
     */
    void slotLoadAggregations();

    void setCurrentAggregation(const Core::Aggregation *aggregation);
};
} // namespace Utils
} // namespace MessageList

#endif //!__MESSAGELIST_UTILS_AGGREGATIONCOMBOBOX_P_H
