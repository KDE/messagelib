/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QLabel>
namespace MessageComposer
{
/**
 * @brief The StatusBarLabelToggledState class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT StatusBarLabelToggledState : public QLabel
{
    Q_OBJECT
public:
    explicit StatusBarLabelToggledState(QWidget *parent = nullptr);
    ~StatusBarLabelToggledState() override;

    void setToggleMode(bool state);

    [[nodiscard]] bool toggleMode() const;

    void setStateString(const QString &toggled, const QString &untoggled);
Q_SIGNALS:
    void toggleModeChanged(bool state);

protected:
    void mousePressEvent(QMouseEvent *ev) override;

private:
    MESSAGECOMPOSER_NO_EXPORT void updateLabel();
    QString mToggled;
    QString mUnToggled;
    bool mToggleMode = false;
};
}
