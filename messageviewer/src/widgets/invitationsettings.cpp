/*
    SPDX-License-Identifier: BSL-1.0
*/

#include "invitationsettings.h"
#include "settings/messageviewersettings.h"
#include "ui_invitationsettings.h"
#include <PimCommon/ConfigureImmutableWidgetUtils>
using namespace PimCommon::ConfigureImmutableWidgetUtils;

#include <KLocalizedString>
#include <KMessageBox>

using namespace MessageViewer;

class MessageViewer::InvitationSettingsPrivate
{
public:
    InvitationSettingsPrivate()
        : mInvitationUi(new Ui_InvitationSettings)
    {
    }

    ~InvitationSettingsPrivate()
    {
        delete mInvitationUi;
    }

    Ui_InvitationSettings *const mInvitationUi;
};

InvitationSettings::InvitationSettings(QWidget *parent)
    : QWidget(parent)
    , d(new MessageViewer::InvitationSettingsPrivate)
{
    d->mInvitationUi->setupUi(this);

    d->mInvitationUi->mDeleteInvitations->setText(
        i18n(MessageViewer::MessageViewerSettings::self()->deleteInvitationEmailsAfterSendingReplyItem()->label().toUtf8().constData()));
    d->mInvitationUi->mDeleteInvitations->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->deleteInvitationEmailsAfterSendingReplyItem()->whatsThis().toUtf8().constData()));
    connect(d->mInvitationUi->mDeleteInvitations, &QCheckBox::toggled, this, &InvitationSettings::changed);

    d->mInvitationUi->mLegacyMangleFromTo->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->legacyMangleFromToHeadersItem()->whatsThis().toUtf8().constData()));
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    connect(d->mInvitationUi->mLegacyMangleFromTo, &QCheckBox::stateChanged, this, &InvitationSettings::changed);
#else
    connect(d->mInvitationUi->mLegacyMangleFromTo, &QCheckBox::checkStateChanged, this, &InvitationSettings::changed);
#endif

    d->mInvitationUi->mLegacyBodyInvites->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->legacyBodyInvitesItem()->whatsThis().toUtf8().constData()));
    connect(d->mInvitationUi->mLegacyBodyInvites, &QCheckBox::toggled, this, &InvitationSettings::slotLegacyBodyInvitesToggled);
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    connect(d->mInvitationUi->mLegacyBodyInvites, &QCheckBox::stateChanged, this, &InvitationSettings::changed);
#else
    connect(d->mInvitationUi->mLegacyBodyInvites, &QCheckBox::checkStateChanged, this, &InvitationSettings::changed);
#endif

    d->mInvitationUi->mExchangeCompatibleInvitations->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->exchangeCompatibleInvitationsItem()->whatsThis().toUtf8().constData()));
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    connect(d->mInvitationUi->mExchangeCompatibleInvitations, &QCheckBox::stateChanged, this, &InvitationSettings::changed);
#else
    connect(d->mInvitationUi->mExchangeCompatibleInvitations, &QCheckBox::checkStateChanged, this, &InvitationSettings::changed);
#endif

    // Laurent BUG:257723: in kmail2 it's not possible to not send automatically.
    d->mInvitationUi->mAutomaticSending->hide();
    d->mInvitationUi->mAutomaticSending->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->automaticSendingItem()->whatsThis().toUtf8().constData()));
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    connect(d->mInvitationUi->mAutomaticSending, &QCheckBox::stateChanged, this, &InvitationSettings::changed);
#else
    connect(d->mInvitationUi->mAutomaticSending, &QCheckBox::checkStateChanged, this, &InvitationSettings::changed);
#endif
}

InvitationSettings::~InvitationSettings() = default;

void InvitationSettings::slotLegacyBodyInvitesToggled(bool on)
{
    if (on) {
        const QString txt = i18n(
            "<qt>Invitations are normally sent as attachments to "
            "a mail. This switch changes the invitation mails to "
            "be sent in the text of the mail instead; this is "
            "necessary to send invitations and replies to "
            "Microsoft Outlook.<br />But, when you do this, you no "
            "longer get descriptive text that mail programs "
            "can read; so, to people who have email programs "
            "that do not understand the invitations, the "
            "resulting messages look very odd.<br />People that have email "
            "programs that do understand invitations will still "
            "be able to work with this.</qt>");
        KMessageBox::information(this, txt, QString(), QStringLiteral("LegacyBodyInvitesWarning"));
    }
    // Invitations in the body are autosent in any case (no point in editing raw ICAL)
    // So the autosend option is only available if invitations are sent as attachment.
    d->mInvitationUi->mAutomaticSending->setEnabled(!d->mInvitationUi->mLegacyBodyInvites->isChecked());
}

void InvitationSettings::doLoadFromGlobalSettings()
{
    loadWidget(d->mInvitationUi->mLegacyMangleFromTo, MessageViewer::MessageViewerSettings::self()->legacyMangleFromToHeadersItem());
    d->mInvitationUi->mLegacyBodyInvites->blockSignals(true);
    loadWidget(d->mInvitationUi->mLegacyBodyInvites, MessageViewer::MessageViewerSettings::self()->legacyBodyInvitesItem());
    d->mInvitationUi->mLegacyBodyInvites->blockSignals(false);
    loadWidget(d->mInvitationUi->mExchangeCompatibleInvitations, MessageViewer::MessageViewerSettings::self()->exchangeCompatibleInvitationsItem());
    loadWidget(d->mInvitationUi->mAutomaticSending, MessageViewer::MessageViewerSettings::self()->automaticSendingItem());
    // TODO verify it
    d->mInvitationUi->mAutomaticSending->setEnabled(!d->mInvitationUi->mLegacyBodyInvites->isChecked());
    loadWidget(d->mInvitationUi->mDeleteInvitations, MessageViewer::MessageViewerSettings::self()->deleteInvitationEmailsAfterSendingReplyItem());
}

void InvitationSettings::save()
{
    saveCheckBox(d->mInvitationUi->mLegacyMangleFromTo, MessageViewer::MessageViewerSettings::self()->legacyMangleFromToHeadersItem());
    saveCheckBox(d->mInvitationUi->mLegacyBodyInvites, MessageViewer::MessageViewerSettings::self()->legacyBodyInvitesItem());
    saveCheckBox(d->mInvitationUi->mExchangeCompatibleInvitations, MessageViewer::MessageViewerSettings::self()->exchangeCompatibleInvitationsItem());
    saveCheckBox(d->mInvitationUi->mAutomaticSending, MessageViewer::MessageViewerSettings::self()->automaticSendingItem());
    saveCheckBox(d->mInvitationUi->mDeleteInvitations, MessageViewer::MessageViewerSettings::self()->deleteInvitationEmailsAfterSendingReplyItem());
}

QString InvitationSettings::helpAnchor() const
{
    return QStringLiteral("configure-misc-invites");
}

void InvitationSettings::doResetToDefaultsOther()
{
    const bool bUseDefaults = MessageViewer::MessageViewerSettings::self()->useDefaults(true);
    loadWidget(d->mInvitationUi->mLegacyMangleFromTo, MessageViewer::MessageViewerSettings::self()->legacyMangleFromToHeadersItem());
    d->mInvitationUi->mLegacyBodyInvites->blockSignals(true);
    loadWidget(d->mInvitationUi->mLegacyBodyInvites, MessageViewer::MessageViewerSettings::self()->legacyBodyInvitesItem());
    d->mInvitationUi->mLegacyBodyInvites->blockSignals(false);
    loadWidget(d->mInvitationUi->mExchangeCompatibleInvitations, MessageViewer::MessageViewerSettings::self()->exchangeCompatibleInvitationsItem());
    loadWidget(d->mInvitationUi->mAutomaticSending, MessageViewer::MessageViewerSettings::self()->automaticSendingItem());
    // TODO verify it
    d->mInvitationUi->mAutomaticSending->setEnabled(!d->mInvitationUi->mLegacyBodyInvites->isChecked());
    loadWidget(d->mInvitationUi->mDeleteInvitations, MessageViewer::MessageViewerSettings::self()->deleteInvitationEmailsAfterSendingReplyItem());
    MessageViewer::MessageViewerSettings::self()->useDefaults(bUseDefaults);
}

#include "moc_invitationsettings.cpp"
