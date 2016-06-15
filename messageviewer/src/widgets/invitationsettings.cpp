/*
    Boost Software License - Version 1.0 - August 17th, 2003

    Permission is hereby granted, free of charge, to any person or organization
    obtaining a copy of the software and accompanying documentation covered by
    this license (the "Software") to use, reproduce, display, distribute,
    execute, and transmit the Software, and to prepare derivative works of the
    Software, and to permit third-parties to whom the Software is furnished to
    do so, all subject to the following:

    The copyright notices in the Software and this entire statement, including
    the above license grant, this restriction and the following disclaimer,
    must be included in all copies of the Software, in whole or in part, and
    all derivative works of the Software, unless such copies or derivative
    works are solely in the form of machine-executable object code generated by
    a source language processor.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*/

#include "invitationsettings.h"
#include "ui_invitationsettings.h"
#include "settings/messageviewersettings.h"
#include "PimCommon/ConfigureImmutableWidgetUtils"
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
        mInvitationUi = 0;
    }
    Ui_InvitationSettings *mInvitationUi;
};

InvitationSettings::InvitationSettings(QWidget *parent)
    : QWidget(parent), d(new MessageViewer::InvitationSettingsPrivate)
{
    d->mInvitationUi->setupUi(this);

    d->mInvitationUi->mDeleteInvitations->setText(
        i18n(MessageViewer::MessageViewerSettings::self()->
             deleteInvitationEmailsAfterSendingReplyItem()->label().toUtf8().constData()));
    d->mInvitationUi->mDeleteInvitations->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->
             deleteInvitationEmailsAfterSendingReplyItem()->whatsThis().toUtf8().constData()));
    connect(d->mInvitationUi->mDeleteInvitations, &QCheckBox::toggled, this, &InvitationSettings::changed);

    d->mInvitationUi->mLegacyMangleFromTo->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->legacyMangleFromToHeadersItem()->whatsThis().toUtf8().constData()));
    connect(d->mInvitationUi->mLegacyMangleFromTo, &QCheckBox::stateChanged, this, &InvitationSettings::changed);

    d->mInvitationUi->mLegacyBodyInvites->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->legacyBodyInvitesItem()->whatsThis().toUtf8().constData()));
    connect(d->mInvitationUi->mLegacyBodyInvites, &QCheckBox::toggled, this, &InvitationSettings::slotLegacyBodyInvitesToggled);
    connect(d->mInvitationUi->mLegacyBodyInvites, &QCheckBox::stateChanged, this, &InvitationSettings::changed);

    d->mInvitationUi->mExchangeCompatibleInvitations->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->exchangeCompatibleInvitationsItem()->whatsThis().toUtf8().constData()));
    connect(d->mInvitationUi->mExchangeCompatibleInvitations, &QCheckBox::stateChanged, this, &InvitationSettings::changed);

    //Laurent BUG:257723: in kmail2 it's not possible to not send automatically.
    d->mInvitationUi->mAutomaticSending->hide();
    d->mInvitationUi->mAutomaticSending->setWhatsThis(
        i18n(MessageViewer::MessageViewerSettings::self()->automaticSendingItem()->whatsThis().toUtf8().constData()));
    connect(d->mInvitationUi->mAutomaticSending, &QCheckBox::stateChanged, this, &InvitationSettings::changed);
}

InvitationSettings::~InvitationSettings()
{
    delete d;
}

void InvitationSettings::slotLegacyBodyInvitesToggled(bool on)
{
    if (on) {
        const QString txt = i18n("<qt>Invitations are normally sent as attachments to "
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
    //TODO verify it
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
    //TODO verify it
    d->mInvitationUi->mAutomaticSending->setEnabled(!d->mInvitationUi->mLegacyBodyInvites->isChecked());
    loadWidget(d->mInvitationUi->mDeleteInvitations, MessageViewer::MessageViewerSettings::self()->deleteInvitationEmailsAfterSendingReplyItem());
    MessageViewer::MessageViewerSettings::self()->useDefaults(bUseDefaults);
}
