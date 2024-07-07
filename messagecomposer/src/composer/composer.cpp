/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "composer.h"

#include "imagescaling/imagescaling.h"
#include "imagescaling/imagescalingutils.h"
#include "job/attachmentjob.h"
#include "job/autocryptheadersjob.h"
#include "job/encryptjob.h"
#include "job/itipjob.h"
#include "job/jobbase_p.h"
#include "job/maintextjob.h"
#include "job/multipartjob.h"
#include "job/signencryptjob.h"
#include "job/signjob.h"
#include "job/skeletonmessagejob.h"
#include "job/transparentjob.h"
#include "part/globalpart.h"
#include "part/infopart.h"
#include "part/itippart.h"
#include "part/textpart.h"

#include "messagecomposer_debug.h"
#include <KLocalizedString>

using namespace MessageComposer;
using MessageCore::AttachmentPart;

class MessageComposer::ComposerPrivate : public JobBasePrivate
{
public:
    explicit ComposerPrivate(Composer *qq)
        : JobBasePrivate(qq)
    {
    }

    ~ComposerPrivate() override
    {
        delete skeletonMessage;
    }

    void init();
    void doStart(); // slot
    void composeStep1();
    void composeStep2();
    [[nodiscard]] QList<ContentJobBase *> createEncryptJobs(ContentJobBase *contentJob, bool sign);
    void contentJobFinished(KJob *job); // slot
    void composeWithLateAttachments(KMime::Message *headers,
                                    KMime::Content *content,
                                    const AttachmentPart::List &parts,
                                    const std::vector<GpgME::Key> &keys,
                                    const QStringList &recipients);
    void attachmentsFinished(KJob *job); // slot

    void composeFinalStep(KMime::Content *headers, KMime::Content *content);

    QString gnupgHome;
    QList<QPair<QStringList, std::vector<GpgME::Key>>> encData;
    GpgME::Key senderEncryptionKey;
    std::vector<GpgME::Key> signers;
    AttachmentPart::List attachmentParts;
    // attachments with different sign/encrypt settings from
    // main message body. added at the end of the process
    AttachmentPart::List lateAttachmentParts;
    QList<KMime::Message::Ptr> resultMessages;

    Kleo::CryptoMessageFormat format;

    // Stuff that the application plays with.
    GlobalPart *globalPart = nullptr;
    InfoPart *infoPart = nullptr;
    TextPart *textPart = nullptr;
    ItipPart *itipPart = nullptr;

    // Stuff that we play with.
    KMime::Message *skeletonMessage = nullptr;

    bool started = false;
    bool finished = false;
    bool sign = false;
    bool encrypt = false;
    bool noCrypto = false;
    bool autoSaving = false;
    bool autocryptEnabled = false;
    Q_DECLARE_PUBLIC(Composer)
};

void ComposerPrivate::init()
{
    Q_Q(Composer);

    // We cannot create these in ComposerPrivate's constructor, because
    // their parent q is not fully constructed at that time.
    globalPart = new GlobalPart(q);
    infoPart = new InfoPart(q);
    textPart = nullptr;
    itipPart = nullptr;
}

void ComposerPrivate::doStart()
{
    Q_ASSERT(!started);
    started = true;
    composeStep1();
}

void ComposerPrivate::composeStep1()
{
    Q_Q(Composer);

    // Create skeleton message (containing headers only; no content).
    auto skeletonJob = new SkeletonMessageJob(infoPart, globalPart, q);
    QObject::connect(skeletonJob, &SkeletonMessageJob::finished, q, [this, skeletonJob](KJob *job) {
        if (job->error()) {
            return; // KCompositeJob takes care of the error.
        }

        // SkeletonMessageJob is a special job creating a Message instead of a Content.
        Q_ASSERT(skeletonMessage == nullptr);
        skeletonMessage = skeletonJob->message();
        Q_ASSERT(skeletonMessage);
        skeletonMessage->assemble();

        composeStep2();
    });
    q->addSubjob(skeletonJob);
    skeletonJob->start();
}

void ComposerPrivate::composeStep2()
{
    Q_Q(Composer);

    ContentJobBase *mainJob = nullptr;
    ContentJobBase *mainContentJob = nullptr;
    Q_ASSERT(textPart || itipPart); // At least one must be present, otherwise it's a useless message
    if (textPart && !itipPart) {
        mainContentJob = new MainTextJob(textPart, q);
    } else if (!textPart && itipPart) {
        mainContentJob = new ItipJob(itipPart, q);
    } else {
        // Combination of both text and itip parts not supported right now
        Q_ASSERT(!textPart || !itipPart);
    }

    if ((sign || encrypt) && format & Kleo::InlineOpenPGPFormat) { // needs custom handling --- one SignEncryptJob by itself
        qCDebug(MESSAGECOMPOSER_LOG) << "sending to sign/enc inline job!";

        if (encrypt) {
            // TODO: fix Inline PGP with encrypted attachments

            const QList<ContentJobBase *> jobs = createEncryptJobs(mainContentJob, sign);
            for (ContentJobBase *subJob : jobs) {
                if (attachmentParts.isEmpty()) {
                    // We have no attachments.  Use the content given by the mainContentJob
                    mainJob = subJob;
                } else {
                    auto multipartJob = new MultipartJob(q);
                    multipartJob->setMultipartSubtype("mixed");
                    multipartJob->appendSubjob(subJob);
                    for (const AttachmentPart::Ptr &part : std::as_const(attachmentParts)) {
                        multipartJob->appendSubjob(new AttachmentJob(part));
                    }
                    mainJob = multipartJob;
                }

                QObject::connect(mainJob, SIGNAL(finished(KJob *)), q, SLOT(contentJobFinished(KJob *)));
                q->addSubjob(mainJob);
            }
        } else {
            auto subJob = new SignJob(q);
            subJob->setSigningKeys(signers);
            subJob->setCryptoMessageFormat(format);
            subJob->appendSubjob(mainContentJob);

            if (attachmentParts.isEmpty()) {
                // We have no attachments.  Use the content given by the mainContentJob.
                mainJob = subJob;
            } else {
                auto multipartJob = new MultipartJob(q);
                multipartJob->setMultipartSubtype("mixed");
                multipartJob->appendSubjob(subJob);
                for (const AttachmentPart::Ptr &part : std::as_const(attachmentParts)) {
                    multipartJob->appendSubjob(new AttachmentJob(part));
                }
                mainJob = multipartJob;
            }
            QObject::connect(mainJob, SIGNAL(finished(KJob *)), q, SLOT(contentJobFinished(KJob *)));
            q->addSubjob(mainJob);
        }

        if (mainJob) {
            mainJob->start();
        } else {
            qCDebug(MESSAGECOMPOSER_LOG) << "main job is null";
        }
        return;
    }

    if (attachmentParts.isEmpty()) {
        // We have no attachments.  Use the content given by the mainContentJob
        mainJob = mainContentJob;
    } else {
        // We have attachments.  Create a multipart/mixed content.
        QMutableListIterator<AttachmentPart::Ptr> iter(attachmentParts);
        while (iter.hasNext()) {
            AttachmentPart::Ptr part = iter.next();
            qCDebug(MESSAGECOMPOSER_LOG) << "Checking attachment crypto policy... signed: " << part->isSigned() << " isEncrypted : " << part->isEncrypted();
            if (!noCrypto && !autoSaving && (sign != part->isSigned() || encrypt != part->isEncrypted())) { // different policy
                qCDebug(MESSAGECOMPOSER_LOG) << "got attachment with different crypto policy!";
                lateAttachmentParts.append(part);
                iter.remove();
            }
        }
        auto multipartJob = new MultipartJob(q);
        multipartJob->setMultipartSubtype("mixed");
        multipartJob->appendSubjob(mainContentJob);
        for (const AttachmentPart::Ptr &part : std::as_const(attachmentParts)) {
            multipartJob->appendSubjob(new AttachmentJob(part));
        }
        mainJob = multipartJob;
    }

    if (autocryptEnabled) {
        auto autocryptJob = new AutocryptHeadersJob();
        autocryptJob->setSkeletonMessage(skeletonMessage);
        autocryptJob->setGnupgHome(gnupgHome);
        autocryptJob->appendSubjob(mainJob);
        autocryptJob->setSenderKey(senderEncryptionKey);
        if (encrypt && format & Kleo::OpenPGPMIMEFormat) {
            qDebug() << "Add gossip: " << encData[0].first.size() << encData[0].second.size();
            if (encData[0].first.size() > 1 && encData[0].second.size() > 2) {
                autocryptJob->setGossipKeys(encData[0].second);
            }
        }
        mainJob = autocryptJob;
    }

    if (sign) {
        auto sJob = new SignJob(q);
        sJob->setCryptoMessageFormat(format);
        sJob->setSigningKeys(signers);
        sJob->appendSubjob(mainJob);
        sJob->setSkeletonMessage(skeletonMessage);
        mainJob = sJob;
    }

    if (encrypt) {
        const auto lstJob = createEncryptJobs(mainJob, false);
        for (ContentJobBase *job : lstJob) {
            auto eJob = dynamic_cast<EncryptJob *>(job);
            if (eJob && sign) {
                // When doing Encrypt and Sign move headers only in the signed part
                eJob->setProtectedHeaders(false);
            }
            QObject::connect(job, SIGNAL(finished(KJob *)), q, SLOT(contentJobFinished(KJob *)));
            q->addSubjob(job);
            mainJob = job; // start only last EncryptJob
        }
    } else {
        QObject::connect(mainJob, SIGNAL(finished(KJob *)), q, SLOT(contentJobFinished(KJob *)));
        q->addSubjob(mainJob);
    }

    mainJob->start();
}

QList<ContentJobBase *> ComposerPrivate::createEncryptJobs(ContentJobBase *contentJob, bool sign)
{
    Q_Q(Composer);

    QList<ContentJobBase *> jobs;

    // each SplitInfo holds a list of recipients/keys, if there is more than
    // one item in it then it means there are secondary recipients that need
    // different messages w/ clean headers
    qCDebug(MESSAGECOMPOSER_LOG) << "starting enc jobs";
    qCDebug(MESSAGECOMPOSER_LOG) << "format:" << format;
    qCDebug(MESSAGECOMPOSER_LOG) << "enc data:" << encData.size();

    if (encData.isEmpty()) { // no key data! bail!
        q->setErrorText(i18n("No key data for recipients found."));
        q->setError(Composer::IncompleteError);
        q->emitResult();
        return jobs;
    }

    const int encDataSize = encData.size();
    jobs.reserve(encDataSize);
    for (int i = 0; i < encDataSize; ++i) {
        QPair<QStringList, std::vector<GpgME::Key>> recipients = encData[i];
        qCDebug(MESSAGECOMPOSER_LOG) << "got first list of recipients:" << recipients.first;
        ContentJobBase *subJob = nullptr;
        if (sign) {
            auto seJob = new SignEncryptJob(q);

            seJob->setCryptoMessageFormat(format);
            seJob->setSigningKeys(signers);
            seJob->setEncryptionKeys(recipients.second);
            seJob->setRecipients(recipients.first);
            seJob->setSkeletonMessage(skeletonMessage);

            subJob = seJob;
        } else {
            auto eJob = new EncryptJob(q);
            eJob->setCryptoMessageFormat(format);
            eJob->setEncryptionKeys(recipients.second);
            eJob->setRecipients(recipients.first);
            eJob->setSkeletonMessage(skeletonMessage);
            eJob->setGnupgHome(gnupgHome);
            subJob = eJob;
        }
        qCDebug(MESSAGECOMPOSER_LOG) << "subJob" << subJob;
        subJob->appendSubjob(contentJob);
        jobs.append(subJob);
    }
    qCDebug(MESSAGECOMPOSER_LOG) << jobs.size();
    return jobs;
}

void ComposerPrivate::contentJobFinished(KJob *job)
{
    if (job->error()) {
        return; // KCompositeJob takes care of the error.
    }
    qCDebug(MESSAGECOMPOSER_LOG) << "composing final message";

    KMime::Message *headers = nullptr;
    KMime::Content *resultContent = nullptr;
    std::vector<GpgME::Key> keys;
    QStringList recipients;

    Q_ASSERT(dynamic_cast<ContentJobBase *>(job) == static_cast<ContentJobBase *>(job));
    auto contentJob = static_cast<ContentJobBase *>(job);

    // create the final headers and body,
    // taking into account secondary recipients for encryption
    if (encData.size() > 1) { // crypto job with secondary recipients..
        Q_ASSERT(dynamic_cast<MessageComposer::AbstractEncryptJob *>(job)); // we need to get the recipients for this job
        auto eJob = dynamic_cast<MessageComposer::AbstractEncryptJob *>(job);

        keys = eJob->encryptionKeys();
        recipients = eJob->recipients();

        resultContent = contentJob->content(); // content() comes from superclass
        headers = new KMime::Message;
        headers->setHeader(skeletonMessage->from());
        headers->setHeader(skeletonMessage->to());
        headers->setHeader(skeletonMessage->cc());
        headers->setHeader(skeletonMessage->subject());
        headers->setHeader(skeletonMessage->date());
        headers->setHeader(skeletonMessage->messageID());

        auto realTo = new KMime::Headers::Generic("X-KMail-EncBccRecipients");
        realTo->fromUnicodeString(eJob->recipients().join(QLatin1Char('%')));

        qCDebug(MESSAGECOMPOSER_LOG) << "got one of multiple messages sending to:" << realTo->asUnicodeString();
        qCDebug(MESSAGECOMPOSER_LOG) << "sending to recipients:" << recipients;
        headers->setHeader(realTo);
        headers->assemble();
    } else { // just use the saved headers from before
        if (!encData.isEmpty()) {
            const auto firstElement = encData.at(0);
            qCDebug(MESSAGECOMPOSER_LOG) << "setting enc data:" << firstElement.first << "with num keys:" << firstElement.second.size();
            keys = firstElement.second;
            recipients = firstElement.first;
        }

        headers = skeletonMessage;
        resultContent = contentJob->content();
    }

    if (lateAttachmentParts.isEmpty()) {
        composeFinalStep(headers, resultContent);
    } else {
        composeWithLateAttachments(headers, resultContent, lateAttachmentParts, keys, recipients);
    }
}

void ComposerPrivate::composeWithLateAttachments(KMime::Message *headers,
                                                 KMime::Content *content,
                                                 const AttachmentPart::List &parts,
                                                 const std::vector<GpgME::Key> &keys,
                                                 const QStringList &recipients)
{
    Q_Q(Composer);

    auto multiJob = new MultipartJob(q);
    multiJob->setMultipartSubtype("mixed");

    // wrap the content into a job for the multijob to handle it
    auto tJob = new MessageComposer::TransparentJob(q);
    tJob->setContent(content);
    multiJob->appendSubjob(tJob);
    multiJob->setExtraContent(headers);

    qCDebug(MESSAGECOMPOSER_LOG) << "attachment encr key size:" << keys.size() << " recipients: " << recipients;

    // operate correctly on each attachment that has a different crypto policy than body.
    for (const AttachmentPart::Ptr &attachment : std::as_const(parts)) {
        auto attachJob = new AttachmentJob(attachment, q);

        qCDebug(MESSAGECOMPOSER_LOG) << "got a late attachment";
        if (attachment->isSigned() && format) {
            qCDebug(MESSAGECOMPOSER_LOG) << "adding signjob for late attachment";
            auto sJob = new SignJob(q);
            sJob->setContent(nullptr);
            sJob->setCryptoMessageFormat(format);
            sJob->setSigningKeys(signers);

            sJob->appendSubjob(attachJob);
            if (attachment->isEncrypted()) {
                qCDebug(MESSAGECOMPOSER_LOG) << "adding sign + encrypt job for late attachment";
                auto eJob = new EncryptJob(q);
                eJob->setCryptoMessageFormat(format);
                eJob->setEncryptionKeys(keys);
                eJob->setRecipients(recipients);

                eJob->appendSubjob(sJob);

                multiJob->appendSubjob(eJob);
            } else {
                qCDebug(MESSAGECOMPOSER_LOG) << "Just signing late attachment";
                multiJob->appendSubjob(sJob);
            }
        } else if (attachment->isEncrypted() && format) { // only encryption
            qCDebug(MESSAGECOMPOSER_LOG) << "just encrypting late attachment";
            auto eJob = new EncryptJob(q);
            eJob->setCryptoMessageFormat(format);
            eJob->setEncryptionKeys(keys);
            eJob->setRecipients(recipients);

            eJob->appendSubjob(attachJob);
            multiJob->appendSubjob(eJob);
        } else {
            qCDebug(MESSAGECOMPOSER_LOG) << "attaching plain non-crypto attachment";
            auto attachSecondJob = new AttachmentJob(attachment, q);
            multiJob->appendSubjob(attachSecondJob);
        }
    }

    QObject::connect(multiJob, SIGNAL(finished(KJob *)), q, SLOT(attachmentsFinished(KJob *)));

    q->addSubjob(multiJob);
    multiJob->start();
}

void ComposerPrivate::attachmentsFinished(KJob *job)
{
    if (job->error()) {
        return; // KCompositeJob takes care of the error.
    }
    qCDebug(MESSAGECOMPOSER_LOG) << "composing final message with late attachments";

    Q_ASSERT(dynamic_cast<ContentJobBase *>(job));
    auto contentJob = static_cast<ContentJobBase *>(job);

    KMime::Content *content = contentJob->content();
    KMime::Content *headers = contentJob->extraContent();

    composeFinalStep(headers, content);
}

void ComposerPrivate::composeFinalStep(KMime::Content *headers, KMime::Content *content)
{
    content->assemble();

    const QByteArray allData = headers->head() + content->encodedContent();

    delete content;

    KMime::Message::Ptr resultMessage(new KMime::Message);
    resultMessage->setContent(allData);
    resultMessage->parse(); // Not strictly necessary.
    resultMessages.append(resultMessage);
}

Composer::Composer(QObject *parent)
    : JobBase(*new ComposerPrivate(this), parent)
{
    Q_D(Composer);
    d->init();
}

Composer::~Composer() = default;

QList<KMime::Message::Ptr> Composer::resultMessages() const
{
    Q_D(const Composer);
    Q_ASSERT(d->finished);
    Q_ASSERT(!error());
    return d->resultMessages;
}

GlobalPart *Composer::globalPart() const
{
    Q_D(const Composer);
    return d->globalPart;
}

InfoPart *Composer::infoPart() const
{
    Q_D(const Composer);
    return d->infoPart;
}

TextPart *Composer::textPart() const
{
    Q_D(const Composer);
    if (!d->textPart) {
        auto *self = const_cast<Composer *>(this);
        self->d_func()->textPart = new TextPart(self);
    }
    return d->textPart;
}

void Composer::clearTextPart()
{
    Q_D(Composer);
    delete d->textPart;
    d->textPart = nullptr;
}

ItipPart *Composer::itipPart() const
{
    Q_D(const Composer);
    if (!d->itipPart) {
        auto *self = const_cast<Composer *>(this);
        self->d_func()->itipPart = new ItipPart(self);
    }
    return d->itipPart;
}

void Composer::clearItipPart()
{
    Q_D(Composer);
    delete d->itipPart;
    d->itipPart = nullptr;
}

AttachmentPart::List Composer::attachmentParts() const
{
    Q_D(const Composer);
    return d->attachmentParts;
}

void Composer::addAttachmentPart(AttachmentPart::Ptr part, bool autoresizeImage)
{
    Q_D(Composer);
    Q_ASSERT(!d->started);
    Q_ASSERT(!d->attachmentParts.contains(part));
    if (autoresizeImage) {
        MessageComposer::Utils resizeUtils;
        if (resizeUtils.resizeImage(part)) {
            MessageComposer::ImageScaling autoResizeJob;
            autoResizeJob.setName(part->name());
            autoResizeJob.setMimetype(part->mimeType());
            if (autoResizeJob.loadImageFromData(part->data())) {
                if (autoResizeJob.resizeImage()) {
                    part->setData(autoResizeJob.imageArray());
                    part->setMimeType(autoResizeJob.mimetype());
                    part->setName(autoResizeJob.generateNewName());
                    resizeUtils.changeFileName(part);
                }
            }
        }
    }
    d->attachmentParts.append(part);
}

void Composer::addAttachmentParts(const AttachmentPart::List &parts, bool autoresizeImage)
{
    for (const AttachmentPart::Ptr &part : parts) {
        addAttachmentPart(part, autoresizeImage);
    }
}

void Composer::removeAttachmentPart(AttachmentPart::Ptr part)
{
    Q_D(Composer);
    Q_ASSERT(!d->started);
    const int numberOfElements = d->attachmentParts.removeAll(part);
    if (numberOfElements <= 0) {
        qCCritical(MESSAGECOMPOSER_LOG) << "Unknown attachment part" << part.data();
        Q_ASSERT(false);
        return;
    }
}

void Composer::setSignAndEncrypt(const bool doSign, const bool doEncrypt)
{
    Q_D(Composer);
    d->sign = doSign;
    d->encrypt = doEncrypt;
}

void Composer::setCryptoMessageFormat(Kleo::CryptoMessageFormat format)
{
    Q_D(Composer);

    d->format = format;
}

void Composer::setSigningKeys(const std::vector<GpgME::Key> &signers)
{
    Q_D(Composer);

    d->signers = signers;
}

void Composer::setEncryptionKeys(const QList<QPair<QStringList, std::vector<GpgME::Key>>> &encData)
{
    Q_D(Composer);

    d->encData = encData;
}

void Composer::setNoCrypto(bool noCrypto)
{
    Q_D(Composer);

    d->noCrypto = noCrypto;
}

void Composer::setAutocryptEnabled(bool autocryptEnabled)
{
    Q_D(Composer);

    d->autocryptEnabled = autocryptEnabled;
}

void Composer::setSenderEncryptionKey(const GpgME::Key &senderKey)
{
    Q_D(Composer);

    d->senderEncryptionKey = senderKey;
}

void Composer::setGnupgHome(const QString &path)
{
    Q_D(Composer);

    d->gnupgHome = path;
}

QString Composer::gnupgHome() const
{
    Q_D(const Composer);

    return d->gnupgHome;
}

bool Composer::finished() const
{
    Q_D(const Composer);

    return d->finished;
}

bool Composer::autoSave() const
{
    Q_D(const Composer);

    return d->autoSaving;
}

void Composer::setAutoSave(bool isAutoSave)
{
    Q_D(Composer);

    d->autoSaving = isAutoSave;
}

void Composer::start()
{
    Q_D(Composer);
    d->doStart();
}

void Composer::slotResult(KJob *job)
{
    Q_D(Composer);
    JobBase::slotResult(job);

    if (!hasSubjobs()) {
        d->finished = true;
        emitResult();
    }
}

#include "moc_composer.cpp"
