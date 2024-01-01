/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
#include <memory>
namespace KIdentityManagementCore
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

    void cleanWhitespace(const KIdentityManagementCore::Signature &sig);

    [[nodiscard]] bool replaceSignature(const KIdentityManagementCore::Signature &oldSig, const KIdentityManagementCore::Signature &newSig);

private:
    class RichTextComposerSignaturesPrivate;
    std::unique_ptr<RichTextComposerSignaturesPrivate> const d;
};
}
