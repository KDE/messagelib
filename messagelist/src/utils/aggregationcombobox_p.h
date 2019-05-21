/* Copyright 2009 James Bendig <james@imptalk.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
