/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
namespace KIdentityManagement
{
class Signature;
}

namespace MessageComposer
{
class RichTextComposerNg;
/**
 * @brief The RichTextComposerSignatures class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT RichTextComposerSignatures : public QObject
{
    Q_OBJECT
public:
    explicit RichTextComposerSignatures(MessageComposer::RichTextComposerNg *composer, QObject *parent = nullptr);
    ~RichTextComposerSignatures() override;

    void cleanWhitespace(const KIdentityManagement::Signature &sig);

    Q_REQUIRED_RESULT bool replaceSignature(const KIdentityManagement::Signature &oldSig, const KIdentityManagement::Signature &newSig);

private:
    class RichTextComposerSignaturesPrivate;
    RichTextComposerSignaturesPrivate *const d;
};
}
