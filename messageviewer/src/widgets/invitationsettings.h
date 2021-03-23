/*
    SPDX-License-Identifier: BSL-1.0
*/

#pragma once

#include "messageviewer_export.h"

#include <QWidget>

namespace MessageViewer
{
class InvitationSettingsPrivate;
/**
 * @brief The InvitationSettings class
 */
class MESSAGEVIEWER_EXPORT InvitationSettings : public QWidget
{
    Q_OBJECT
public:
    explicit InvitationSettings(QWidget *parent = nullptr);
    ~InvitationSettings() override;
    void save();
    Q_REQUIRED_RESULT QString helpAnchor() const;
    void doLoadFromGlobalSettings();
    void doResetToDefaultsOther();

Q_SIGNALS:
    void changed();

private:
    void slotLegacyBodyInvitesToggled(bool on);
    InvitationSettingsPrivate *const d;
};
}

