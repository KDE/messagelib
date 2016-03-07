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

#ifndef ADBLOCKAUTOMATICRULESLISTWIDGET_H
#define ADBLOCKAUTOMATICRULESLISTWIDGET_H

#include <QListWidget>

namespace MessageViewer
{
class AdBlockAutomaticRulesListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit AdBlockAutomaticRulesListWidget(QWidget *parent = Q_NULLPTR);
    ~AdBlockAutomaticRulesListWidget();
    void setRules(const QString &rules);

    void setDisabledRules(const QStringList &disabledRules);
    QStringList disabledRules() const;
private:
    void createItem(const QString &rule);
};
}

#endif // ADBLOCKAUTOMATICRULESLISTWIDGET_H
