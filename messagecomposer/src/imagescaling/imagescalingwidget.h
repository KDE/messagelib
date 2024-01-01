/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

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
    MESSAGECOMPOSER_NO_EXPORT void slotComboboxChanged(int index);
    MESSAGECOMPOSER_NO_EXPORT void slotSourceFilterClicked(int);
    MESSAGECOMPOSER_NO_EXPORT void slotRecipientFilterClicked(int);
    MESSAGECOMPOSER_NO_EXPORT void slotHelpLinkClicked(const QString &);

private:
    MESSAGECOMPOSER_NO_EXPORT void slotRecipientFilterAbstractClicked(QAbstractButton *button);
    MESSAGECOMPOSER_NO_EXPORT void slotSourceFilterAbstractButtonClicked(QAbstractButton *button);
    MESSAGECOMPOSER_NO_EXPORT void updateFilterSourceTypeSettings();
    MESSAGECOMPOSER_NO_EXPORT void initComboBox(QComboBox *combo);
    MESSAGECOMPOSER_NO_EXPORT void initWriteImageFormat();
    MESSAGECOMPOSER_NO_EXPORT void updateEmailsFilterTypeSettings();
    MESSAGECOMPOSER_NO_EXPORT void updateSettings();
    std::unique_ptr<ImageScalingWidgetPrivate> const d;
};
}
