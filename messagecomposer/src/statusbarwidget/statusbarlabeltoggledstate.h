/*
   SPDX-FileCopyrightText: 2014-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QLabel>
namespace MessageComposer
{
/*!
 * \class MessageComposer::StatusBarLabelToggledState
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/StatusBarLabelToggledState
 * \brief The StatusBarLabelToggledState class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT StatusBarLabelToggledState : public QLabel
{
    Q_OBJECT
public:
    /*! \brief Constructs a StatusBarLabelToggledState.
        \param parent The parent widget.
    */
    explicit StatusBarLabelToggledState(QWidget *parent = nullptr);
    /*! \brief Destroys the StatusBarLabelToggledState. */
    ~StatusBarLabelToggledState() override;

    /*! \brief Sets the toggle mode state.
        \param state True to set toggle mode on.
    */
    void setToggleMode(bool state);

    /*! \brief Returns the current toggle mode state. */
    [[nodiscard]] bool toggleMode() const;

    /*! \brief Sets the state strings for the label.
        \param toggled The text to display when toggled on.
        \param untoggled The text to display when toggled off.
    */
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
