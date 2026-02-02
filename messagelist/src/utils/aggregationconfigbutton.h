/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

#include "messagelist_export.h"
#include <QPushButton>

namespace MessageList
{
namespace Utils
{
class AggregationComboBox;
class AggregationConfigButtonPrivate;

/*!
 * \class MessageList::Utils::AggregationConfigButton
 * \inmodule MessageList
 * \inheaderfile MessageList/AggregationConfigButton
 *
 * A specialized QPushButton that displays the aggregation
 * configure dialog when pressed.
 */
class MESSAGELIST_EXPORT AggregationConfigButton : public QPushButton
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor.
     * \param parent The parent widget for the button.
     * \param aggregationComboBox Optional AggregationComboBox to be kept in sync
     * with changes made by the configure dialog.
     */
    explicit AggregationConfigButton(QWidget *parent, const AggregationComboBox *aggregationComboBox = nullptr);
    /*!
     * \brief Destructor.
     */
    ~AggregationConfigButton() override;

Q_SIGNALS:
    /*!
     * \brief A signal emitted when configure dialog has been successfully completed.
     */
    void configureDialogCompleted();

private:
    std::unique_ptr<AggregationConfigButtonPrivate> const d;
};
} // namespace Utils
} // namespace MessageList
