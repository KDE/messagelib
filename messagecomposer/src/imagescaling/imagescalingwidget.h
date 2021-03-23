/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagecomposer_export.h"
#include <QWidget>

class QComboBox;
class QAbstractButton;
namespace Ui
{
class ImageScalingWidget;
}

namespace MessageComposer
{
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
    ~ImageScalingWidget() override;

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

