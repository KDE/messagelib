/*
  Copyright (c) 2017 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "generatecryptomessagesjob.h"
#include "messagecomposer_debug.h"
#include "settings/messagecomposersettings.h"
#include "composer/keyresolver.h"
#include <KLocalizedString>

using namespace MessageComposer;

namespace
{

// helper methods for reading encryption settings

inline int encryptKeyNearExpiryWarningThresholdInDays()
{
    if (! MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return -1;
    }
    const int num =
        MessageComposer::MessageComposerSettings::self()->cryptoWarnEncrKeyNearExpiryThresholdDays();
    return qMax(1, num);
}

inline int signingKeyNearExpiryWarningThresholdInDays()
{
    if (! MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return -1;
    }
    const int num =
        MessageComposer::MessageComposerSettings::self()->cryptoWarnSignKeyNearExpiryThresholdDays();
    return qMax(1, num);
}

inline int encryptRootCertNearExpiryWarningThresholdInDays()
{
    if (! MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return -1;
    }
    const int num =
        MessageComposer::MessageComposerSettings::self()->cryptoWarnEncrRootNearExpiryThresholdDays();
    return qMax(1, num);
}

inline int signingRootCertNearExpiryWarningThresholdInDays()
{
    if (! MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return -1;
    }
    const int num =
        MessageComposer::MessageComposerSettings::self()->cryptoWarnSignRootNearExpiryThresholdDays();
    return qMax(1, num);
}

inline int encryptChainCertNearExpiryWarningThresholdInDays()
{
    if (! MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return -1;
    }
    const int num =
        MessageComposer::MessageComposerSettings::self()->cryptoWarnEncrChaincertNearExpiryThresholdDays();
    return qMax(1, num);
}

inline int signingChainCertNearExpiryWarningThresholdInDays()
{
    if (! MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return -1;
    }
    const int num =
        MessageComposer::MessageComposerSettings::self()->cryptoWarnSignChaincertNearExpiryThresholdDays();;
    return qMax(1, num);
}

inline bool encryptToSelf()
{
    return MessageComposer::MessageComposerSettings::self()->cryptoEncryptToSelf();
}

inline bool showKeyApprovalDialog()
{
    return MessageComposer::MessageComposerSettings::self()->cryptoShowKeysForApproval();
}

} // nameless namespace


GenerateCryptoMessagesJob::GenerateCryptoMessagesJob(QObject *parent)
    : QObject(parent)
{

}

GenerateCryptoMessagesJob::~GenerateCryptoMessagesJob()
{

}

#if 0
QList< MessageComposer::Composer * > ComposerViewBase::generateCryptoMessages(bool &wasCanceled)
{
    const KIdentityManagement::Identity &id = m_identMan->identityForUoidOrDefault(m_identityCombo->currentIdentity());

    qCDebug(MESSAGECOMPOSER_LOG) << "filling crypto info";
    Kleo::KeyResolver *keyResolver = new Kleo::KeyResolver(encryptToSelf(),
            showKeyApprovalDialog(),
            id.pgpAutoEncrypt(),
            m_cryptoMessageFormat,
            encryptKeyNearExpiryWarningThresholdInDays(),
            signingKeyNearExpiryWarningThresholdInDays(),
            encryptRootCertNearExpiryWarningThresholdInDays(),
            signingRootCertNearExpiryWarningThresholdInDays(),
            encryptChainCertNearExpiryWarningThresholdInDays(),
            signingChainCertNearExpiryWarningThresholdInDays());

    QStringList encryptToSelfKeys;
    QStringList signKeys;

    bool signSomething = m_sign;
    bool doSignCompletely = m_sign;
    bool encryptSomething = m_encrypt;
    bool doEncryptCompletely = m_encrypt;

    //Add encryptionkeys from id to keyResolver
    qCDebug(MESSAGECOMPOSER_LOG) << id.pgpEncryptionKey().isEmpty() << id.smimeEncryptionKey().isEmpty();
    if (!id.pgpEncryptionKey().isEmpty()) {
        encryptToSelfKeys.push_back(QLatin1String(id.pgpEncryptionKey()));
    }
    if (!id.smimeEncryptionKey().isEmpty()) {
        encryptToSelfKeys.push_back(QLatin1String(id.smimeEncryptionKey()));
    }
    if (keyResolver->setEncryptToSelfKeys(encryptToSelfKeys) != Kleo::Ok) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Failed to set encryptoToSelf keys!";
        return QList< MessageComposer::Composer * >();
    }

    //Add signingkeys from id to keyResolver
    if (!id.pgpSigningKey().isEmpty()) {
        signKeys.push_back(QLatin1String(id.pgpSigningKey()));
    }
    if (!id.smimeSigningKey().isEmpty()) {
        signKeys.push_back(QLatin1String(id.smimeSigningKey()));
    }
    if (keyResolver->setSigningKeys(signKeys) != Kleo::Ok) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Failed to set signing keys!";
        return QList< MessageComposer::Composer * >();
    }

    if (m_attachmentModel) {
        foreach (const MessageCore::AttachmentPart::Ptr &attachment, m_attachmentModel->attachments()) {
            if (attachment->isSigned()) {
                signSomething = true;
            } else {
                doEncryptCompletely = false;
            }
            if (attachment->isEncrypted()) {
                encryptSomething = true;
            } else {
                doSignCompletely = false;
            }
        }
    }

    QStringList recipients(mExpandedTo), bcc(mExpandedBcc);
    recipients.append(mExpandedCc);

    keyResolver->setPrimaryRecipients(recipients);
    keyResolver->setSecondaryRecipients(bcc);

    bool result = true;
    bool canceled = false;
    signSomething = determineWhetherToSign(doSignCompletely, keyResolver, signSomething, result, canceled);
    if (!result) {
        // TODO handle failure
        qCDebug(MESSAGECOMPOSER_LOG) << "determineWhetherToSign: failed to resolve keys! oh noes";
        if (!canceled) {
            Q_EMIT failed(i18n("Failed to resolve keys. Please report a bug."));
        } else {
            Q_EMIT failed(QString());
        }
        wasCanceled = canceled;
        return QList< MessageComposer::Composer *>();
    }

    canceled = false;
    encryptSomething = determineWhetherToEncrypt(doEncryptCompletely, keyResolver, encryptSomething, signSomething, result, canceled);
    if (!result) {
        // TODO handle failure
        qCDebug(MESSAGECOMPOSER_LOG) << "determineWhetherToEncrypt: failed to resolve keys! oh noes";
        if (!canceled) {
            Q_EMIT failed(i18n("Failed to resolve keys. Please report a bug."));
        } else {
            Q_EMIT failed(QString());
        }

        wasCanceled = canceled;
        return QList< MessageComposer::Composer *>();
    }

    //No encryption or signing is needed
    if (!signSomething && !encryptSomething) {
        return QList< MessageComposer::Composer * >() << new MessageComposer::Composer();
    }

    const Kleo::Result kpgpResult = keyResolver->resolveAllKeys(signSomething, encryptSomething);
    if (kpgpResult == Kleo::Canceled) {
        qCDebug(MESSAGECOMPOSER_LOG) << "resolveAllKeys: one key resolution canceled by user";
        return QList< MessageComposer::Composer *>();
    } else if (kpgpResult != Kleo::Ok) {
        // TODO handle failure
        qCDebug(MESSAGECOMPOSER_LOG) << "resolveAllKeys: failed to resolve keys! oh noes";
        Q_EMIT failed(i18n("Failed to resolve keys. Please report a bug."));
        return QList< MessageComposer::Composer *>();
    }
    qCDebug(MESSAGECOMPOSER_LOG) << "done resolving keys:";

    QList< MessageComposer::Composer * > composers;

    if (encryptSomething || signSomething) {
        Kleo::CryptoMessageFormat concreteFormat = Kleo::AutoFormat;
        for (unsigned int i = 0; i < numConcreteCryptoMessageFormats; ++i) {
            concreteFormat = concreteCryptoMessageFormats[i];
            if (keyResolver->encryptionItems(concreteFormat).empty()) {
                continue;
            }

            if (!(concreteFormat & m_cryptoMessageFormat)) {
                continue;
            }

            MessageComposer::Composer *composer =  new MessageComposer::Composer;

            if (encryptSomething) {
                std::vector<Kleo::KeyResolver::SplitInfo> encData = keyResolver->encryptionItems(concreteFormat);
                std::vector<Kleo::KeyResolver::SplitInfo>::iterator it;
                std::vector<Kleo::KeyResolver::SplitInfo>::iterator end(encData.end());
                QList<QPair<QStringList, std::vector<GpgME::Key> > > data;
                data.reserve(encData.size());
                for (it = encData.begin(); it != end; ++it) {
                    QPair<QStringList, std::vector<GpgME::Key> > p(it->recipients, it->keys);
                    data.append(p);
                    qCDebug(MESSAGECOMPOSER_LOG) << "got resolved keys for:" << it->recipients;
                }
                composer->setEncryptionKeys(data);
            }

            if (signSomething) {
                // find signing keys for this format
                std::vector<GpgME::Key> signingKeys = keyResolver->signingKeys(concreteFormat);
                composer->setSigningKeys(signingKeys);
            }

            composer->setMessageCryptoFormat(concreteFormat);
            composer->setSignAndEncrypt(signSomething, encryptSomething);

            composers.append(composer);
        }
    } else {
        MessageComposer::Composer *composer =  new MessageComposer::Composer;
        composers.append(composer);
        //If we canceled sign or encrypt be sure to change status in attachment.
        markAllAttachmentsForSigning(false);
        markAllAttachmentsForEncryption(false);
    }

    if (composers.isEmpty() && (signSomething || encryptSomething)) {
        Q_ASSERT_X(false, "ComposerViewBase::fillCryptoInfo", "No concrete sign or encrypt method selected");
    }

    return composers;
}
#endif
