/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "utils/comboboxutils.h"

#include <QVariant>

#include <QComboBox>

using namespace MessageList::Utils;

void ComboBoxUtils::fillIntegerOptionCombo(QComboBox *combo, const QList<QPair<QString, int>> &optionDescriptors)
{
    int val = getIntegerOptionComboValue(combo, -1);
    combo->clear();
    int valIdx = -1;
    int idx = 0;

    QList<QPair<QString, int>>::ConstIterator end(optionDescriptors.end());

    for (QList<QPair<QString, int>>::ConstIterator it = optionDescriptors.constBegin(); it != end; ++it) {
        if (val == (*it).second) {
            valIdx = idx;
        }
        combo->addItem((*it).first, QVariant((*it).second));
        ++idx;
    }
    if (idx == 0) {
        combo->addItem(QStringLiteral("-"), QVariant((int)0)); // always default to 0
        combo->setEnabled(false);
    } else {
        if (!combo->isEnabled()) {
            combo->setEnabled(true);
        }
        if (valIdx >= 0) {
            combo->setCurrentIndex(valIdx);
        }
        if (combo->count() == 1) {
            combo->setEnabled(false); // disable when there is no choice
        }
    }
}

void ComboBoxUtils::setIntegerOptionComboValue(QComboBox *combo, int value)
{
    if (combo->itemData(combo->currentIndex()).toInt() == value) {
        return;
    }
    int index = combo->findData(value);
    if (index != -1) {
        combo->setCurrentIndex(index);
    } else {
        combo->setCurrentIndex(0); // default
    }
}

int ComboBoxUtils::getIntegerOptionComboValue(QComboBox *combo, int defaultValue)
{
    const int idx = combo->currentIndex();
    if (idx < 0) {
        return defaultValue;
    }

    QVariant data = combo->itemData(idx);
    bool ok;
    const int val = data.toInt(&ok);
    if (!ok) {
        return defaultValue;
    }
    return val;
}
