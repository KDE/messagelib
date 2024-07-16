/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "composerviewbase.h"

#include "attachment/attachmentcontrollerbase.h"
#include "attachment/attachmentmodel.h"
#include "composer-ng/richtextcomposerng.h"
#include "composer-ng/richtextcomposersignatures.h"
#include "composer.h"
#include "composer/keyresolver.h"
#include "composer/signaturecontroller.h"
#include "draftstatus/draftstatus.h"
#include "imagescaling/imagescalingutils.h"
#include "job/emailaddressresolvejob.h"
#include "part/globalpart.h"
#include "part/infopart.h"
#include "utils/kleo_util.h"
#include "utils/util.h"
#include "utils/util_p.h"
#include <KPIMTextEdit/RichTextComposerControler>
#include <KPIMTextEdit/RichTextComposerImages>

#include "sendlater/sendlatercreatejob.h"
#include "sendlater/sendlaterinfo.h"

#include <PimCommonAkonadi/RecentAddresses>

#include "settings/messagecomposersettings.h"
#include <MessageComposer/RecipientsEditor>

#include <KCursorSaver>
#include <KIdentityManagementCore/Identity>
#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/SimpleObjectTreeSource>
#include <Sonnet/DictionaryComboBox>

#include <MessageCore/AutocryptStorage>
#include <MessageCore/StringUtil>

#include <Akonadi/MessageQueueJob>
#include <MailTransport/TransportComboBox>
#include <MailTransport/TransportManager>

#include <Akonadi/CollectionComboBox>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/MessageFlags>
#include <Akonadi/SpecialMailCollections>

#include <KEmailAddress>
#include <KIdentityManagementCore/IdentityManager>
#include <KIdentityManagementWidgets/IdentityCombo>

#include "messagecomposer_debug.h"

#include <Libkleo/ExpiryChecker>
#include <Libkleo/ExpiryCheckerSettings>

#include <QGpgME/ExportJob>
#include <QGpgME/ImportJob>
#include <QGpgME/Protocol>
#include <gpgme++/context.h>
#include <gpgme++/importresult.h>

#include <KLocalizedString>
#include <KMessageBox>
#include <QSaveFile>

#include "followupreminder/followupremindercreatejob.h"
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTimer>
#include <QUuid>

using namespace MessageComposer;

ComposerViewBase::ComposerViewBase(QObject *parent, QWidget *parentGui)
    : QObject(parent)
    , m_msg(KMime::Message::Ptr(new KMime::Message))
    , m_parentWidget(parentGui)
    , m_cryptoMessageFormat(Kleo::AutoFormat)
    , m_autoSaveInterval(60000) // default of 1 min
{
    initAutoSave();
}

ComposerViewBase::~ComposerViewBase() = default;

bool ComposerViewBase::isComposing() const
{
    return !m_composers.isEmpty();
}

void ComposerViewBase::setMessage(const KMime::Message::Ptr &msg, bool allowDecryption)
{
    if (m_attachmentModel) {
        const auto attachments{m_attachmentModel->attachments()};
        for (const MessageCore::AttachmentPart::Ptr &attachment : attachments) {
            if (!m_attachmentModel->removeAttachment(attachment)) {
                qCWarning(MESSAGECOMPOSER_LOG) << "Attachment not found.";
            }
        }
    }
    m_msg = msg;
    if (m_recipientsEditor) {
        m_recipientsEditor->clear();
        bool resultTooManyRecipients = m_recipientsEditor->setRecipientString(m_msg->to()->mailboxes(), MessageComposer::Recipient::To);
        if (!resultTooManyRecipients) {
            resultTooManyRecipients = m_recipientsEditor->setRecipientString(m_msg->cc()->mailboxes(), MessageComposer::Recipient::Cc);
        }
        if (!resultTooManyRecipients) {
            resultTooManyRecipients = m_recipientsEditor->setRecipientString(m_msg->bcc()->mailboxes(), MessageComposer::Recipient::Bcc);
        }
        if (!resultTooManyRecipients) {
            resultTooManyRecipients = m_recipientsEditor->setRecipientString(m_msg->replyTo()->mailboxes(), MessageComposer::Recipient::ReplyTo);
        }
        m_recipientsEditor->setFocusBottom();

        if (!resultTooManyRecipients) {
            // If we are loading from a draft, load unexpanded aliases as well
            if (auto hrd = m_msg->headerByType("X-KMail-UnExpanded-To")) {
                const QStringList spl = hrd->asUnicodeString().split(QLatin1Char(','));
                for (const QString &addr : spl) {
                    if (m_recipientsEditor->addRecipient(addr, MessageComposer::Recipient::To)) {
                        resultTooManyRecipients = true;
                        qCWarning(MESSAGECOMPOSER_LOG) << "Impossible to add recipient.";
                        break;
                    }
                }
            }
        }
        if (!resultTooManyRecipients) {
            if (auto hrd = m_msg->headerByType("X-KMail-UnExpanded-CC")) {
                const QStringList spl = hrd->asUnicodeString().split(QLatin1Char(','));
                for (const QString &addr : spl) {
                    if (m_recipientsEditor->addRecipient(addr, MessageComposer::Recipient::Cc)) {
                        qCWarning(MESSAGECOMPOSER_LOG) << "Impossible to add recipient.";
                        resultTooManyRecipients = true;
                        break;
                    }
                }
            }
        }
        if (!resultTooManyRecipients) {
            if (auto hrd = m_msg->headerByType("X-KMail-UnExpanded-BCC")) {
                const QStringList spl = hrd->asUnicodeString().split(QLatin1Char(','));
                for (const QString &addr : spl) {
                    if (m_recipientsEditor->addRecipient(addr, MessageComposer::Recipient::Bcc)) {
                        qCWarning(MESSAGECOMPOSER_LOG) << "Impossible to add recipient.";
                        resultTooManyRecipients = true;
                        break;
                    }
                }
            }
        }
        if (!resultTooManyRecipients) {
            if (auto hrd = m_msg->headerByType("X-KMail-UnExpanded-Reply-To")) {
                const QStringList spl = hrd->asUnicodeString().split(QLatin1Char(','));
                for (const QString &addr : spl) {
                    if (m_recipientsEditor->addRecipient(addr, MessageComposer::Recipient::ReplyTo)) {
                        qCWarning(MESSAGECOMPOSER_LOG) << "Impossible to add recipient.";
                        resultTooManyRecipients = true;
                        break;
                    }
                }
            }
        }
        Q_EMIT tooManyRecipient(resultTooManyRecipients);
    }
    // First, we copy the message and then parse it to the object tree parser.
    // The otp gets the message text out of it, in textualContent(), and also decrypts
    // the message if necessary.
    KMime::Content msgContent;
    msgContent.setContent(m_msg->encodedContent());
    msgContent.parse();
    MimeTreeParser::SimpleObjectTreeSource emptySource;
    MimeTreeParser::ObjectTreeParser otp(&emptySource); // All default are ok
    emptySource.setDecryptMessage(allowDecryption);
    otp.parseObjectTree(&msgContent);

    // Load the attachments
    const auto attachmentsOfExtraContents{otp.nodeHelper()->attachmentsOfExtraContents()};
    for (const auto &att : attachmentsOfExtraContents) {
        addAttachmentPart(att);
    }
    const auto attachments{msgContent.attachments()};
    for (const auto &att : attachments) {
        addAttachmentPart(att);
    }

    int transportId = -1;
    if (auto hdr = m_msg->headerByType("X-KMail-Transport")) {
        transportId = hdr->asUnicodeString().toInt();
    }

    if (m_transport) {
        const MailTransport::Transport *transport = MailTransport::TransportManager::self()->transportById(transportId);
        if (transport) {
            if (!m_transport->setCurrentTransport(transport->id())) {
                qCWarning(MESSAGECOMPOSER_LOG) << "Impossible to find transport id" << transport->id();
            }
        }
    }

    // Set the HTML text and collect HTML images
    QString htmlContent = otp.htmlContent();
    if (htmlContent.isEmpty()) {
        m_editor->setPlainText(otp.plainTextContent());
    } else {
        // Bug 372085 <div id="name"> is replaced in qtextedit by <a id="name">... => break url
        htmlContent.replace(QRegularExpression(QStringLiteral("<div\\s*id=\".*\">")), QStringLiteral("<div>"));
        m_editor->setHtml(htmlContent);
        Q_EMIT enableHtml();
        collectImages(m_msg.data());
    }

    if (auto hdr = m_msg->headerByType("X-KMail-CursorPos")) {
        m_editor->setCursorPositionFromStart(hdr->asUnicodeString().toUInt());
    }
}

void ComposerViewBase::updateTemplate(const KMime::Message::Ptr &msg)
{
    // First, we copy the message and then parse it to the object tree parser.
    // The otp gets the message text out of it, in textualContent(), and also decrypts
    // the message if necessary.
    KMime::Content msgContent;
    msgContent.setContent(msg->encodedContent());
    msgContent.parse();
    MimeTreeParser::SimpleObjectTreeSource emptySource;
    MimeTreeParser::ObjectTreeParser otp(&emptySource); // All default are ok
    otp.parseObjectTree(&msgContent);
    // Set the HTML text and collect HTML images
    if (!otp.htmlContent().isEmpty()) {
        m_editor->setHtml(otp.htmlContent());
        Q_EMIT enableHtml();
        collectImages(msg.data());
    } else {
        m_editor->setPlainText(otp.plainTextContent());
    }

    if (auto hdr = msg->headerByType("X-KMail-CursorPos")) {
        m_editor->setCursorPositionFromStart(hdr->asUnicodeString().toInt());
    }
}

void ComposerViewBase::saveMailSettings()
{
    const auto identity = currentIdentity();
    auto header = new KMime::Headers::Generic("X-KMail-Transport");
    header->fromUnicodeString(QString::number(m_transport->currentTransportId()));
    m_msg->setHeader(header);

    header = new KMime::Headers::Generic("X-KMail-Transport-Name");
    header->fromUnicodeString(m_transport->currentText());
    m_msg->setHeader(header);

    header = new KMime::Headers::Generic("X-KMail-Fcc");
    header->fromUnicodeString(QString::number(m_fccCollection.id()));
    m_msg->setHeader(header);

    header = new KMime::Headers::Generic("X-KMail-Identity");
    header->fromUnicodeString(QString::number(identity.uoid()));
    m_msg->setHeader(header);

    header = new KMime::Headers::Generic("X-KMail-Identity-Name");
    header->fromUnicodeString(identity.identityName());
    m_msg->setHeader(header);

    header = new KMime::Headers::Generic("X-KMail-Dictionary");
    header->fromUnicodeString(m_dictionary->currentDictionary());
    m_msg->setHeader(header);

    // Save the quote prefix which is used for this message. Each message can have
    // a different quote prefix, for example depending on the original sender.
    if (m_editor->quotePrefixName().isEmpty()) {
        m_msg->removeHeader("X-KMail-QuotePrefix");
    } else {
        header = new KMime::Headers::Generic("X-KMail-QuotePrefix");
        header->fromUnicodeString(m_editor->quotePrefixName());
        m_msg->setHeader(header);
    }

    if (m_editor->composerControler()->isFormattingUsed()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "HTML mode";
        header = new KMime::Headers::Generic("X-KMail-Markup");
        header->fromUnicodeString(QStringLiteral("true"));
        m_msg->setHeader(header);
    } else {
        m_msg->removeHeader("X-KMail-Markup");
        qCDebug(MESSAGECOMPOSER_LOG) << "Plain text";
    }
}

void ComposerViewBase::clearFollowUp()
{
    mFollowUpDate = QDate();
    mFollowUpCollection = Akonadi::Collection();
}

void ComposerViewBase::send(MessageComposer::MessageSender::SendMethod method, MessageComposer::MessageSender::SaveIn saveIn, bool checkMailDispatcher)
{
    mSendMethod = method;
    mSaveIn = saveIn;

    KCursorSaver saver(Qt::WaitCursor);
    const auto identity = currentIdentity();

    if (identity.attachVcard() && m_attachmentController->attachOwnVcard()) {
        const QString vcardFileName = identity.vCardFile();
        if (!vcardFileName.isEmpty()) {
            m_attachmentController->addAttachmentUrlSync(QUrl::fromLocalFile(vcardFileName));
        }
    }
    saveMailSettings();

    if (m_editor->composerControler()->isFormattingUsed() && inlineSigningEncryptionSelected()) {
        const QString keepBtnText =
            m_encrypt ? m_sign ? i18n("&Keep markup, do not sign/encrypt") : i18n("&Keep markup, do not encrypt") : i18n("&Keep markup, do not sign");
        const QString yesBtnText = m_encrypt ? m_sign ? i18n("Sign/Encrypt (delete markup)") : i18n("Encrypt (delete markup)") : i18n("Sign (delete markup)");
        int ret = KMessageBox::warningTwoActionsCancel(m_parentWidget,
                                                       i18n("<qt><p>Inline signing/encrypting of HTML messages is not possible;</p>"
                                                            "<p>do you want to delete your markup?</p></qt>"),
                                                       i18nc("@title:window", "Sign/Encrypt Message?"),
                                                       KGuiItem(yesBtnText),
                                                       KGuiItem(keepBtnText));
        if (KMessageBox::Cancel == ret) {
            return;
        }
        if (KMessageBox::ButtonCode::SecondaryAction == ret) {
            m_encrypt = false;
            m_sign = false;
        } else {
            Q_EMIT disableHtml(NoConfirmationNeeded);
        }
    }

    if (m_neverEncrypt && saveIn != MessageComposer::MessageSender::SaveInNone) {
        // we can't use the state of the mail itself, to remember the
        // signing and encryption state, so let's add a header instead
        DraftSignatureState(m_msg).setState(m_sign);
        DraftEncryptionState(m_msg).setState(m_encrypt);
        DraftCryptoMessageFormatState(m_msg).setState(m_cryptoMessageFormat);
    } else {
        removeDraftCryptoHeaders(m_msg);
    }

    if (mSendMethod == MessageComposer::MessageSender::SendImmediate && checkMailDispatcher) {
        if (!MessageComposer::Util::sendMailDispatcherIsOnline(m_parentWidget)) {
            qCWarning(MESSAGECOMPOSER_LOG) << "Impossible to set sendmaildispatcher online. Please verify it";
            return;
        }
    }

    readyForSending();
}

void ComposerViewBase::setCustomHeader(const QMap<QByteArray, QString> &customHeader)
{
    m_customHeader = customHeader;
}

void ComposerViewBase::readyForSending()
{
    qCDebug(MESSAGECOMPOSER_LOG) << "Entering readyForSending";
    if (!m_msg) {
        qCDebug(MESSAGECOMPOSER_LOG) << "m_msg == 0!";
        return;
    }

    if (!m_composers.isEmpty()) {
        // This may happen if e.g. the autosave timer calls applyChanges.
        qCDebug(MESSAGECOMPOSER_LOG) << "ready for sending: Called while composer active; ignoring. Number of composer " << m_composers.count();
        return;
    }

    // first, expand all addresses
    auto job = new MessageComposer::EmailAddressResolveJob(this);
    const auto identity = currentIdentity();
    if (!identity.isNull()) {
        job->setDefaultDomainName(identity.defaultDomainName());
    }
    job->setFrom(from());
    job->setTo(m_recipientsEditor->recipientStringList(MessageComposer::Recipient::To));
    job->setCc(m_recipientsEditor->recipientStringList(MessageComposer::Recipient::Cc));
    job->setBcc(m_recipientsEditor->recipientStringList(MessageComposer::Recipient::Bcc));
    job->setReplyTo(m_recipientsEditor->recipientStringList(MessageComposer::Recipient::ReplyTo));

    connect(job, &MessageComposer::EmailAddressResolveJob::result, this, &ComposerViewBase::slotEmailAddressResolved);
    job->start();
}

void ComposerViewBase::slotEmailAddressResolved(KJob *job)
{
    if (job->error()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "An error occurred while resolving the email addresses:" << job->errorString();
        // This error could be caused by a broken search infrastructure, so we ignore it for now
        // to not block sending emails completely.
    }

    bool autoresizeImage = MessageComposer::MessageComposerSettings::self()->autoResizeImageEnabled();

    const MessageComposer::EmailAddressResolveJob *resolveJob = qobject_cast<MessageComposer::EmailAddressResolveJob *>(job);
    if (mSaveIn == MessageComposer::MessageSender::SaveInNone) {
        mExpandedFrom = resolveJob->expandedFrom();
        mExpandedTo = resolveJob->expandedTo();
        mExpandedCc = resolveJob->expandedCc();
        mExpandedBcc = resolveJob->expandedBcc();
        mExpandedReplyTo = resolveJob->expandedReplyTo();
        if (autoresizeImage) {
            const QStringList listEmails = QStringList() << mExpandedFrom << mExpandedTo << mExpandedCc << mExpandedBcc << mExpandedReplyTo;
            MessageComposer::Utils resizeUtils;
            autoresizeImage = resizeUtils.filterRecipients(listEmails);
        }
    } else { // saved to draft, so keep the old values, not very nice.
        mExpandedFrom = from();
        const auto recipients{m_recipientsEditor->recipients()};
        for (const MessageComposer::Recipient::Ptr &r : recipients) {
            switch (r->type()) {
            case MessageComposer::Recipient::To:
                mExpandedTo << r->email();
                break;
            case MessageComposer::Recipient::Cc:
                mExpandedCc << r->email();
                break;
            case MessageComposer::Recipient::Bcc:
                mExpandedBcc << r->email();
                break;
            case MessageComposer::Recipient::ReplyTo:
                mExpandedReplyTo << r->email();
                break;
            case MessageComposer::Recipient::Undefined:
                Q_ASSERT(!"Unknown recipient type!");
                break;
            }
        }
        QStringList unExpandedTo;
        QStringList unExpandedCc;
        QStringList unExpandedBcc;
        QStringList unExpandedReplyTo;
        const auto expandedToLst{resolveJob->expandedTo()};
        for (const QString &exp : expandedToLst) {
            if (!mExpandedTo.contains(exp)) { // this address was expanded, so save it explicitly
                unExpandedTo << exp;
            }
        }
        const auto expandedCcLst{resolveJob->expandedCc()};
        for (const QString &exp : expandedCcLst) {
            if (!mExpandedCc.contains(exp)) {
                unExpandedCc << exp;
            }
        }
        const auto expandedBCcLst{resolveJob->expandedBcc()};
        for (const QString &exp : expandedBCcLst) {
            if (!mExpandedBcc.contains(exp)) { // this address was expanded, so save it explicitly
                unExpandedBcc << exp;
            }
        }
        const auto expandedReplyLst{resolveJob->expandedReplyTo()};
        for (const QString &exp : expandedReplyLst) {
            if (!mExpandedReplyTo.contains(exp)) { // this address was expanded, so save it explicitly
                unExpandedReplyTo << exp;
            }
        }
        auto header = new KMime::Headers::Generic("X-KMail-UnExpanded-To");
        header->from7BitString(unExpandedTo.join(QLatin1StringView(", ")).toLatin1());
        m_msg->setHeader(header);
        header = new KMime::Headers::Generic("X-KMail-UnExpanded-CC");
        header->from7BitString(unExpandedCc.join(QLatin1StringView(", ")).toLatin1());
        m_msg->setHeader(header);
        header = new KMime::Headers::Generic("X-KMail-UnExpanded-BCC");
        header->from7BitString(unExpandedBcc.join(QLatin1StringView(", ")).toLatin1());
        m_msg->setHeader(header);
        header = new KMime::Headers::Generic("X-KMail-UnExpanded-Reply-To");
        header->from7BitString(unExpandedReplyTo.join(QLatin1StringView(", ")).toLatin1());
        m_msg->setHeader(header);
        autoresizeImage = false;
    }

    Q_ASSERT(m_composers.isEmpty()); // composers should be empty. The caller of this function
    // checks for emptiness before calling it
    // so just ensure it actually is empty
    // and document it
    // we first figure out if we need to create multiple messages with different crypto formats
    // if so, we create a composer per format
    // if we aren't signing or encrypting, this just returns a single empty message
    if (m_neverEncrypt && mSaveIn != MessageComposer::MessageSender::SaveInNone && !mSendLaterInfo) {
        auto composer = new MessageComposer::Composer;
        composer->setNoCrypto(true);
        m_composers.append(composer);
    } else {
        bool wasCanceled = false;
        m_composers = generateCryptoMessages(wasCanceled);
        if (wasCanceled) {
            return;
        }
    }

    if (m_composers.isEmpty()) {
        Q_EMIT failed(i18n("It was not possible to create a message composer."));
        return;
    }

    if (autoresizeImage) {
        if (MessageComposer::MessageComposerSettings::self()->askBeforeResizing()) {
            if (m_attachmentModel) {
                MessageComposer::Utils resizeUtils;
                if (resizeUtils.containsImage(m_attachmentModel->attachments())) {
                    const int rc = KMessageBox::warningTwoActions(m_parentWidget,
                                                                  i18n("Do you want to resize images?"),
                                                                  i18nc("@title:window", "Auto Resize Images"),
                                                                  KGuiItem(i18nc("@action:button", "Auto Resize")),
                                                                  KGuiItem(i18nc("@action:button", "Do Not Resize")));
                    if (rc == KMessageBox::ButtonCode::PrimaryAction) {
                        autoresizeImage = true;
                    } else {
                        autoresizeImage = false;
                    }
                } else {
                    autoresizeImage = false;
                }
            }
        }
    }
    // Compose each message and prepare it for queueing, sending, or storing

    // working copy in case composers instantly emit result
    const auto composers = m_composers;
    for (MessageComposer::Composer *composer : composers) {
        fillComposer(composer, UseExpandedRecipients, autoresizeImage);
        connect(composer, &MessageComposer::Composer::result, this, &ComposerViewBase::slotSendComposeResult);
        composer->start();
        qCDebug(MESSAGECOMPOSER_LOG) << "Started a composer for sending!";
    }
}

namespace
{
// helper methods for reading encryption settings

inline Kleo::chrono::days encryptOwnKeyNearExpiryWarningThresholdInDays()
{
    if (!MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return Kleo::chrono::days{-1};
    }
    const int num = MessageComposer::MessageComposerSettings::self()->cryptoWarnOwnEncrKeyNearExpiryThresholdDays();
    return Kleo::chrono::days{qMax(1, num)};
}

inline Kleo::chrono::days encryptKeyNearExpiryWarningThresholdInDays()
{
    if (!MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return Kleo::chrono::days{-1};
    }
    const int num = MessageComposer::MessageComposerSettings::self()->cryptoWarnEncrKeyNearExpiryThresholdDays();
    return Kleo::chrono::days{qMax(1, num)};
}

inline Kleo::chrono::days encryptRootCertNearExpiryWarningThresholdInDays()
{
    if (!MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return Kleo::chrono::days{-1};
    }
    const int num = MessageComposer::MessageComposerSettings::self()->cryptoWarnEncrRootNearExpiryThresholdDays();
    return Kleo::chrono::days{qMax(1, num)};
}

inline Kleo::chrono::days encryptChainCertNearExpiryWarningThresholdInDays()
{
    if (!MessageComposer::MessageComposerSettings::self()->cryptoWarnWhenNearExpire()) {
        return Kleo::chrono::days{-1};
    }
    const int num = MessageComposer::MessageComposerSettings::self()->cryptoWarnEncrChaincertNearExpiryThresholdDays();
    return Kleo::chrono::days{qMax(1, num)};
}

inline bool showKeyApprovalDialog()
{
    return MessageComposer::MessageComposerSettings::self()->cryptoShowKeysForApproval();
}

inline bool cryptoWarningUnsigned(const KIdentityManagementCore::Identity &identity)
{
    if (identity.encryptionOverride()) {
        return identity.warnNotSign();
    }
    return MessageComposer::MessageComposerSettings::self()->cryptoWarningUnsigned();
}

inline bool cryptoWarningUnencrypted(const KIdentityManagementCore::Identity &identity)
{
    if (identity.encryptionOverride()) {
        return identity.warnNotEncrypt();
    }
    return MessageComposer::MessageComposerSettings::self()->cryptoWarningUnencrypted();
}
} // nameless namespace

bool ComposerViewBase::addKeysToContext(const QString &gnupgHome,
                                        const QList<QPair<QStringList, std::vector<GpgME::Key>>> &data,
                                        const std::map<QByteArray, QString> &autocryptMap)
{
    bool needSpecialContext = false;

    for (const auto &p : data) {
        for (const auto &k : p.second) {
            const auto it = autocryptMap.find(k.primaryFingerprint());
            if (it != autocryptMap.end()) {
                needSpecialContext = true;
                break;
            }
        }
        if (needSpecialContext) {
            break;
        }
    }

    if (!needSpecialContext) {
        return false;
    }
    const QGpgME::Protocol *proto(QGpgME::openpgp());

    const auto storage = MessageCore::AutocryptStorage::self();
    QEventLoop loop;
    int runningJobs = 0;
    for (const auto &p : data) {
        for (const auto &k : p.second) {
            const auto it = autocryptMap.find(k.primaryFingerprint());
            if (it == autocryptMap.end()) {
                qCDebug(MESSAGECOMPOSER_LOG) << "Adding " << k.primaryFingerprint() << "via Export/Import";
                auto exportJob = proto->publicKeyExportJob(false);
                connect(exportJob,
                        &QGpgME::ExportJob::result,
                        this,
                        [&gnupgHome, &proto, &runningJobs, &loop, &k](const GpgME::Error &result,
                                                                      const QByteArray &keyData,
                                                                      const QString &auditLogAsHtml,
                                                                      const GpgME::Error &auditLogError) {
                            Q_UNUSED(auditLogAsHtml);
                            Q_UNUSED(auditLogError);
                            if (result) {
                                qCWarning(MESSAGECOMPOSER_LOG) << "Failed to export " << k.primaryFingerprint() << result.asString();
                                --runningJobs;
                                if (runningJobs < 1) {
                                    loop.quit();
                                }
                            }

                            auto importJob = proto->importJob();
                            QGpgME::Job::context(importJob)->setEngineHomeDirectory(gnupgHome.toUtf8().constData());
                            importJob->exec(keyData);
                            importJob->deleteLater();
                            --runningJobs;
                            if (runningJobs < 1) {
                                loop.quit();
                            }
                        });
                QStringList patterns;
                patterns << QString::fromUtf8(k.primaryFingerprint());
                runningJobs++;
                exportJob->start(patterns);
                exportJob->setExportFlags(GpgME::Context::ExportMinimal);
            } else {
                qCDebug(MESSAGECOMPOSER_LOG) << "Adding " << k.primaryFingerprint() << "from Autocrypt storage";
                const auto recipient = storage->getRecipient(it->second.toUtf8());
                auto key = recipient->gpgKey();
                auto keydata = recipient->gpgKeydata();
                if (QByteArray(key.primaryFingerprint()) != QByteArray(k.primaryFingerprint())) {
                    qCDebug(MESSAGECOMPOSER_LOG) << "Using gossipkey";
                    keydata = recipient->gossipKeydata();
                }
                auto importJob = proto->importJob();
                QGpgME::Job::context(importJob)->setEngineHomeDirectory(gnupgHome.toUtf8().constData());
                const auto result = importJob->exec(keydata);
                importJob->deleteLater();
            }
        }
    }
    loop.exec();
    return true;
}

void ComposerViewBase::setAkonadiLookupEnabled(bool akonadiLookupEnabled)
{
    m_akonadiLookupEnabled = akonadiLookupEnabled;
}

QList<MessageComposer::Composer *> ComposerViewBase::generateCryptoMessages(bool &wasCanceled)
{
    const auto id = currentIdentity();

    bool canceled = false;

    qCDebug(MESSAGECOMPOSER_LOG) << "filling crypto info";
    connect(expiryChecker().get(),
            &Kleo::ExpiryChecker::expiryMessage,
            this,
            [&canceled](const GpgME::Key &key, QString msg, Kleo::ExpiryChecker::ExpiryInformation info, bool isNewMessage) {
                if (!isNewMessage) {
                    return;
                }

                if (canceled) {
                    return;
                }
                QString title;
                QString dontAskAgainName;
                if (info == Kleo::ExpiryChecker::OwnKeyExpired || info == Kleo::ExpiryChecker::OwnKeyNearExpiry) {
                    dontAskAgainName = QStringLiteral("own key expires soon warning");
                } else {
                    dontAskAgainName = QStringLiteral("other encryption key near expiry warning");
                }
                if (info == Kleo::ExpiryChecker::OwnKeyExpired || info == Kleo::ExpiryChecker::OtherKeyExpired) {
                    title = key.protocol() == GpgME::OpenPGP ? i18n("OpenPGP Key Expired") : i18n("S/MIME Certificate Expired");
                } else {
                    title = key.protocol() == GpgME::OpenPGP ? i18n("OpenPGP Key Expires Soon") : i18n("S/MIME Certificate Expires Soon");
                }
                if (KMessageBox::warningContinueCancel(nullptr, msg, title, KStandardGuiItem::cont(), KStandardGuiItem::cancel(), dontAskAgainName)
                    == KMessageBox::Cancel) {
                    canceled = true;
                }
            });

    QScopedPointer<Kleo::KeyResolver> keyResolver(
        new Kleo::KeyResolver(true, showKeyApprovalDialog(), id.pgpAutoEncrypt(), m_cryptoMessageFormat, expiryChecker()));

    keyResolver->setAutocryptEnabled(autocryptEnabled());
    keyResolver->setAkonadiLookupEnabled(m_akonadiLookupEnabled);

    QStringList encryptToSelfKeys;
    QStringList signKeys;

    bool signSomething = m_sign;
    bool doSignCompletely = m_sign;
    bool encryptSomething = m_encrypt;
    bool doEncryptCompletely = m_encrypt;

    // Add encryptionkeys from id to keyResolver
    if (!id.pgpEncryptionKey().isEmpty()) {
        encryptToSelfKeys.push_back(QLatin1StringView(id.pgpEncryptionKey()));
    }
    if (!id.smimeEncryptionKey().isEmpty()) {
        encryptToSelfKeys.push_back(QLatin1StringView(id.smimeEncryptionKey()));
    }
    if (canceled || keyResolver->setEncryptToSelfKeys(encryptToSelfKeys) != Kleo::Ok) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Failed to set encryptoToSelf keys!";
        return {};
    }

    // Add signingkeys from id to keyResolver
    if (!id.pgpSigningKey().isEmpty()) {
        signKeys.push_back(QLatin1StringView(id.pgpSigningKey()));
    }
    if (!id.smimeSigningKey().isEmpty()) {
        signKeys.push_back(QLatin1StringView(id.smimeSigningKey()));
    }
    if (canceled || keyResolver->setSigningKeys(signKeys) != Kleo::Ok) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Failed to set signing keys!";
        return {};
    }

    if (m_attachmentModel) {
        const auto attachments = m_attachmentModel->attachments();
        for (const MessageCore::AttachmentPart::Ptr &attachment : attachments) {
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

    const QStringList recipients = mExpandedTo + mExpandedCc;
    const QStringList bcc(mExpandedBcc);

    keyResolver->setPrimaryRecipients(recipients);
    keyResolver->setSecondaryRecipients(bcc);

    bool result = true;
    canceled = false;
    signSomething = determineWhetherToSign(doSignCompletely, keyResolver.data(), signSomething, result, canceled);
    if (!result) {
        // TODO handle failure
        qCDebug(MESSAGECOMPOSER_LOG) << "determineWhetherToSign: failed to resolve keys! oh noes";
        if (!canceled) {
            Q_EMIT failed(i18n("Failed to resolve keys. Please report a bug."));
        } else {
            Q_EMIT failed(QString());
        }
        wasCanceled = canceled;
        return {};
    }

    canceled = false;
    encryptSomething = determineWhetherToEncrypt(doEncryptCompletely, keyResolver.data(), encryptSomething, signSomething, result, canceled);
    if (!result) {
        // TODO handle failure
        qCDebug(MESSAGECOMPOSER_LOG) << "determineWhetherToEncrypt: failed to resolve keys! oh noes";
        if (!canceled) {
            Q_EMIT failed(i18n("Failed to resolve keys. Please report a bug."));
        } else {
            Q_EMIT failed(QString());
        }

        wasCanceled = canceled;
        return {};
    }

    QList<MessageComposer::Composer *> composers;

    // No encryption or signing is needed
    if (!signSomething && !encryptSomething) {
        auto composer = new MessageComposer::Composer;
        if (m_cryptoMessageFormat & Kleo::OpenPGPMIMEFormat) {
            composer->setAutocryptEnabled(autocryptEnabled());
            if (keyResolver->encryptToSelfKeysFor(Kleo::OpenPGPMIMEFormat).size() > 0) {
                composer->setSenderEncryptionKey(keyResolver->encryptToSelfKeysFor(Kleo::OpenPGPMIMEFormat)[0]);
            }
        }
        composers.append(composer);
        return composers;
    }

    canceled = false;
    const Kleo::Result kpgpResult = keyResolver->resolveAllKeys(signSomething, encryptSomething);
    if (kpgpResult == Kleo::Canceled || canceled) {
        qCDebug(MESSAGECOMPOSER_LOG) << "resolveAllKeys: one key resolution canceled by user";
        return {};
    } else if (kpgpResult != Kleo::Ok) {
        // TODO handle failure
        qCDebug(MESSAGECOMPOSER_LOG) << "resolveAllKeys: failed to resolve keys! oh noes";
        Q_EMIT failed(i18n("Failed to resolve keys. Please report a bug."));
        return {};
    }

    qCDebug(MESSAGECOMPOSER_LOG) << "done resolving keys.";

    if (encryptSomething || signSomething) {
        Kleo::CryptoMessageFormat concreteFormat = Kleo::AutoFormat;
        for (unsigned int i = 0; i < numConcreteCryptoMessageFormats; ++i) {
            concreteFormat = concreteCryptoMessageFormats[i];
            const auto encData = keyResolver->encryptionItems(concreteFormat);
            if (encData.empty()) {
                continue;
            }

            if (!(concreteFormat & m_cryptoMessageFormat)) {
                continue;
            }

            auto composer = new MessageComposer::Composer;

            if (encryptSomething || autocryptEnabled()) {
                auto end(encData.end());
                QList<QPair<QStringList, std::vector<GpgME::Key>>> data;
                data.reserve(encData.size());
                for (auto it = encData.begin(); it != end; ++it) {
                    QPair<QStringList, std::vector<GpgME::Key>> p(it->recipients, it->keys);
                    data.append(p);
                    qCDebug(MESSAGECOMPOSER_LOG) << "got resolved keys for:" << it->recipients;
                }
                composer->setEncryptionKeys(data);
                if (concreteFormat & Kleo::OpenPGPMIMEFormat && autocryptEnabled()) {
                    composer->setAutocryptEnabled(autocryptEnabled());
                    composer->setSenderEncryptionKey(keyResolver->encryptToSelfKeysFor(concreteFormat)[0]);
                    QTemporaryDir dir;
                    bool specialGnupgHome = addKeysToContext(dir.path(), data, keyResolver->useAutocrypt());
                    if (specialGnupgHome) {
                        dir.setAutoRemove(false);
                        composer->setGnupgHome(dir.path());
                    }
                }
            }

            if (signSomething) {
                // find signing keys for this format
                std::vector<GpgME::Key> signingKeys = keyResolver->signingKeys(concreteFormat);
                composer->setSigningKeys(signingKeys);
            }

            composer->setCryptoMessageFormat(concreteFormat);
            composer->setSignAndEncrypt(signSomething, encryptSomething);

            composers.append(composer);
        }
    } else {
        auto composer = new MessageComposer::Composer;
        composers.append(composer);
        // If we canceled sign or encrypt be sure to change status in attachment.
        markAllAttachmentsForSigning(false);
        markAllAttachmentsForEncryption(false);
    }

    if (composers.isEmpty() && (signSomething || encryptSomething)) {
        Q_ASSERT_X(false, "ComposerViewBase::generateCryptoMessages", "No concrete sign or encrypt method selected");
    }

    return composers;
}

void ComposerViewBase::fillGlobalPart(MessageComposer::GlobalPart *globalPart)
{
    globalPart->setParentWidgetForGui(m_parentWidget);
    globalPart->setMDNRequested(m_mdnRequested);
    globalPart->setRequestDeleveryConfirmation(m_requestDeleveryConfirmation);
}

void ComposerViewBase::fillInfoPart(MessageComposer::InfoPart *infoPart, ComposerViewBase::RecipientExpansion expansion)
{
    // TODO splitAddressList and expandAliases ugliness should be handled by a
    // special AddressListEdit widget... (later: see RecipientsEditor)

    if (m_fccCombo) {
        infoPart->setFcc(QString::number(m_fccCombo->currentCollection().id()));
    } else {
        if (m_fccCollection.isValid()) {
            infoPart->setFcc(QString::number(m_fccCollection.id()));
        }
    }

    infoPart->setTransportId(m_transport->currentTransportId());
    if (expansion == UseExpandedRecipients) {
        infoPart->setFrom(mExpandedFrom);
        infoPart->setTo(mExpandedTo);
        infoPart->setCc(mExpandedCc);
        infoPart->setBcc(mExpandedBcc);
        infoPart->setReplyTo(mExpandedReplyTo);
    } else {
        infoPart->setFrom(from());
        infoPart->setTo(m_recipientsEditor->recipientStringList(MessageComposer::Recipient::To));
        infoPart->setCc(m_recipientsEditor->recipientStringList(MessageComposer::Recipient::Cc));
        infoPart->setBcc(m_recipientsEditor->recipientStringList(MessageComposer::Recipient::Bcc));
        infoPart->setReplyTo(m_recipientsEditor->recipientStringList(MessageComposer::Recipient::ReplyTo));
    }
    infoPart->setSubject(subject());
    infoPart->setUserAgent(QStringLiteral("KMail"));
    infoPart->setUrgent(m_urgent);

    if (auto inReplyTo = m_msg->inReplyTo(false)) {
        infoPart->setInReplyTo(inReplyTo->asUnicodeString());
    }

    if (auto references = m_msg->references(false)) {
        infoPart->setReferences(references->asUnicodeString());
    }

    KMime::Headers::Base::List extras;
    if (auto hdr = m_msg->headerByType("X-KMail-SignatureActionEnabled")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-EncryptActionEnabled")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-CryptoMessageFormat")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-UnExpanded-To")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-UnExpanded-CC")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-UnExpanded-BCC")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-UnExpanded-Reply-To")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->organization(false)) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-Identity")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-Transport")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-Fcc")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-Drafts")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-Templates")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-Link-Message")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-Link-Type")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-Face")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("Face")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-FccDisabled")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-Identity-Name")) {
        extras << hdr;
    }
    if (auto hdr = m_msg->headerByType("X-KMail-Transport-Name")) {
        extras << hdr;
    }

    infoPart->setExtraHeaders(extras);
}

void ComposerViewBase::slotSendComposeResult(KJob *job)
{
    Q_ASSERT(dynamic_cast<MessageComposer::Composer *>(job));
    auto composer = static_cast<MessageComposer::Composer *>(job);
    if (composer->error() != MessageComposer::Composer::NoError) {
        qCDebug(MESSAGECOMPOSER_LOG) << "compose job might have error: " << job->error() << " errorString: " << job->errorString();
    }

    if (composer->error() == MessageComposer::Composer::NoError) {
        Q_ASSERT(m_composers.contains(composer));
        // The messages were composed successfully.
        qCDebug(MESSAGECOMPOSER_LOG) << "NoError.";
        const int numberOfMessage(composer->resultMessages().size());
        for (int i = 0; i < numberOfMessage; ++i) {
            if (mSaveIn == MessageComposer::MessageSender::SaveInNone) {
                queueMessage(composer->resultMessages().at(i), composer);
            } else {
                saveMessage(composer->resultMessages().at(i), mSaveIn);
            }
        }
        saveRecentAddresses(composer->resultMessages().at(0));
    } else if (composer->error() == MessageComposer::Composer::UserCancelledError) {
        // The job warned the user about something, and the user chose to return
        // to the message.  Nothing to do.
        qCDebug(MESSAGECOMPOSER_LOG) << "UserCancelledError.";
        Q_EMIT failed(i18n("Job cancelled by the user"));
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "other Error." << composer->error();
        QString msg;
        if (composer->error() == MessageComposer::Composer::BugError) {
            msg = i18n("Could not compose message: %1 \n Please report this bug.", job->errorString());
        } else {
            msg = i18n("Could not compose message: %1", job->errorString());
        }
        Q_EMIT failed(msg);
    }

    if (!composer->gnupgHome().isEmpty()) {
        QDir dir(composer->gnupgHome());
        dir.removeRecursively();
    }

    m_composers.removeAll(composer);
}

void ComposerViewBase::saveRecentAddresses(const KMime::Message::Ptr &msg)
{
    KConfig *config = MessageComposer::MessageComposerSettings::self()->config();
    if (auto to = msg->to(false)) {
        const auto toAddresses = to->mailboxes();
        for (const auto &address : toAddresses) {
            PimCommon::RecentAddresses::self(config)->add(address.prettyAddress());
        }
    }
    if (auto cc = msg->cc(false)) {
        const auto ccAddresses = cc->mailboxes();
        for (const auto &address : ccAddresses) {
            PimCommon::RecentAddresses::self(config)->add(address.prettyAddress());
        }
    }
    if (auto bcc = msg->bcc(false)) {
        const auto bccAddresses = bcc->mailboxes();
        for (const auto &address : bccAddresses) {
            PimCommon::RecentAddresses::self(config)->add(address.prettyAddress());
        }
    }
}

void ComposerViewBase::queueMessage(const KMime::Message::Ptr &message, MessageComposer::Composer *composer)
{
    const MessageComposer::InfoPart *infoPart = composer->infoPart();
    auto qjob = new Akonadi::MessageQueueJob(this);
    qjob->setMessage(message);
    qjob->transportAttribute().setTransportId(infoPart->transportId());
    if (mSendMethod == MessageComposer::MessageSender::SendLater) {
        qjob->dispatchModeAttribute().setDispatchMode(Akonadi::DispatchModeAttribute::Manual);
    }

    if (message->hasHeader("X-KMail-FccDisabled")) {
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::Delete);
    } else if (!infoPart->fcc().isEmpty()) {
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToCollection);

        const Akonadi::Collection sentCollection(infoPart->fcc().toLongLong());
        qjob->sentBehaviourAttribute().setMoveToCollection(sentCollection);
    } else {
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToDefaultSentCollection);
    }

    MailTransport::Transport *transport = MailTransport::TransportManager::self()->transportById(infoPart->transportId());
    if (transport && transport->specifySenderOverwriteAddress()) {
        qjob->addressAttribute().setFrom(
            KEmailAddress::extractEmailAddress(KEmailAddress::normalizeAddressesAndEncodeIdn(transport->senderOverwriteAddress())));
    } else {
        qjob->addressAttribute().setFrom(KEmailAddress::extractEmailAddress(KEmailAddress::normalizeAddressesAndEncodeIdn(infoPart->from())));
    }
    // if this header is not empty, it contains the real recipient of the message, either the primary or one of the
    //  secondary recipients. so we set that to the transport job, while leaving the message itself alone.
    if (KMime::Headers::Base *realTo = message->headerByType("X-KMail-EncBccRecipients")) {
        qjob->addressAttribute().setTo(MessageComposer::Util::cleanUpEmailListAndEncoding(realTo->asUnicodeString().split(QLatin1Char('%'))));
        message->removeHeader("X-KMail-EncBccRecipients");
        message->assemble();
        qCDebug(MESSAGECOMPOSER_LOG) << "sending with-bcc encr mail to a/n recipient:" << qjob->addressAttribute().to();
    } else {
        qjob->addressAttribute().setTo(MessageComposer::Util::cleanUpEmailListAndEncoding(infoPart->to()));
        qjob->addressAttribute().setCc(MessageComposer::Util::cleanUpEmailListAndEncoding(infoPart->cc()));
        qjob->addressAttribute().setBcc(MessageComposer::Util::cleanUpEmailListAndEncoding(infoPart->bcc()));
    }
    if (m_requestDeleveryConfirmation) {
        qjob->addressAttribute().setDeliveryStatusNotification(true);
    }
    MessageComposer::Util::addSendReplyForwardAction(message, qjob);
    MessageCore::StringUtil::removePrivateHeaderFields(message, false);

    MessageComposer::Util::addCustomHeaders(message, m_customHeader);
    message->assemble();
    connect(qjob, &Akonadi::MessageQueueJob::result, this, &ComposerViewBase::slotQueueResult);
    m_pendingQueueJobs++;
    qjob->start();

    qCDebug(MESSAGECOMPOSER_LOG) << "Queued a message.";
}

void ComposerViewBase::slotQueueResult(KJob *job)
{
    m_pendingQueueJobs--;
    auto qjob = static_cast<Akonadi::MessageQueueJob *>(job);
    qCDebug(MESSAGECOMPOSER_LOG) << "mPendingQueueJobs" << m_pendingQueueJobs;
    Q_ASSERT(m_pendingQueueJobs >= 0);

    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Failed to queue a message:" << job->errorString();
        // There is not much we can do now, since all the MessageQueueJobs have been
        // started.  So just wait for them to finish.
        // TODO show a message box or something
        const QString msg = i18n("There were problems trying to queue the message for sending: %1", job->errorString());

        if (m_pendingQueueJobs == 0) {
            Q_EMIT failed(msg);
            return;
        }
    }

    if (m_pendingQueueJobs == 0) {
        addFollowupReminder(qjob->message()->messageID(false)->asUnicodeString());
        Q_EMIT sentSuccessfully(-1);
    }
}

void ComposerViewBase::initAutoSave()
{
    qCDebug(MESSAGECOMPOSER_LOG) << "initialising autosave";

    // Ensure that the autosave directory exists.
    QDir dataDirectory(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1StringView("/kmail2/"));
    if (!dataDirectory.exists(QStringLiteral("autosave"))) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Creating autosave directory.";
        dataDirectory.mkdir(QStringLiteral("autosave"));
    }

    // Construct a file name
    if (m_autoSaveUUID.isEmpty()) {
        m_autoSaveUUID = QUuid::createUuid().toString();
    }

    updateAutoSave();
}

Akonadi::Collection ComposerViewBase::followUpCollection() const
{
    return mFollowUpCollection;
}

void ComposerViewBase::setFollowUpCollection(const Akonadi::Collection &followUpCollection)
{
    mFollowUpCollection = followUpCollection;
}

QDate ComposerViewBase::followUpDate() const
{
    return mFollowUpDate;
}

void ComposerViewBase::setFollowUpDate(const QDate &followUpDate)
{
    mFollowUpDate = followUpDate;
}

Sonnet::DictionaryComboBox *ComposerViewBase::dictionary() const
{
    return m_dictionary;
}

void ComposerViewBase::setDictionary(Sonnet::DictionaryComboBox *dictionary)
{
    m_dictionary = dictionary;
}

void ComposerViewBase::updateAutoSave()
{
    if (m_autoSaveInterval == 0) {
        delete m_autoSaveTimer;
        m_autoSaveTimer = nullptr;
    } else {
        if (!m_autoSaveTimer) {
            m_autoSaveTimer = new QTimer(this);
            if (m_parentWidget) {
                connect(m_autoSaveTimer, SIGNAL(timeout()), m_parentWidget, SLOT(autoSaveMessage()));
            } else {
                connect(m_autoSaveTimer, &QTimer::timeout, this, &ComposerViewBase::autoSaveMessage);
            }
        }
        m_autoSaveTimer->start(m_autoSaveInterval);
    }
}

void ComposerViewBase::cleanupAutoSave()
{
    delete m_autoSaveTimer;
    m_autoSaveTimer = nullptr;
    if (!m_autoSaveUUID.isEmpty()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "deleting autosave files" << m_autoSaveUUID;

        // Delete the autosave files
        QDir autoSaveDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1StringView("/kmail2/autosave"));

        // Filter out only this composer window's autosave files
        const QStringList autoSaveFilter{m_autoSaveUUID + QLatin1StringView("*")};
        autoSaveDir.setNameFilters(autoSaveFilter);

        // Return the files to be removed
        const QStringList autoSaveFiles = autoSaveDir.entryList();
        qCDebug(MESSAGECOMPOSER_LOG) << "There are" << autoSaveFiles.count() << "to be deleted.";

        // Delete each file
        for (const QString &file : autoSaveFiles) {
            autoSaveDir.remove(file);
        }
        m_autoSaveUUID.clear();
    }
}

//-----------------------------------------------------------------------------
void ComposerViewBase::autoSaveMessage()
{
    qCDebug(MESSAGECOMPOSER_LOG) << "Autosaving message";

    if (m_autoSaveTimer) {
        m_autoSaveTimer->stop();
    }

    if (!m_composers.isEmpty()) {
        // This may happen if e.g. the autosave timer calls applyChanges.
        qCDebug(MESSAGECOMPOSER_LOG) << "Autosave: Called while composer active; ignoring. Number of composer " << m_composers.count();
        return;
    }

    auto composer = new Composer();
    fillComposer(composer);
    composer->setAutoSave(true);
    composer->setAutocryptEnabled(autocryptEnabled());
    m_composers.append(composer);
    connect(composer, &MessageComposer::Composer::result, this, &ComposerViewBase::slotAutoSaveComposeResult);
    composer->start();
}

void ComposerViewBase::setAutoSaveFileName(const QString &fileName)
{
    m_autoSaveUUID = fileName;

    Q_EMIT modified(true);
}

void ComposerViewBase::slotAutoSaveComposeResult(KJob *job)
{
    using MessageComposer::Composer;

    Q_ASSERT(dynamic_cast<Composer *>(job));
    auto composer = static_cast<Composer *>(job);

    if (composer->error() == Composer::NoError) {
        Q_ASSERT(m_composers.contains(composer));

        // The messages were composed successfully. Only save the first message, there should
        // only be one anyway, since crypto is disabled.
        qCDebug(MESSAGECOMPOSER_LOG) << "NoError.";
        writeAutoSaveToDisk(composer->resultMessages().constFirst());
        Q_ASSERT(composer->resultMessages().size() == 1);

        if (m_autoSaveInterval > 0) {
            updateAutoSave();
        }
    } else if (composer->error() == MessageComposer::Composer::UserCancelledError) {
        // The job warned the user about something, and the user chose to return
        // to the message.  Nothing to do.
        qCDebug(MESSAGECOMPOSER_LOG) << "UserCancelledError.";
        Q_EMIT failed(i18n("Job cancelled by the user"), AutoSave);
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "other Error.";
        Q_EMIT failed(i18n("Could not autosave message: %1", job->errorString()), AutoSave);
    }

    m_composers.removeAll(composer);
}

void ComposerViewBase::writeAutoSaveToDisk(const KMime::Message::Ptr &message)
{
    const QString autosavePath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1StringView("/kmail2/autosave/");
    QDir().mkpath(autosavePath);
    const QString filename = autosavePath + m_autoSaveUUID;
    QSaveFile file(filename);
    QString errorMessage;
    qCDebug(MESSAGECOMPOSER_LOG) << "Writing message to disk as" << filename;

    if (file.open(QIODevice::WriteOnly)) {
        file.setPermissions(QFile::ReadUser | QFile::WriteUser);

        if (file.write(message->encodedContent()) != static_cast<qint64>(message->encodedContent().size())) {
            errorMessage = i18n("Could not write all data to file.");
        } else {
            if (!file.commit()) {
                errorMessage = i18n("Could not finalize the file.");
            }
        }
    } else {
        errorMessage = i18n("Could not open file.");
    }

    if (!errorMessage.isEmpty()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Auto saving failed:" << errorMessage << file.errorString() << " m_autoSaveUUID" << m_autoSaveUUID;
        if (!m_autoSaveErrorShown) {
            KMessageBox::error(m_parentWidget,
                               i18n("Autosaving the message as %1 failed.\n"
                                    "%2\n"
                                    "Reason: %3",
                                    filename,
                                    errorMessage,
                                    file.errorString()),
                               i18nc("@title:window", "Autosaving Message Failed"));

            // Error dialog shown, hide the errors the next time
            m_autoSaveErrorShown = true;
        }
    } else {
        // No error occurred, the next error should be shown again
        m_autoSaveErrorShown = false;
    }
    file.commit();
    message->clear();
}

void ComposerViewBase::saveMessage(const KMime::Message::Ptr &message, MessageComposer::MessageSender::SaveIn saveIn)
{
    Akonadi::Collection target;
    const auto identity = currentIdentity();
    message->date()->setDateTime(QDateTime::currentDateTime());
    if (!identity.isNull()) {
        if (auto header = message->headerByType("X-KMail-Fcc")) {
            const int sentCollectionId = header->asUnicodeString().toInt();
            if (identity.fcc() == QString::number(sentCollectionId)) {
                message->removeHeader("X-KMail-Fcc");
            }
        }
    }
    MessageComposer::Util::addCustomHeaders(message, m_customHeader);

    message->assemble();

    Akonadi::Item item;
    item.setMimeType(QStringLiteral("message/rfc822"));
    item.setPayload(message);
    Akonadi::MessageFlags::copyMessageFlags(*message, item);

    if (!identity.isNull()) { // we have a valid identity
        switch (saveIn) {
        case MessageComposer::MessageSender::SaveInTemplates:
            if (!identity.templates().isEmpty()) { // the user has specified a custom templates collection
                target = Akonadi::Collection(identity.templates().toLongLong());
            }
            break;
        case MessageComposer::MessageSender::SaveInDrafts:
            if (!identity.drafts().isEmpty()) { // the user has specified a custom drafts collection
                target = Akonadi::Collection(identity.drafts().toLongLong());
            }
            break;
        case MessageComposer::MessageSender::SaveInOutbox: // We don't define save outbox in identity
            target = Akonadi::SpecialMailCollections::self()->defaultCollection(Akonadi::SpecialMailCollections::Outbox);
            break;
        case MessageComposer::MessageSender::SaveInNone:
            break;
        }

        auto saveMessageJob = new Akonadi::CollectionFetchJob(target, Akonadi::CollectionFetchJob::Base);
        saveMessageJob->setProperty("Akonadi::Item", QVariant::fromValue(item));
        QObject::connect(saveMessageJob, &Akonadi::CollectionFetchJob::result, this, &ComposerViewBase::slotSaveMessage);
    } else {
        // preinitialize with the default collections
        target = defaultSpecialTarget();
        auto create = new Akonadi::ItemCreateJob(item, target, this);
        connect(create, &Akonadi::ItemCreateJob::result, this, &ComposerViewBase::slotCreateItemResult);
        ++m_pendingQueueJobs;
    }
}

void ComposerViewBase::slotSaveMessage(KJob *job)
{
    Akonadi::Collection target;
    auto item = job->property("Akonadi::Item").value<Akonadi::Item>();
    if (job->error()) {
        target = defaultSpecialTarget();
    } else {
        const Akonadi::CollectionFetchJob *fetchJob = qobject_cast<Akonadi::CollectionFetchJob *>(job);
        if (fetchJob->collections().isEmpty()) {
            target = defaultSpecialTarget();
        } else {
            target = fetchJob->collections().at(0);
        }
    }
    auto create = new Akonadi::ItemCreateJob(item, target, this);
    connect(create, &Akonadi::ItemCreateJob::result, this, &ComposerViewBase::slotCreateItemResult);
    ++m_pendingQueueJobs;
}

Akonadi::Collection ComposerViewBase::defaultSpecialTarget() const
{
    Akonadi::Collection target;
    switch (mSaveIn) {
    case MessageComposer::MessageSender::SaveInNone:
        break;
    case MessageComposer::MessageSender::SaveInDrafts:
        target = Akonadi::SpecialMailCollections::self()->defaultCollection(Akonadi::SpecialMailCollections::Drafts);
        break;
    case MessageComposer::MessageSender::SaveInTemplates:
        target = Akonadi::SpecialMailCollections::self()->defaultCollection(Akonadi::SpecialMailCollections::Templates);
        break;
    case MessageComposer::MessageSender::SaveInOutbox:
        target = Akonadi::SpecialMailCollections::self()->defaultCollection(Akonadi::SpecialMailCollections::Outbox);
        break;
    }

    return target;
}

void ComposerViewBase::slotCreateItemResult(KJob *job)
{
    --m_pendingQueueJobs;
    qCDebug(MESSAGECOMPOSER_LOG) << "mPendingCreateItemJobs" << m_pendingQueueJobs;
    Q_ASSERT(m_pendingQueueJobs >= 0);

    if (job->error()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Failed to save a message:" << job->errorString();
        Q_EMIT failed(i18n("Failed to save the message: %1", job->errorString()));
        return;
    }

    Akonadi::Item::Id id = -1;
    if (mSendLaterInfo) {
        auto createJob = static_cast<Akonadi::ItemCreateJob *>(job);
        const Akonadi::Item item = createJob->item();
        if (item.isValid()) {
            id = item.id();
            addSendLaterItem(item);
        }
    }

    if (m_pendingQueueJobs == 0) {
        Q_EMIT sentSuccessfully(id);
    }
}

void ComposerViewBase::addAttachment(const QUrl &url, const QString &comment, bool sync)
{
    Q_UNUSED(comment)
    qCDebug(MESSAGECOMPOSER_LOG) << "adding attachment with url:" << url;
    if (sync) {
        m_attachmentController->addAttachmentUrlSync(url);
    } else {
        m_attachmentController->addAttachment(url);
    }
}

void ComposerViewBase::addAttachment(const QString &name, const QString &filename, const QString &charset, const QByteArray &data, const QByteArray &mimeType)
{
    MessageCore::AttachmentPart::Ptr attachment = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart());
    if (!data.isEmpty()) {
        attachment->setName(name);
        attachment->setFileName(filename);
        attachment->setData(data);
        attachment->setCharset(charset.toLatin1());
        attachment->setMimeType(mimeType);
        // TODO what about the other fields?

        m_attachmentController->addAttachment(attachment);
    }
}

void ComposerViewBase::addAttachmentPart(KMime::Content *partToAttach)
{
    MessageCore::AttachmentPart::Ptr part(new MessageCore::AttachmentPart);
    if (partToAttach->contentType()->mimeType() == "multipart/digest" || partToAttach->contentType(false)->mimeType() == "message/rfc822") {
        // if it is a digest or a full message, use the encodedContent() of the attachment,
        // which already has the proper headers
        part->setData(partToAttach->encodedContent());
    } else {
        part->setData(partToAttach->decodedContent());
    }
    part->setMimeType(partToAttach->contentType(false)->mimeType());
    if (auto cd = partToAttach->contentDescription(false)) {
        part->setDescription(cd->asUnicodeString());
    }
    if (auto ct = partToAttach->contentType(false)) {
        if (ct->hasParameter("name")) {
            part->setName(ct->parameter("name"));
        }
    }
    if (auto cd = partToAttach->contentDisposition(false)) {
        part->setFileName(cd->filename());
        part->setInline(cd->disposition() == KMime::Headers::CDinline);
    }
    if (part->name().isEmpty() && !part->fileName().isEmpty()) {
        part->setName(part->fileName());
    }
    if (part->fileName().isEmpty() && !part->name().isEmpty()) {
        part->setFileName(part->name());
    }
    m_attachmentController->addAttachment(part);
}

void ComposerViewBase::fillComposer(MessageComposer::Composer *composer)
{
    fillComposer(composer, UseUnExpandedRecipients, false);
}

void ComposerViewBase::fillComposer(MessageComposer::Composer *composer, ComposerViewBase::RecipientExpansion expansion, bool autoresize)
{
    fillGlobalPart(composer->globalPart());
    m_editor->fillComposerTextPart(composer->textPart());
    fillInfoPart(composer->infoPart(), expansion);
    if (m_attachmentModel) {
        composer->addAttachmentParts(m_attachmentModel->attachments(), autoresize);
    }
}

//-----------------------------------------------------------------------------
QString ComposerViewBase::to() const
{
    if (m_recipientsEditor) {
        return MessageComposer::Util::cleanedUpHeaderString(m_recipientsEditor->recipientString(MessageComposer::Recipient::To));
    }
    return {};
}

//-----------------------------------------------------------------------------
QString ComposerViewBase::cc() const
{
    if (m_recipientsEditor) {
        return MessageComposer::Util::cleanedUpHeaderString(m_recipientsEditor->recipientString(MessageComposer::Recipient::Cc));
    }
    return {};
}

//-----------------------------------------------------------------------------
QString ComposerViewBase::bcc() const
{
    if (m_recipientsEditor) {
        return MessageComposer::Util::cleanedUpHeaderString(m_recipientsEditor->recipientString(MessageComposer::Recipient::Bcc));
    }
    return {};
}

QString ComposerViewBase::from() const
{
    return MessageComposer::Util::cleanedUpHeaderString(m_from);
}

QString ComposerViewBase::replyTo() const
{
    if (m_recipientsEditor) {
        return MessageComposer::Util::cleanedUpHeaderString(m_recipientsEditor->recipientString(MessageComposer::Recipient::ReplyTo));
    }
    return {};
}

QString ComposerViewBase::subject() const
{
    return MessageComposer::Util::cleanedUpHeaderString(m_subject);
}

const KIdentityManagementCore::Identity &ComposerViewBase::currentIdentity() const
{
    return m_identMan->identityForUoidOrDefault(m_identityCombo->currentIdentity());
}

bool ComposerViewBase::autocryptEnabled() const
{
    return currentIdentity().autocryptEnabled();
}

void ComposerViewBase::setParentWidgetForGui(QWidget *w)
{
    m_parentWidget = w;
}

void ComposerViewBase::setAttachmentController(MessageComposer::AttachmentControllerBase *controller)
{
    m_attachmentController = controller;
}

MessageComposer::AttachmentControllerBase *ComposerViewBase::attachmentController()
{
    return m_attachmentController;
}

void ComposerViewBase::setAttachmentModel(MessageComposer::AttachmentModel *model)
{
    m_attachmentModel = model;
}

MessageComposer::AttachmentModel *ComposerViewBase::attachmentModel()
{
    return m_attachmentModel;
}

void ComposerViewBase::setRecipientsEditor(MessageComposer::RecipientsEditor *recEditor)
{
    m_recipientsEditor = recEditor;
}

MessageComposer::RecipientsEditor *ComposerViewBase::recipientsEditor()
{
    return m_recipientsEditor;
}

void ComposerViewBase::setSignatureController(MessageComposer::SignatureController *sigController)
{
    m_signatureController = sigController;
}

MessageComposer::SignatureController *ComposerViewBase::signatureController()
{
    return m_signatureController;
}

void ComposerViewBase::setIdentityCombo(KIdentityManagementWidgets::IdentityCombo *identCombo)
{
    m_identityCombo = identCombo;
}

KIdentityManagementWidgets::IdentityCombo *ComposerViewBase::identityCombo()
{
    return m_identityCombo;
}

void ComposerViewBase::updateRecipients(const KIdentityManagementCore::Identity &ident,
                                        const KIdentityManagementCore::Identity &oldIdent,
                                        MessageComposer::Recipient::Type type)
{
    QString oldIdentList;
    QString newIdentList;
    switch (type) {
    case MessageComposer::Recipient::Bcc: {
        oldIdentList = oldIdent.bcc();
        newIdentList = ident.bcc();
        break;
    }
    case MessageComposer::Recipient::Cc: {
        oldIdentList = oldIdent.cc();
        newIdentList = ident.cc();
        break;
    }
    case MessageComposer::Recipient::ReplyTo: {
        oldIdentList = oldIdent.replyToAddr();
        newIdentList = ident.replyToAddr();
        break;
    }
    case MessageComposer::Recipient::To:
    case MessageComposer::Recipient::Undefined:
        return;
    }

    if (oldIdentList != newIdentList) {
        const auto oldRecipients = KMime::Types::Mailbox::listFromUnicodeString(oldIdentList);
        for (const KMime::Types::Mailbox &recipient : oldRecipients) {
            m_recipientsEditor->removeRecipient(recipient.prettyAddress(), type);
        }

        const auto newRecipients = KMime::Types::Mailbox::listFromUnicodeString(newIdentList);
        for (const KMime::Types::Mailbox &recipient : newRecipients) {
            m_recipientsEditor->addRecipient(recipient.prettyAddress(), type);
        }
        m_recipientsEditor->setFocusBottom();
    }
}

void ComposerViewBase::identityChanged(const KIdentityManagementCore::Identity &ident, const KIdentityManagementCore::Identity &oldIdent, bool msgCleared)
{
    updateRecipients(ident, oldIdent, MessageComposer::Recipient::Bcc);
    updateRecipients(ident, oldIdent, MessageComposer::Recipient::Cc);
    updateRecipients(ident, oldIdent, MessageComposer::Recipient::ReplyTo);

    KIdentityManagementCore::Signature oldSig = const_cast<KIdentityManagementCore::Identity &>(oldIdent).signature();
    KIdentityManagementCore::Signature newSig = const_cast<KIdentityManagementCore::Identity &>(ident).signature();
    // replace existing signatures
    const bool replaced = editor()->composerSignature()->replaceSignature(oldSig, newSig);
    // Just append the signature if there was no old signature
    if (!replaced && (msgCleared || oldSig.rawText().isEmpty())) {
        signatureController()->applySignature(newSig);
    }
    const QString vcardFileName = ident.vCardFile();
    attachmentController()->setIdentityHasOwnVcard(!vcardFileName.isEmpty());
    attachmentController()->setAttachOwnVcard(ident.attachVcard());

    m_editor->setAutocorrectionLanguage(ident.autocorrectionLanguage());
}

void ComposerViewBase::setEditor(MessageComposer::RichTextComposerNg *editor)
{
    m_editor = editor;
    m_editor->document()->setModified(false);
}

MessageComposer::RichTextComposerNg *ComposerViewBase::editor() const
{
    return m_editor;
}

void ComposerViewBase::setTransportCombo(MailTransport::TransportComboBox *transpCombo)
{
    m_transport = transpCombo;
}

MailTransport::TransportComboBox *ComposerViewBase::transportComboBox() const
{
    return m_transport;
}

void ComposerViewBase::setIdentityManager(KIdentityManagementCore::IdentityManager *identMan)
{
    m_identMan = identMan;
}

KIdentityManagementCore::IdentityManager *ComposerViewBase::identityManager()
{
    return m_identMan;
}

void ComposerViewBase::setFcc(const Akonadi::Collection &fccCollection)
{
    if (m_fccCombo) {
        m_fccCombo->setDefaultCollection(fccCollection);
    } else {
        m_fccCollection = fccCollection;
    }
    auto const checkFccCollectionJob = new Akonadi::CollectionFetchJob(fccCollection, Akonadi::CollectionFetchJob::Base);
    connect(checkFccCollectionJob, &KJob::result, this, &ComposerViewBase::slotFccCollectionCheckResult);
}

void ComposerViewBase::slotFccCollectionCheckResult(KJob *job)
{
    if (job->error()) {
        qCWarning(MESSAGECOMPOSER_LOG) << " void ComposerViewBase::slotFccCollectionCheckResult(KJob *job) error " << job->errorString();
        const Akonadi::Collection sentMailCol = Akonadi::SpecialMailCollections::self()->defaultCollection(Akonadi::SpecialMailCollections::SentMail);
        if (m_fccCombo) {
            m_fccCombo->setDefaultCollection(sentMailCol);
        } else {
            m_fccCollection = sentMailCol;
        }
    }
}

void ComposerViewBase::setFccCombo(Akonadi::CollectionComboBox *fcc)
{
    m_fccCombo = fcc;
}

Akonadi::CollectionComboBox *ComposerViewBase::fccCombo() const
{
    return m_fccCombo;
}

void ComposerViewBase::setFrom(const QString &from)
{
    m_from = from;
}

void ComposerViewBase::setSubject(const QString &subject)
{
    m_subject = subject;
    if (mSendLaterInfo) {
        mSendLaterInfo->setSubject(m_subject);
        mSendLaterInfo->setTo(to());
    }
}

void ComposerViewBase::setAutoSaveInterval(int interval)
{
    m_autoSaveInterval = interval;
}

void ComposerViewBase::setCryptoOptions(bool sign, bool encrypt, Kleo::CryptoMessageFormat format, bool neverEncryptDrafts)
{
    m_sign = sign;
    m_encrypt = encrypt;
    m_cryptoMessageFormat = format;
    m_neverEncrypt = neverEncryptDrafts;
}

void ComposerViewBase::setMDNRequested(bool mdnRequested)
{
    m_mdnRequested = mdnRequested;
}

void ComposerViewBase::setUrgent(bool urgent)
{
    m_urgent = urgent;
}

int ComposerViewBase::autoSaveInterval() const
{
    return m_autoSaveInterval;
}

//-----------------------------------------------------------------------------
void ComposerViewBase::collectImages(KMime::Content *root)
{
    if (KMime::Content *n = Util::findTypeInMessage(root, "multipart", "alternative")) {
        KMime::Content *parentnode = n->parent();
        if (parentnode && parentnode->contentType()->isMultipart() && parentnode->contentType()->subType() == "related") {
            const auto nodes = parentnode->contents();
            for (auto node : nodes) {
                if (node->contentType()->isImage()) {
                    qCDebug(MESSAGECOMPOSER_LOG) << "found image in multipart/related : " << node->contentType()->name();
                    QImage img;
                    img.loadFromData(node->decodedContent());
                    m_editor->composerControler()->composerImages()->loadImage(
                        img,
                        QString::fromLatin1(QByteArray(QByteArrayLiteral("cid:") + node->contentID()->identifier())),
                        node->contentType()->name());
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
bool ComposerViewBase::inlineSigningEncryptionSelected() const
{
    if (!m_sign && !m_encrypt) {
        return false;
    }
    return m_cryptoMessageFormat == Kleo::InlineOpenPGPFormat;
}

bool ComposerViewBase::hasMissingAttachments(const QStringList &attachmentKeywords)
{
    if (attachmentKeywords.isEmpty()) {
        return false;
    }
    if (m_attachmentModel && m_attachmentModel->rowCount() > 0) {
        return false;
    }

    return MessageComposer::Util::hasMissingAttachments(attachmentKeywords, m_editor->document(), subject());
}

ComposerViewBase::MissingAttachment ComposerViewBase::checkForMissingAttachments(const QStringList &attachmentKeywords)
{
    if (!hasMissingAttachments(attachmentKeywords)) {
        return NoMissingAttachmentFound;
    }
    const int rc = KMessageBox::warningTwoActionsCancel(m_editor,

                                                        i18n("The message you have composed seems to refer to an "
                                                             "attached file but you have not attached anything.\n"
                                                             "Do you want to attach a file to your message?"),
                                                        i18nc("@title:window", "File Attachment Reminder"),
                                                        KGuiItem(i18nc("@action:button", "&Attach File..."), QLatin1StringView("mail-attachment")),
                                                        KGuiItem(i18nc("@action:button", "&Send as Is"), QLatin1StringView("mail-send")));
    if (rc == KMessageBox::Cancel) {
        return FoundMissingAttachmentAndCancel;
    }
    if (rc == KMessageBox::ButtonCode::PrimaryAction) {
        m_attachmentController->showAddAttachmentFileDialog();
        return FoundMissingAttachmentAndAddedAttachment;
    }

    return FoundMissingAttachmentAndSending;
}

void ComposerViewBase::markAllAttachmentsForSigning(bool sign)
{
    if (m_attachmentModel) {
        const auto attachments = m_attachmentModel->attachments();
        for (MessageCore::AttachmentPart::Ptr attachment : attachments) {
            attachment->setSigned(sign);
        }
    }
}

void ComposerViewBase::markAllAttachmentsForEncryption(bool encrypt)
{
    if (m_attachmentModel) {
        const auto attachments = m_attachmentModel->attachments();
        for (MessageCore::AttachmentPart::Ptr attachment : attachments) {
            attachment->setEncrypted(encrypt);
        }
    }
}

bool ComposerViewBase::determineWhetherToSign(bool doSignCompletely, Kleo::KeyResolver *keyResolver, bool signSomething, bool &result, bool &canceled)
{
    bool sign = false;
    switch (keyResolver->checkSigningPreferences(signSomething)) {
    case Kleo::DoIt:
        if (!signSomething) {
            markAllAttachmentsForSigning(true);
            return true;
        }
        sign = true;
        break;
    case Kleo::DontDoIt:
        sign = false;
        break;
    case Kleo::AskOpportunistic:
        assert(0);
    case Kleo::Ask: {
        // the user wants to be asked or has to be asked
        KCursorSaver saver(Qt::WaitCursor);
        const QString msg = i18n(
            "Examination of the recipient's signing preferences "
            "yielded that you be asked whether or not to sign "
            "this message.\n"
            "Sign this message?");
        switch (KMessageBox::warningTwoActionsCancel(m_parentWidget,
                                                     msg,
                                                     i18nc("@title:window", "Sign Message?"),
                                                     KGuiItem(i18nc("to sign", "&Sign")),
                                                     KGuiItem(i18nc("@action:button", "Do &Not Sign")))) {
        case KMessageBox::Cancel:
            result = false;
            canceled = true;
            return false;
        case KMessageBox::ButtonCode::PrimaryAction:
            markAllAttachmentsForSigning(true);
            return true;
        case KMessageBox::ButtonCode::SecondaryAction:
            markAllAttachmentsForSigning(false);
            return false;
        default:
            qCWarning(MESSAGECOMPOSER_LOG) << "Unhandled MessageBox response";
            return false;
        }
        break;
    }
    case Kleo::Conflict: {
        // warn the user that there are conflicting signing preferences
        KCursorSaver saver(Qt::WaitCursor);
        const QString msg = i18n(
            "There are conflicting signing preferences "
            "for these recipients.\n"
            "Sign this message?");
        switch (KMessageBox::warningTwoActionsCancel(m_parentWidget,
                                                     msg,
                                                     i18nc("@title:window", "Sign Message?"),
                                                     KGuiItem(i18nc("to sign", "&Sign")),
                                                     KGuiItem(i18nc("@action:button", "Do &Not Sign")))) {
        case KMessageBox::Cancel:
            result = false;
            canceled = true;
            return false;
        case KMessageBox::ButtonCode::PrimaryAction:
            markAllAttachmentsForSigning(true);
            return true;
        case KMessageBox::ButtonCode::SecondaryAction:
            markAllAttachmentsForSigning(false);
            return false;
        default:
            qCWarning(MESSAGECOMPOSER_LOG) << "Unhandled MessageBox response";
            return false;
        }
        break;
    }
    case Kleo::Impossible: {
        KCursorSaver saver(Qt::WaitCursor);
        const QString msg = i18n(
            "You have requested to sign this message, "
            "but no valid signing keys have been configured "
            "for this identity.");
        if (KMessageBox::warningContinueCancel(m_parentWidget,
                                               msg,
                                               i18nc("@title:window", "Send Unsigned?"),
                                               KGuiItem(i18nc("@action:button", "Send &Unsigned")))
            == KMessageBox::Cancel) {
            result = false;
            return false;
        } else {
            markAllAttachmentsForSigning(false);
            return false;
        }
    }
    }

    if (!sign || !doSignCompletely) {
        if (cryptoWarningUnsigned(currentIdentity())) {
            KCursorSaver saver(Qt::WaitCursor);
            const QString msg = sign && !doSignCompletely ? i18n(
                                    "Some parts of this message will not be signed.\n"
                                    "Sending only partially signed messages might violate site policy.\n"
                                    "Sign all parts instead?") // oh, I hate this...
                                                          : i18n(
                                                              "This message will not be signed.\n"
                                                              "Sending unsigned message might violate site policy.\n"
                                                              "Sign message instead?"); // oh, I hate this...
            const QString buttonText = sign && !doSignCompletely ? i18n("&Sign All Parts") : i18n("&Sign");
            switch (KMessageBox::warningTwoActionsCancel(m_parentWidget,
                                                         msg,
                                                         i18nc("@title:window", "Unsigned-Message Warning"),
                                                         KGuiItem(buttonText),
                                                         KGuiItem(i18nc("@action:button", "Send &As Is")))) {
            case KMessageBox::Cancel:
                result = false;
                canceled = true;
                return false;
            case KMessageBox::ButtonCode::PrimaryAction:
                markAllAttachmentsForSigning(true);
                return true;
            case KMessageBox::ButtonCode::SecondaryAction:
                return sign || doSignCompletely;
            default:
                qCWarning(MESSAGECOMPOSER_LOG) << "Unhandled MessageBox response";
                return false;
            }
        }
    }
    return sign || doSignCompletely;
}

bool ComposerViewBase::determineWhetherToEncrypt(bool doEncryptCompletely,
                                                 Kleo::KeyResolver *keyResolver,
                                                 bool encryptSomething,
                                                 bool signSomething,
                                                 bool &result,
                                                 bool &canceled)
{
    bool encrypt = false;
    bool opportunistic = false;
    switch (keyResolver->checkEncryptionPreferences(encryptSomething)) {
    case Kleo::DoIt:
        if (!encryptSomething) {
            markAllAttachmentsForEncryption(true);
            return true;
        }
        encrypt = true;
        break;
    case Kleo::DontDoIt:
        encrypt = false;
        break;
    case Kleo::AskOpportunistic:
        opportunistic = true;
        // fall through...
        [[fallthrough]];
    case Kleo::Ask: {
        // the user wants to be asked or has to be asked
        KCursorSaver saver(Qt::WaitCursor);
        const QString msg = opportunistic ? i18n(
                                "Valid trusted encryption keys were found for all recipients.\n"
                                "Encrypt this message?")
                                          : i18n(
                                              "Examination of the recipient's encryption preferences "
                                              "yielded that you be asked whether or not to encrypt "
                                              "this message.\n"
                                              "Encrypt this message?");
        switch (KMessageBox::warningTwoActionsCancel(m_parentWidget,
                                                     msg,
                                                     i18n("Encrypt Message?"),
                                                     KGuiItem(signSomething ? i18n("Sign && &Encrypt") : i18n("&Encrypt")),
                                                     KGuiItem(signSomething ? i18n("&Sign Only") : i18n("&Send As-Is")))) {
        case KMessageBox::Cancel:
            result = false;
            canceled = true;
            return false;
        case KMessageBox::ButtonCode::PrimaryAction:
            markAllAttachmentsForEncryption(true);
            return true;
        case KMessageBox::ButtonCode::SecondaryAction:
            markAllAttachmentsForEncryption(false);
            return false;
        default:
            qCWarning(MESSAGECOMPOSER_LOG) << "Unhandled MessageBox response";
            return false;
        }
        break;
    }
    case Kleo::Conflict: {
        // warn the user that there are conflicting encryption preferences
        KCursorSaver saver(Qt::WaitCursor);
        const QString msg = i18n(
            "There are conflicting encryption preferences "
            "for these recipients.\n"
            "Encrypt this message?");
        switch (KMessageBox::warningTwoActionsCancel(

            m_parentWidget,
            msg,
            i18n("Encrypt Message?"),
            KGuiItem(i18nc("@action:button", "&Encrypt")),
            KGuiItem(i18nc("@action:button", "Do &Not Encrypt")))) {
        case KMessageBox::Cancel:
            result = false;
            canceled = true;
            return false;
        case KMessageBox::ButtonCode::PrimaryAction:
            markAllAttachmentsForEncryption(true);
            return true;
        case KMessageBox::ButtonCode::SecondaryAction:
            markAllAttachmentsForEncryption(false);
            return false;
        default:
            qCWarning(MESSAGECOMPOSER_LOG) << "Unhandled MessageBox response";
            return false;
        }
        break;
    }
    case Kleo::Impossible: {
        KCursorSaver saver(Qt::WaitCursor);
        const QString msg = i18n(
            "You have requested to encrypt this message, "
            "and to encrypt a copy to yourself, "
            "but no valid trusted encryption keys have been "
            "configured for this identity.");
        if (KMessageBox::warningContinueCancel(m_parentWidget,
                                               msg,
                                               i18nc("@title:window", "Send Unencrypted?"),
                                               KGuiItem(i18nc("@action:button", "Send &Unencrypted")))
            == KMessageBox::Cancel) {
            result = false;
            return false;
        } else {
            markAllAttachmentsForEncryption(false);
            return false;
        }
    }
    }

    if (!encrypt || !doEncryptCompletely) {
        if (cryptoWarningUnencrypted(currentIdentity())) {
            KCursorSaver saver(Qt::WaitCursor);
            const QString msg = !doEncryptCompletely ? i18n(
                                    "Some parts of this message will not be encrypted.\n"
                                    "Sending only partially encrypted messages might violate "
                                    "site policy and/or leak sensitive information.\n"
                                    "Encrypt all parts instead?") // oh, I hate this...
                                                     : i18n(
                                                         "This message will not be encrypted.\n"
                                                         "Sending unencrypted messages might violate site policy and/or "
                                                         "leak sensitive information.\n"
                                                         "Encrypt messages instead?"); // oh, I hate this...
            const QString buttonText = !doEncryptCompletely ? i18n("&Encrypt All Parts") : i18n("&Encrypt");
            switch (KMessageBox::warningTwoActionsCancel(m_parentWidget,
                                                         msg,
                                                         i18nc("@title:window", "Unencrypted Message Warning"),
                                                         KGuiItem(buttonText),
                                                         KGuiItem(signSomething ? i18n("&Sign Only") : i18n("&Send As-Is")))) {
            case KMessageBox::Cancel:
                result = false;
                canceled = true;
                return false;
            case KMessageBox::ButtonCode::PrimaryAction:
                markAllAttachmentsForEncryption(true);
                return true;
            case KMessageBox::ButtonCode::SecondaryAction:
                return encrypt || doEncryptCompletely;
            default:
                qCWarning(MESSAGECOMPOSER_LOG) << "Unhandled MessageBox response";
                return false;
            }
        }
    }

    return encrypt || doEncryptCompletely;
}

void ComposerViewBase::setSendLaterInfo(SendLaterInfo *info)
{
    mSendLaterInfo.reset(info);
}

SendLaterInfo *ComposerViewBase::sendLaterInfo() const
{
    return mSendLaterInfo.get();
}

void ComposerViewBase::addFollowupReminder(const QString &messageId)
{
    if (!messageId.isEmpty()) {
        if (mFollowUpDate.isValid()) {
            auto job = new MessageComposer::FollowupReminderCreateJob;
            job->setSubject(m_subject);
            job->setMessageId(messageId);
            job->setTo(mExpandedReplyTo.isEmpty() ? mExpandedTo.join(QLatin1Char(',')) : mExpandedReplyTo.join(QLatin1Char(',')));
            job->setFollowUpReminderDate(mFollowUpDate);
            job->setCollectionToDo(mFollowUpCollection);
            job->start();
        }
    }
}

void ComposerViewBase::addSendLaterItem(const Akonadi::Item &item)
{
    mSendLaterInfo->setItemId(item.id());

    auto job = new MessageComposer::SendLaterCreateJob(*mSendLaterInfo, this);
    job->start();
}

bool ComposerViewBase::requestDeleveryConfirmation() const
{
    return m_requestDeleveryConfirmation;
}

void ComposerViewBase::setRequestDeleveryConfirmation(bool requestDeleveryConfirmation)
{
    m_requestDeleveryConfirmation = requestDeleveryConfirmation;
}

KMime::Message::Ptr ComposerViewBase::msg() const
{
    return m_msg;
}

std::shared_ptr<Kleo::ExpiryChecker> ComposerViewBase::expiryChecker()
{
    if (!mExpiryChecker) {
        mExpiryChecker.reset(new Kleo::ExpiryChecker{Kleo::ExpiryCheckerSettings{encryptOwnKeyNearExpiryWarningThresholdInDays(),
                                                                                 encryptKeyNearExpiryWarningThresholdInDays(),
                                                                                 encryptRootCertNearExpiryWarningThresholdInDays(),
                                                                                 encryptChainCertNearExpiryWarningThresholdInDays()}});
    }
    return mExpiryChecker;
}

#include "moc_composerviewbase.cpp"
