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

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <KIdentityManagementWidgets/IdentityCombo>

using namespace MessageComposer;

class MessageComposer::SignatureControllerPrivate
{
public:
    SignatureControllerPrivate() = default;

    uint m_currentIdentityId = 0;
    KIdentityManagementWidgets::IdentityCombo *m_identityCombo = nullptr;
    MessageComposer::RichTextComposerNg *m_editor = nullptr;
};

SignatureController::SignatureController(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::SignatureControllerPrivate)
{
}

SignatureController::~SignatureController() = default;

void SignatureController::setEditor(MessageComposer::RichTextComposerNg *editor)
{
    d->m_editor = editor;
}

void SignatureController::setIdentityCombo(KIdentityManagementWidgets::IdentityCombo *combo)
{
    d->m_identityCombo = combo;
    d->m_currentIdentityId = combo->currentIdentity();
    resume();
}

void SignatureController::identityChanged(uint id)
{
    Q_ASSERT(d->m_identityCombo);
    const KIdentityManagementCore::Identity &newIdentity = d->m_identityCombo->identityManager()->identityForUoid(id);
    if (newIdentity.isNull() || !d->m_editor) {
        return;
    }

    const KIdentityManagementCore::Identity &oldIdentity = d->m_identityCombo->identityManager()->identityForUoidOrDefault(d->m_currentIdentityId);

    const KIdentityManagementCore::Signature oldSig = const_cast<KIdentityManagementCore::Identity &>(oldIdentity).signature();
    const KIdentityManagementCore::Signature newSig = const_cast<KIdentityManagementCore::Identity &>(newIdentity).signature();
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
        disconnect(d->m_identityCombo, &KIdentityManagementWidgets::IdentityCombo::identityChanged, this, &SignatureController::identityChanged);
    }
}

void SignatureController::resume()
{
    if (d->m_identityCombo) {
        connect(d->m_identityCombo, &KIdentityManagementWidgets::IdentityCombo::identityChanged, this, &SignatureController::identityChanged);
    }
}

void SignatureController::appendSignature()
{
    insertSignatureHelper(KIdentityManagementCore::Signature::End);
}

void SignatureController::prependSignature()
{
    insertSignatureHelper(KIdentityManagementCore::Signature::Start);
}

void SignatureController::insertSignatureAtCursor()
{
    insertSignatureHelper(KIdentityManagementCore::Signature::AtCursor);
}

void SignatureController::cleanSpace()
{
    if (!d->m_editor || !d->m_identityCombo) {
        return;
    }
    const KIdentityManagementCore::Identity &ident = d->m_identityCombo->identityManager()->identityForUoidOrDefault(d->m_identityCombo->currentIdentity());
    const KIdentityManagementCore::Signature signature = const_cast<KIdentityManagementCore::Identity &>(ident).signature();
    d->m_editor->composerSignature()->cleanWhitespace(signature);
}

void SignatureController::insertSignatureHelper(KIdentityManagementCore::Signature::Placement placement)
{
    if (!d->m_identityCombo || !d->m_editor) {
        return;
    }

    // Identity::signature() is not const, although it should be, therefore the
    // const_cast.
    auto &ident =
        const_cast<KIdentityManagementCore::Identity &>(d->m_identityCombo->identityManager()->identityForUoidOrDefault(d->m_identityCombo->currentIdentity()));
    const KIdentityManagementCore::Signature signature = ident.signature();

    if (signature.isInlinedHtml() && signature.type() == KIdentityManagementCore::Signature::Inlined) {
        Q_EMIT enableHtml();
    }

    KIdentityManagementCore::Signature::AddedText addedText = KIdentityManagementCore::Signature::AddNewLines;
    if (MessageComposer::MessageComposerSettings::self()->dashDashSignature()) {
        addedText |= KIdentityManagementCore::Signature::AddSeparator;
    }
    d->m_editor->insertSignature(signature, placement, addedText);
    if ((placement == KIdentityManagementCore::Signature::Start) || (placement == KIdentityManagementCore::Signature::End)) {
        Q_EMIT signatureAdded();
    }
}

void SignatureController::applySignature(const KIdentityManagementCore::Signature &signature)
{
    if (!d->m_editor) {
        return;
    }

    if (MessageComposer::MessageComposerSettings::self()->autoTextSignature() == QLatin1StringView("auto")) {
        KIdentityManagementCore::Signature::AddedText addedText = KIdentityManagementCore::Signature::AddNewLines;
        if (MessageComposer::MessageComposerSettings::self()->dashDashSignature()) {
            addedText |= KIdentityManagementCore::Signature::AddSeparator;
        }
        if (MessageComposer::MessageComposerSettings::self()->prependSignature()) {
            d->m_editor->insertSignature(signature, KIdentityManagementCore::Signature::Start, addedText);
        } else {
            d->m_editor->insertSignature(signature, KIdentityManagementCore::Signature::End, addedText);
        }
    }
}

#include "moc_signaturecontroller.cpp"
