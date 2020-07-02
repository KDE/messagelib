/*
    SPDX-License-Identifier: BSL-1.0
*/

#ifndef MESSAGEVIEWER_INVITATIONSETTINGS_H
#define MESSAGEVIEWER_INVITATIONSETTINGS_H

#include "messageviewer_export.h"

#include <QWidget>

namespace MessageViewer {
class InvitationSettingsPrivate;
/**
 * @brief The InvitationSettings class
 */
class MESSAGEVIEWER_EXPORT InvitationSettings : public QWidget
{
    Q_OBJECT
public:
    explicit InvitationSettings(QWidget *parent = nullptr);
    ~InvitationSettings();
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

#endif // MESSAGEVIEWER_INVITATIONSETTINGS_H
