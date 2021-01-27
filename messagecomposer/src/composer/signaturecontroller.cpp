/*
 * SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
 *
 * Based on kmail/kmcomposewin.cpp
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * Based on KMail code by:
 * SPDX-FileCopyrightText: 1997 Markus Wuebben <markus.wuebben@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "signaturecontroller.h"
#include "composer-ng/richtextcomposerng.h"
#include "composer-ng/richtextcomposersignatures.h"
#include "settings/messagecomposersettings.h"

#include <KIdentityManagement/kidentitymanagement/identity.h>
#include <KIdentityManagement/kidentitymanagement/identitycombo.h>
#include <KIdentityManagement/kidentitymanagement/identitymanager.h>

using namespace MessageComposer;

class MessageComposer::SignatureControllerPrivate
{
public:
    SignatureControllerPrivate()
    {
    }

    uint m_currentIdentityId = 0;
    KIdentityManagement::IdentityCombo *m_identityCombo = nullptr;
    MessageComposer::RichTextComposerNg *m_editor = nullptr;
};

SignatureController::SignatureController(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::SignatureControllerPrivate)
{
}

SignatureController::~SignatureController()
{
    delete d;
}

void SignatureController::setEditor(MessageComposer::RichTextComposerNg *editor)
{
    d->m_editor = editor;
}

void SignatureController::setIdentityCombo(KIdentityManagement::IdentityCombo *combo)
{
    d->m_identityCombo = combo;
    d->m_currentIdentityId = combo->currentIdentity();
    resume();
}

void SignatureController::identityChanged(uint id)
{
    Q_ASSERT(d->m_identityCombo);
    const KIdentityManagement::Identity &newIdentity = d->m_identityCombo->identityManager()->identityForUoid(id);
    if (newIdentity.isNull() || !d->m_editor) {
        return;
    }

    const KIdentityManagement::Identity &oldIdentity = d->m_identityCombo->identityManager()->identityForUoidOrDefault(d->m_currentIdentityId);

    const KIdentityManagement::Signature oldSig = const_cast<KIdentityManagement::Identity &>(oldIdentity).signature();
    const KIdentityManagement::Signature newSig = const_cast<KIdentityManagement::Identity &>(newIdentity).signature();
    const bool replaced = d->m_editor->composerSignature()->replaceSignature(oldSig, newSig);

    // Just append the signature if there was no old signature
    if (!replaced && oldSig.rawText().isEmpty()) {
        applySignature(newSig);
    }

    d->m_currentIdentityId = id;
}

void SignatureController::suspend()
{
    if (d->m_identityCombo) {
        disconnect(d->m_identityCombo, &KIdentityManagement::IdentityCombo::identityChanged, this, &SignatureController::identityChanged);
    }
}

void SignatureController::resume()
{
    if (d->m_identityCombo) {
        connect(d->m_identityCombo, &KIdentityManagement::IdentityCombo::identityChanged, this, &SignatureController::identityChanged);
    }
}

void SignatureController::appendSignature()
{
    insertSignatureHelper(KIdentityManagement::Signature::End);
}

void SignatureController::prependSignature()
{
    insertSignatureHelper(KIdentityManagement::Signature::Start);
}

void SignatureController::insertSignatureAtCursor()
{
    insertSignatureHelper(KIdentityManagement::Signature::AtCursor);
}

void SignatureController::cleanSpace()
{
    if (!d->m_editor || !d->m_identityCombo) {
        return;
    }
    const KIdentityManagement::Identity &ident = d->m_identityCombo->identityManager()->identityForUoidOrDefault(d->m_identityCombo->currentIdentity());
    const KIdentityManagement::Signature signature = const_cast<KIdentityManagement::Identity &>(ident).signature();
    d->m_editor->composerSignature()->cleanWhitespace(signature);
}

void SignatureController::insertSignatureHelper(KIdentityManagement::Signature::Placement placement)
{
    if (!d->m_identityCombo || !d->m_editor) {
        return;
    }

    // Identity::signature() is not const, although it should be, therefore the
    // const_cast.
    auto &ident =
        const_cast<KIdentityManagement::Identity &>(d->m_identityCombo->identityManager()->identityForUoidOrDefault(d->m_identityCombo->currentIdentity()));
    const KIdentityManagement::Signature signature = ident.signature();

    if (signature.isInlinedHtml() && signature.type() == KIdentityManagement::Signature::Inlined) {
        Q_EMIT enableHtml();
    }

    KIdentityManagement::Signature::AddedText addedText = KIdentityManagement::Signature::AddNewLines;
    if (MessageComposer::MessageComposerSettings::self()->dashDashSignature()) {
        addedText |= KIdentityManagement::Signature::AddSeparator;
    }
    d->m_editor->insertSignature(signature, placement, addedText);
    if ((placement == KIdentityManagement::Signature::Start) || (placement == KIdentityManagement::Signature::End)) {
        Q_EMIT signatureAdded();
    }
}

void SignatureController::applySignature(const KIdentityManagement::Signature &signature)
{
    if (!d->m_editor) {
        return;
    }

    if (MessageComposer::MessageComposerSettings::self()->autoTextSignature() == QLatin1String("auto")) {
        KIdentityManagement::Signature::AddedText addedText = KIdentityManagement::Signature::AddNewLines;
        if (MessageComposer::MessageComposerSettings::self()->dashDashSignature()) {
            addedText |= KIdentityManagement::Signature::AddSeparator;
        }
        if (MessageComposer::MessageComposerSettings::self()->prependSignature()) {
            d->m_editor->insertSignature(signature, KIdentityManagement::Signature::Start, addedText);
        } else {
            d->m_editor->insertSignature(signature, KIdentityManagement::Signature::End, addedText);
        }
    }
}
