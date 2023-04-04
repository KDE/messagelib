/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QPair>

class QComboBox;

namespace MessageList
{
namespace Utils
{
/**
 * Namespace containing some helper functions for QComboBox widgets.
 */
namespace ComboBoxUtils
{
/**
 * Fills the specified QComboBox with the options available in optionDescriptors.
 * Each option descriptor contains a description string and a distinct integer (possibly enum)
 * identifier value.
 */
void fillIntegerOptionCombo(QComboBox *combo, const QList<QPair<QString, int>> &optionDescriptors);

/**
 * Returns the identifier of the currently selected option in the specified combo.
 * If the combo has no current selection or something goes wrong then the defaultValue
 * is returned instead.
 */
int getIntegerOptionComboValue(QComboBox *combo, int defaultValue);

/**
 * Sets the currently selected option in the specified combo.
 */
void setIntegerOptionComboValue(QComboBox *combo, int value);
} // namespace ComboBoxUtils
} // namespace Utils
} // namespace MessageList
