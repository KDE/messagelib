/*
   Copyright (C) 2012-2020 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef IMAGESCALINGWIDGET_H
#define IMAGESCALINGWIDGET_H
#include <QWidget>
#include "messagecomposer_export.h"

class QComboBox;
class QAbstractButton;
namespace Ui {
class ImageScalingWidget;
}

namespace MessageComposer {
class ImageScalingWidgetPrivate;
/**
 * @brief The ImageScalingWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT ImageScalingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageScalingWidget(QWidget *parent = nullptr);
    ~ImageScalingWidget();

    void loadConfig();
    void writeConfig();
    void resetToDefault();

Q_SIGNALS:
    void changed();

private Q_SLOTS:
    void slotComboboxChanged(int index);
    void slotSourceFilterClicked(int);
    void slotRecipientFilterClicked(int);
    void slotHelpLinkClicked(const QString &);

private:
    void slotRecipientFilterAbstractClicked(QAbstractButton *button);
    void slotSourceFilterAbstractButtonClicked(QAbstractButton *button);
    void updateFilterSourceTypeSettings();
    void initComboBox(QComboBox *combo);
    void initWriteImageFormat();
    void updateEmailsFilterTypeSettings();
    void updateSettings();
    ImageScalingWidgetPrivate *const d;
};
}

#endif // IMAGESCALINGWIDGET_H
