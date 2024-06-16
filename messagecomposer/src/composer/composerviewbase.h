/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "MessageComposer/Recipient"
#include "messagecomposer/messagesender.h"
#include "messagecomposer_export.h"
#include <Akonadi/Collection>
#include <Akonadi/Item>
#include <KMime/Message>

#include <Libkleo/Enum>
#include <QList>
#include <QObject>
#include <QUrl>

class QTimer;
class KJob;
class QWidget;

class ComposerViewBaseTest;

namespace Sonnet
{
class DictionaryComboBox;
}

namespace Akonadi
{
class CollectionComboBox;
}

namespace MailTransport
{
class TransportComboBox;
}
namespace KIdentityManagementWidgets
{
class IdentityCombo;
}
namespace KIdentityManagementCore
{
class Identity;
class IdentityManager;
}

namespace Kleo
{
class ExpiryChecker;
class KeyResolver;
}

namespace MessageComposer
{
class RecipientsEditor;
class RichTextComposerNg;
class InfoPart;
class GlobalPart;
class Composer;
class AttachmentControllerBase;
class AttachmentModel;
class SignatureController;
class SendLaterInfo;
/**
 * @brief The ComposerViewBase class
 */
class MESSAGECOMPOSER_EXPORT ComposerViewBase : public QObject
{
    Q_OBJECT
public:
    explicit ComposerViewBase(QObject *parent = nullptr, QWidget *widget = nullptr);
    ~ComposerViewBase() override;

    enum Confirmation {
        LetUserConfirm,
        NoConfirmationNeeded,
    };
    enum MissingAttachment {
        NoMissingAttachmentFound,
        FoundMissingAttachmentAndSending,
        FoundMissingAttachmentAndAddedAttachment,
        FoundMissingAttachmentAndCancel,
    };

    enum FailedType {
        Sending,
        AutoSave,
    };

    /**
     * Set the message to be opened in the composer window, and set the internal data structures to
     *  keep track of it.
     */
    void setMessage(const KMime::Message::Ptr &newMsg, bool allowDecryption);

    void updateTemplate(const KMime::Message::Ptr &msg);

    /**
     * Send the message with the specified method, saving it in the specified folder.
     */
    void send(MessageComposer::MessageSender::SendMethod method, MessageComposer::MessageSender::SaveIn saveIn, bool checkMailDispatcher = true);

    /**
     * Returns true if there is at least one composer job running.
     */
    [[nodiscard]] bool isComposing() const;

    /**
     * Add the given attachment to the message.
     */
    void addAttachment(const QUrl &url, const QString &comment, bool sync);
    void addAttachment(const QString &name, const QString &filename, const QString &charset, const QByteArray &data, const QByteArray &mimeType);
    void addAttachmentPart(KMime::Content *part);

    void fillComposer(MessageComposer::Composer *composer);

    /**
     * Header fields in recipients editor.
     */
    [[nodiscard]] QString to() const;
    [[nodiscard]] QString cc() const;
    [[nodiscard]] QString bcc() const;
    [[nodiscard]] QString from() const;
    [[nodiscard]] QString replyTo() const;
    [[nodiscard]] QString subject() const;

    [[nodiscard]] const KIdentityManagementCore::Identity &currentIdentity() const;
    [[nodiscard]] bool autocryptEnabled() const;

    /**
     * The following are for setting the various options and widgets in the
     *  composer.
     */
    void setAttachmentModel(MessageComposer::AttachmentModel *model);
    [[nodiscard]] MessageComposer::AttachmentModel *attachmentModel();

    void setAttachmentController(MessageComposer::AttachmentControllerBase *controller);
    [[nodiscard]] MessageComposer::AttachmentControllerBase *attachmentController();

    void setRecipientsEditor(MessageComposer::RecipientsEditor *recEditor);
    [[nodiscard]] MessageComposer::RecipientsEditor *recipientsEditor();

    void setSignatureController(MessageComposer::SignatureController *sigController);
    [[nodiscard]] MessageComposer::SignatureController *signatureController();

    void setIdentityCombo(KIdentityManagementWidgets::IdentityCombo *identCombo);
    [[nodiscard]] KIdentityManagementWidgets::IdentityCombo *identityCombo();

    void setIdentityManager(KIdentityManagementCore::IdentityManager *identMan);
    [[nodiscard]] KIdentityManagementCore::IdentityManager *identityManager();

    void setEditor(MessageComposer::RichTextComposerNg *editor);
    [[nodiscard]] MessageComposer::RichTextComposerNg *editor() const;

    void setTransportCombo(MailTransport::TransportComboBox *transpCombo);
    [[nodiscard]] MailTransport::TransportComboBox *transportComboBox() const;

    void setFccCombo(Akonadi::CollectionComboBox *fcc);
    [[nodiscard]] Akonadi::CollectionComboBox *fccCombo() const;
    void setFcc(const Akonadi::Collection &id);

    [[nodiscard]] Sonnet::DictionaryComboBox *dictionary() const;
    void setDictionary(Sonnet::DictionaryComboBox *dictionary);

    /**
     * Widgets for editing differ in client classes, so
     *  values are set before sending.
     */
    void setFrom(const QString &from);
    void setSubject(const QString &subject);

    /**
     * The following are various settings the user can modify when composing a message. If they are not set,
     *  the default values will be used.
     */
    void setCryptoOptions(bool sign, bool encrypt, Kleo::CryptoMessageFormat format, bool neverEncryptDrafts = false);
    void setMDNRequested(bool mdnRequested);
    void setUrgent(bool urgent);

    void setAutoSaveInterval(int interval);
    void setCustomHeader(const QMap<QByteArray, QString> &customHeader);

    /**
     * Enables/disables autosaving depending on the value of the autosave
     * interval.
     */
    void updateAutoSave();

    /**
     * Sets the filename to use when autosaving something. This is used when the client recovers
     * the autosave files: It calls this method, so that the composer uses the same filename again.
     * That way, the recovered autosave file is properly cleaned up in cleanupAutoSave():
     */
    void setAutoSaveFileName(const QString &fileName);

    /**
     * Stop autosaving and delete the autosaved message.
     */
    void cleanupAutoSave();

    void setParentWidgetForGui(QWidget *);

    /**
     * Check if the mail has references to attachments, but no attachments are added to it.
     * If missing attachments are found, a dialog to add new attachments is shown.
     * @param attachmentKeywords a list with the keywords that indicate an attachment should be present
     * @return NoMissingAttachmentFound, if there is attachment in email
     *         FoundMissingAttachmentAndCancelSending, if mail might miss attachment but sending
     *         FoundMissingAttachmentAndAddedAttachment, if mail might miss attachment and we added an attachment
     *         FoundMissingAttachmentAndCancel, if mail might miss attachment and cancel sending
     */
    [[nodiscard]] ComposerViewBase::MissingAttachment checkForMissingAttachments(const QStringList &attachmentKeywords);

    [[nodiscard]] bool hasMissingAttachments(const QStringList &attachmentKeywords);

    void setSendLaterInfo(SendLaterInfo *info);
    [[nodiscard]] SendLaterInfo *sendLaterInfo() const;
    void saveMailSettings();

    [[nodiscard]] QDate followUpDate() const;
    void setFollowUpDate(const QDate &followUpDate);

    void clearFollowUp();

    [[nodiscard]] Akonadi::Collection followUpCollection() const;
    void setFollowUpCollection(const Akonadi::Collection &followUpCollection);

    [[nodiscard]] KMime::Message::Ptr msg() const;

    [[nodiscard]] bool requestDeleveryConfirmation() const;
    void setRequestDeleveryConfirmation(bool requestDeleveryConfirmation);

    [[nodiscard]] std::shared_ptr<Kleo::ExpiryChecker> expiryChecker();

public Q_SLOTS:
    void identityChanged(const KIdentityManagementCore::Identity &ident, const KIdentityManagementCore::Identity &oldIdent, bool msgCleared = false);

    /**
     * Save the message.
     */
    void autoSaveMessage();

Q_SIGNALS:
    /**
     * Message sending completed successfully.
     */
    void sentSuccessfully(Akonadi::Item::Id id);
    /**
     * Message sending failed with given error message.
     */
    void failed(const QString &errorMessage, MessageComposer::ComposerViewBase::FailedType type = Sending);

    /**
     * The composer was modified. This can happen behind the users' back
     *  when, for example, and autosaved message was recovered.
     */
    void modified(bool isModified);

    /**
     * Enabling or disabling HTML in the editor is affected
     *  by various client options, so when that would otherwise happen,
     *  hand it off to the client to enact it for real.
     */
    void disableHtml(MessageComposer::ComposerViewBase::Confirmation);
    void enableHtml();
    void tooManyRecipient(bool);

private:
    void slotEmailAddressResolved(KJob *);
    void slotSendComposeResult(KJob *);
    void slotQueueResult(KJob *job);
    void slotCreateItemResult(KJob *);
    void slotAutoSaveComposeResult(KJob *job);
    void slotFccCollectionCheckResult(KJob *job);
    void slotSaveMessage(KJob *job);

    [[nodiscard]] Akonadi::Collection defaultSpecialTarget() const;
    /**
     * Searches the mime tree, where root is the root node, for embedded images,
     * extracts them froom the body and adds them to the editor.
     */
    void collectImages(KMime::Content *root);
    [[nodiscard]] bool inlineSigningEncryptionSelected() const;
    /**
     * Applies the user changes to the message object of the composer
     * and signs/encrypts the message if activated.
     * Disables the controls of the composer window.
     */
    void readyForSending();

    enum RecipientExpansion {
        UseExpandedRecipients,
        UseUnExpandedRecipients,
    };
    void fillComposer(MessageComposer::Composer *composer, ComposerViewBase::RecipientExpansion expansion, bool autoresize);
    [[nodiscard]] QList<MessageComposer::Composer *> generateCryptoMessages(bool &wasCanceled);
    void fillGlobalPart(MessageComposer::GlobalPart *globalPart);
    void fillInfoPart(MessageComposer::InfoPart *part, RecipientExpansion expansion);
    void queueMessage(const KMime::Message::Ptr &message, MessageComposer::Composer *composer);
    void saveMessage(const KMime::Message::Ptr &message, MessageComposer::MessageSender::SaveIn saveIn);
    void saveRecentAddresses(const KMime::Message::Ptr &ptr);
    void
    updateRecipients(const KIdentityManagementCore::Identity &ident, const KIdentityManagementCore::Identity &oldIdent, MessageComposer::Recipient::Type type);

    void markAllAttachmentsForSigning(bool sign);
    void markAllAttachmentsForEncryption(bool encrypt);
    bool determineWhetherToSign(bool doSignCompletely, Kleo::KeyResolver *keyResolver, bool signSomething, bool &result, bool &canceled);
    bool determineWhetherToEncrypt(bool doEncryptCompletely,
                                   Kleo::KeyResolver *keyResolver,
                                   bool encryptSomething,
                                   bool signSomething,
                                   bool &result,
                                   bool &canceled);

    /**
     * Writes out autosave data to the disk from the KMime::Message message.
     * Also appends the msgNum to the filename as a message can have a number of
     * KMime::Messages
     */
    void writeAutoSaveToDisk(const KMime::Message::Ptr &message);

    /**
     * Returns the autosave interval in milliseconds (as needed for QTimer).
     */
    int autoSaveInterval() const;

    /**
     * Initialize autosaving (timer and filename).
     */
    void initAutoSave();
    void addFollowupReminder(const QString &messageId);
    void addSendLaterItem(const Akonadi::Item &item);

    bool addKeysToContext(const QString &gnupgHome,
                          const QList<QPair<QStringList, std::vector<GpgME::Key>>> &data,
                          const std::map<QByteArray, QString> &autocryptMap);

    void setAkonadiLookupEnabled(bool akonadiLookupEnabled);

    KMime::Message::Ptr m_msg;
    MessageComposer::AttachmentControllerBase *m_attachmentController = nullptr;
    MessageComposer::AttachmentModel *m_attachmentModel = nullptr;
    MessageComposer::SignatureController *m_signatureController = nullptr;
    MessageComposer::RecipientsEditor *m_recipientsEditor = nullptr;
    KIdentityManagementWidgets::IdentityCombo *m_identityCombo = nullptr;
    KIdentityManagementCore::IdentityManager *m_identMan = nullptr;
    MessageComposer::RichTextComposerNg *m_editor = nullptr;
    MailTransport::TransportComboBox *m_transport = nullptr;
    Sonnet::DictionaryComboBox *m_dictionary = nullptr;
    Akonadi::CollectionComboBox *m_fccCombo = nullptr;
    Akonadi::Collection m_fccCollection;
    QWidget *m_parentWidget = nullptr;

    // List of active composer jobs. For example, saving as draft, autosaving and printing
    // all create a composer, which is added to this list as long as it is active.
    // Used mainly to prevent closing the window if a composer is active
    QList<MessageComposer::Composer *> m_composers;

    bool m_sign = false;
    bool m_encrypt = false;
    bool m_neverEncrypt = false;
    bool m_mdnRequested = false;
    bool m_urgent = false;
    bool m_requestDeleveryConfirmation = false;
    bool m_akonadiLookupEnabled = true;
    Kleo::CryptoMessageFormat m_cryptoMessageFormat;
    QString mExpandedFrom;
    QString m_from;
    QString m_subject;
    QStringList mExpandedTo, mExpandedCc, mExpandedBcc, mExpandedReplyTo;
    QMap<QByteArray, QString> m_customHeader;

    int m_pendingQueueJobs = 0;

    QTimer *m_autoSaveTimer = nullptr;
    QString m_autoSaveUUID;
    bool m_autoSaveErrorShown = false; // Stops an error message being shown every time autosave is executed.
    int m_autoSaveInterval;

    MessageComposer::MessageSender::SendMethod mSendMethod;
    MessageComposer::MessageSender::SaveIn mSaveIn;

    std::shared_ptr<Kleo::ExpiryChecker> mExpiryChecker;

    QDate mFollowUpDate;
    Akonadi::Collection mFollowUpCollection;

    std::unique_ptr<SendLaterInfo> mSendLaterInfo;

    friend ComposerViewBaseTest;
};
} // namespace
