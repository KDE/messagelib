/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "adblockautomaticruleslistwidget.h"

using namespace MessageViewer;

AdBlockAutomaticRulesListWidget::AdBlockAutomaticRulesListWidget(QWidget *parent)
    : QListWidget(parent)
{

}

AdBlockAutomaticRulesListWidget::~AdBlockAutomaticRulesListWidget()
{

}

void AdBlockAutomaticRulesListWidget::setDisabledRules(const QStringList &disabledRules)
{
    mDisabledRules = disabledRules;
}

void AdBlockAutomaticRulesListWidget::setRules(const QString &rules)
{
    clear();
    const QStringList lst = rules.split(QLatin1Char('\n'), QString::SkipEmptyParts);
    Q_FOREACH (const QString &rule, lst) {
        createItem(rule);
    }
}

void AdBlockAutomaticRulesListWidget::createItem(const QString &rule)
{
    QListWidgetItem *subItem = new QListWidgetItem(this);
    subItem->setText(rule);
    if (rule.startsWith(QLatin1Char('!'))) {
        //Comment
        subItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    } else {
        subItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
        const bool checkState = mDisabledRules.contains(rule);
        subItem->setCheckState(checkState ? Qt::Unchecked : Qt::Checked);
        if (checkState) {
            QFont font = subItem->font();
            font.setItalic(true);
            subItem->setFont(font);
        }
    }
    //TODO define color here.
}

QStringList AdBlockAutomaticRulesListWidget::disabledRules() const
{
    QStringList currentDisabledRules;
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *row = item(i);
        if (row->flags() & Qt::ItemIsUserCheckable) {
            if (row->checkState() == Qt::Unchecked) {
                currentDisabledRules << row->text();
            }
        }
    }
    return currentDisabledRules;
}
