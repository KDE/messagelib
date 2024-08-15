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

#pragma once

#include "messagecomposer_export.h"
#include <KIdentityManagementCore/Signature>
#include <QObject>

namespace KIdentityManagementWidgets
{
class IdentityCombo;
}

namespace MessageComposer
{
class RichTextComposerNg;
class SignatureControllerPrivate;
/**
 * @brief The SignatureController class
 * Controls signature (the footer thing, not the crypto thing) operations
 *  happening on a KMEditor triggered by identity selection or menu actions.
 *  @since 4.5
 */
class MESSAGECOMPOSER_EXPORT SignatureController : public QObject
{
    Q_OBJECT
public:
    explicit SignatureController(QObject *parent = nullptr);
    ~SignatureController() override;

    void setEditor(MessageComposer::RichTextComposerNg *editor);
    void setIdentityCombo(KIdentityManagementWidgets::IdentityCombo *combo);

    /** Temporarily disable identity tracking, useful for initial loading for example. */
    void suspend();
    /** Resume identity change tracking after a previous call to suspend(). */
    void resume();

    /** Adds the given signature to the editor, taking user preferences into account.
     */
    void applySignature(const KIdentityManagementCore::Signature &signature);

public Q_SLOTS:
    /**
     * Append signature to the end of the text in the editor.
     */
    void appendSignature();

    /**
     * Prepend signature at the beginning of the text in the editor.
     */
    void prependSignature();

    /**
     * Insert signature at the cursor position of the text in the editor.
     */
    void insertSignatureAtCursor();

    void cleanSpace();

Q_SIGNALS:
    /**
     * A HTML signature is about to be inserted, so enable HTML support in the editor.
     */
    void enableHtml();
    void signatureAdded();

private:
    /**
     * Helper to insert the signature of the current identity arbitrarily
     * in the editor, connecting slot functions to KMeditor::insertSignature().
     * @param placement the position of the signature
     */
    MESSAGECOMPOSER_NO_EXPORT void insertSignatureHelper(KIdentityManagementCore::Signature::Placement placement);
    MESSAGECOMPOSER_NO_EXPORT void identityChanged(uint id);

    std::unique_ptr<SignatureControllerPrivate> const d;
};
}
