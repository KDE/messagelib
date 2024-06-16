/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * Parts based on KMail code by:
 * Various authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "attachmentcontrollerbase.h"

#include "MessageComposer/AttachmentClipBoardJob"
#include "MessageComposer/AttachmentFromPublicKeyJob"
#include "MessageComposer/AttachmentJob"
#include "MessageComposer/AttachmentVcardFromAddressBookJob"
#include "MessageComposer/Composer"
#include "MessageComposer/GlobalPart"
#include <MessageComposer/AttachmentModel>

#include <MessageViewer/MessageViewerUtil>

#include <MimeTreeParser/NodeHelper>

#include <MessageCore/StringUtil>

#include <Akonadi/ItemFetchJob>
#include <KIO/JobUiDelegateFactory>
#include <QIcon>
#include <QStringConverter>

#include <QMenu>
#include <QPointer>
#include <QTreeView>

#include "messagecomposer_debug.h"
#include <KActionCollection>
#include <KActionMenu>
#include <KFileItemActions>
#include <KIO/ApplicationLauncherJob>
#include <KLocalizedString>
#include <KMessageBox>
#include <QAction>
#include <QMimeDatabase>
#include <QPushButton>
#include <QTemporaryFile>

#include <Libkleo/KeySelectionDialog>
#include <QGpgME/Protocol>

#include "messagecore/attachmentfromurlbasejob.h"
#include "messagecore/attachmentupdatejob.h"
#include <Akonadi/EmailAddressSelectionDialog>
#include <Akonadi/EmailAddressSelectionWidget>
#include <KIO/JobUiDelegate>
#include <KIO/OpenUrlJob>
#include <KIO/StoredTransferJob>
#include <MessageCore/AttachmentCompressJob>
#include <MessageCore/AttachmentFromUrlUtils>
#include <MessageCore/AttachmentPropertiesDialog>

#include <KJob>
#include <KMime/Content>

#include <QActionGroup>
#include <QFileDialog>

using namespace MessageComposer;
using namespace MessageCore;

class MessageComposer::AttachmentControllerBase::AttachmentControllerBasePrivate
{
public:
    explicit AttachmentControllerBasePrivate(AttachmentControllerBase *qq);
    ~AttachmentControllerBasePrivate();

    void attachmentRemoved(const AttachmentPart::Ptr &part);
    void compressJobResult(KJob *job);
    void loadJobResult(KJob *job);
    void openSelectedAttachments();
    void viewSelectedAttachments();
    void editSelectedAttachment();
    void editSelectedAttachmentWith();
    void removeSelectedAttachments();
    void saveSelectedAttachmentAs();
    void selectedAttachmentProperties();
    void editDone(MessageComposer::EditorWatcher *watcher);
    void attachPublicKeyJobResult(KJob *job);
    void slotAttachmentContentCreated(KJob *job);
    void addAttachmentPart(AttachmentPart::Ptr part);
    void attachVcardFromAddressBook(KJob *job);
    void attachClipBoardElement(KJob *job);
    void selectedAllAttachment();
    void createOpenWithMenu(QMenu *topMenu, const AttachmentPart::Ptr &part);
    void reloadAttachment();
    void updateJobResult(KJob *);

    AttachmentPart::List selectedParts;
    AttachmentControllerBase *const q;
    MessageComposer::AttachmentModel *model = nullptr;
    QWidget *wParent = nullptr;
    QHash<MessageComposer::EditorWatcher *, AttachmentPart::Ptr> editorPart;
    QHash<MessageComposer::EditorWatcher *, QTemporaryFile *> editorTempFile;

    KActionCollection *mActionCollection = nullptr;
    QAction *attachPublicKeyAction = nullptr;
    QAction *attachMyPublicKeyAction = nullptr;
    QAction *openContextAction = nullptr;
    QAction *viewContextAction = nullptr;
    QAction *editContextAction = nullptr;
    QAction *editWithContextAction = nullptr;
    QAction *removeAction = nullptr;
    QAction *removeContextAction = nullptr;
    QAction *saveAsAction = nullptr;
    QAction *saveAsContextAction = nullptr;
    QAction *propertiesAction = nullptr;
    QAction *propertiesContextAction = nullptr;
    QAction *addAttachmentFileAction = nullptr;
    QAction *addAttachmentDirectoryAction = nullptr;
    QAction *addContextAction = nullptr;
    QAction *selectAllAction = nullptr;
    KActionMenu *attachmentMenu = nullptr;
    QAction *addOwnVcardAction = nullptr;
    QAction *reloadAttachmentAction = nullptr;
    QAction *attachVCardsAction = nullptr;
    QAction *attachClipBoardAction = nullptr;
    // If part p is compressed, uncompressedParts[p] is the uncompressed part.
    QHash<AttachmentPart::Ptr, AttachmentPart::Ptr> uncompressedParts;
    bool encryptEnabled = false;
    bool signEnabled = false;
};

AttachmentControllerBase::AttachmentControllerBasePrivate::AttachmentControllerBasePrivate(AttachmentControllerBase *qq)
    : q(qq)
{
}

AttachmentControllerBase::AttachmentControllerBasePrivate::~AttachmentControllerBasePrivate() = default;

void AttachmentControllerBase::setSelectedParts(const AttachmentPart::List &selectedParts)
{
    d->selectedParts = selectedParts;
    const int selectedCount = selectedParts.count();
    const bool enableEditAction = (selectedCount == 1) && (!selectedParts.first()->isMessageOrMessageCollection());

    d->openContextAction->setEnabled(selectedCount > 0);
    d->viewContextAction->setEnabled(selectedCount > 0);
    d->editContextAction->setEnabled(enableEditAction);
    d->editWithContextAction->setEnabled(enableEditAction);
    d->removeAction->setEnabled(selectedCount > 0);
    d->removeContextAction->setEnabled(selectedCount > 0);
    d->saveAsAction->setEnabled(selectedCount == 1);
    d->saveAsContextAction->setEnabled(selectedCount == 1);
    d->propertiesAction->setEnabled(selectedCount == 1);
    d->propertiesContextAction->setEnabled(selectedCount == 1);
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::attachmentRemoved(const AttachmentPart::Ptr &part)
{
    uncompressedParts.remove(part);
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::compressJobResult(KJob *job)
{
    if (job->error()) {
        KMessageBox::error(wParent, job->errorString(), i18nc("@title:window", "Failed to compress attachment"));
        return;
    }

    auto ajob = qobject_cast<AttachmentCompressJob *>(job);
    Q_ASSERT(ajob);
    AttachmentPart::Ptr originalPart = ajob->originalPart();
    AttachmentPart::Ptr compressedPart = ajob->compressedPart();

    if (ajob->isCompressedPartLarger()) {
        const int result = KMessageBox::questionTwoActions(wParent,
                                                           i18n("The compressed attachment is larger than the original. "
                                                                "Do you want to keep the original one?"),
                                                           QString(/*caption*/),
                                                           KGuiItem(i18nc("Do not compress", "Keep")),
                                                           KGuiItem(i18nc("@action:button", "Compress")));
        if (result == KMessageBox::ButtonCode::PrimaryAction) {
            // The user has chosen to keep the uncompressed file.
            return;
        }
    }

    qCDebug(MESSAGECOMPOSER_LOG) << "Replacing uncompressed part in model.";
    uncompressedParts[compressedPart] = originalPart;
    bool ok = model->replaceAttachment(originalPart, compressedPart);
    if (!ok) {
        // The attachment was removed from the model while we were compressing.
        qCDebug(MESSAGECOMPOSER_LOG) << "Compressed a zombie.";
    }
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::loadJobResult(KJob *job)
{
    if (job->error()) {
        KMessageBox::error(wParent, job->errorString(), i18n("Failed to attach file"));
        return;
    }

    auto ajob = qobject_cast<AttachmentLoadJob *>(job);
    Q_ASSERT(ajob);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::openSelectedAttachments()
{
    Q_ASSERT(selectedParts.count() >= 1);
    for (const AttachmentPart::Ptr &part : std::as_const(selectedParts)) {
        q->openAttachment(part);
    }
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::viewSelectedAttachments()
{
    Q_ASSERT(selectedParts.count() >= 1);
    for (const AttachmentPart::Ptr &part : std::as_const(selectedParts)) {
        q->viewAttachment(part);
    }
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::editSelectedAttachment()
{
    Q_ASSERT(selectedParts.count() == 1);
    q->editAttachment(selectedParts.constFirst(), MessageComposer::EditorWatcher::NoOpenWithDialog);
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::editSelectedAttachmentWith()
{
    Q_ASSERT(selectedParts.count() == 1);
    q->editAttachment(selectedParts.constFirst(), MessageComposer::EditorWatcher::OpenWithDialog);
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::removeSelectedAttachments()
{
    Q_ASSERT(selectedParts.count() >= 1);
    // We must store list, otherwise when we remove it changes selectedParts (as selection changed) => it will crash.
    const AttachmentPart::List toRemove = selectedParts;
    for (const AttachmentPart::Ptr &part : toRemove) {
        model->removeAttachment(part);
    }
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::saveSelectedAttachmentAs()
{
    Q_ASSERT(selectedParts.count() == 1);
    q->saveAttachmentAs(selectedParts.constFirst());
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::selectedAttachmentProperties()
{
    Q_ASSERT(selectedParts.count() == 1);
    q->attachmentProperties(selectedParts.constFirst());
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::reloadAttachment()
{
    Q_ASSERT(selectedParts.count() == 1);
    auto ajob = new AttachmentUpdateJob(selectedParts.constFirst(), q);
    connect(ajob, &AttachmentUpdateJob::result, q, [this](KJob *job) {
        updateJobResult(job);
    });
    ajob->start();
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::updateJobResult(KJob *job)
{
    if (job->error()) {
        KMessageBox::error(wParent, job->errorString(), i18n("Failed to reload attachment"));
        return;
    }
    auto ajob = qobject_cast<AttachmentUpdateJob *>(job);
    Q_ASSERT(ajob);
    AttachmentPart::Ptr originalPart = ajob->originalPart();
    AttachmentPart::Ptr updatedPart = ajob->updatedPart();

    attachmentRemoved(originalPart);
    bool ok = model->replaceAttachment(originalPart, updatedPart);
    if (!ok) {
        // The attachment was removed from the model while we were compressing.
        qCDebug(MESSAGECOMPOSER_LOG) << "Updated a zombie.";
    }
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::editDone(MessageComposer::EditorWatcher *watcher)
{
    AttachmentPart::Ptr part = editorPart.take(watcher);
    Q_ASSERT(part);
    QTemporaryFile *tempFile = editorTempFile.take(watcher);
    Q_ASSERT(tempFile);
    if (watcher->fileChanged()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "File has changed.";
        const QString name = watcher->url().path();
        QFile file(name);
        if (file.open(QIODevice::ReadOnly)) {
            const QByteArray data = file.readAll();
            part->setData(data);
            model->updateAttachment(part);
        }
    }

    delete tempFile;
    // The watcher deletes itself.
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::createOpenWithMenu(QMenu *topMenu, const AttachmentPart::Ptr &part)
{
    const QString contentTypeStr = QString::fromLatin1(part->mimeType());
    const KService::List offers = KFileItemActions::associatedApplications(QStringList() << contentTypeStr);
    if (!offers.isEmpty()) {
        QMenu *menu = topMenu;
        auto actionGroup = new QActionGroup(menu);
        connect(actionGroup, &QActionGroup::triggered, q, &AttachmentControllerBase::slotOpenWithAction);

        if (offers.count() > 1) { // submenu 'open with'
            menu = new QMenu(i18nc("@title:menu", "&Open With"), topMenu);
            menu->menuAction()->setObjectName(QLatin1StringView("openWith_submenu")); // for the unittest
            topMenu->addMenu(menu);
        }
        // qCDebug(MESSAGECOMPOSER_LOG) << offers.count() << "offers" << topMenu << menu;

        KService::List::ConstIterator it = offers.constBegin();
        KService::List::ConstIterator end = offers.constEnd();
        for (; it != end; ++it) {
            QAction *act = MessageViewer::Util::createAppAction(*it,
                                                                // no submenu -> prefix single offer
                                                                menu == topMenu,
                                                                actionGroup,
                                                                menu);
            menu->addAction(act);
        }

        QString openWithActionName;
        if (menu != topMenu) { // submenu
            menu->addSeparator();
            openWithActionName = i18nc("@action:inmenu Open With", "&Other...");
        } else {
            openWithActionName = i18nc("@title:menu", "&Open With...");
        }
        auto openWithAct = new QAction(menu);
        openWithAct->setText(openWithActionName);
        QObject::connect(openWithAct, &QAction::triggered, q, &AttachmentControllerBase::slotOpenWithDialog);
        menu->addAction(openWithAct);
    } else { // no app offers -> Open With...
        auto act = new QAction(topMenu);
        act->setText(i18nc("@title:menu", "&Open With..."));
        QObject::connect(act, &QAction::triggered, q, &AttachmentControllerBase::slotOpenWithDialog);
        topMenu->addAction(act);
    }
}

void AttachmentControllerBase::exportPublicKey(const QString &fingerprint)
{
    if (fingerprint.isEmpty() || !QGpgME::openpgp()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Tried to export key with empty fingerprint, or no OpenPGP.";
        return;
    }

    auto ajob = new MessageComposer::AttachmentFromPublicKeyJob(fingerprint, this);
    connect(ajob, &AttachmentFromPublicKeyJob::result, this, [this](KJob *job) {
        d->attachPublicKeyJobResult(job);
    });
    ajob->start();
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::attachPublicKeyJobResult(KJob *job)
{
    // The only reason we can't use loadJobResult() and need a separate method
    // is that we want to show the proper caption ("public key" instead of "file")...

    if (job->error()) {
        KMessageBox::error(wParent, job->errorString(), i18n("Failed to attach public key"));
        return;
    }

    Q_ASSERT(dynamic_cast<MessageComposer::AttachmentFromPublicKeyJob *>(job));
    auto ajob = static_cast<MessageComposer::AttachmentFromPublicKeyJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::attachVcardFromAddressBook(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << " Error during when get vCard";
        KMessageBox::error(wParent, job->errorString(), i18n("Failed to attach vCard"));
        return;
    }

    auto ajob = static_cast<MessageComposer::AttachmentVcardFromAddressBookJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::attachClipBoardElement(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << " Error during when get try to attach text from clipboard";
        KMessageBox::error(wParent, job->errorString(), i18n("Failed to attach text from clipboard"));
        return;
    }

    auto ajob = static_cast<MessageComposer::AttachmentClipBoardJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

static QTemporaryFile *dumpAttachmentToTempFile(const AttachmentPart::Ptr &part) // local
{
    auto file = new QTemporaryFile;
    if (!file->open()) {
        qCCritical(MESSAGECOMPOSER_LOG) << "Could not open tempfile" << file->fileName();
        delete file;
        return nullptr;
    }
    if (file->write(part->data()) == -1) {
        qCCritical(MESSAGECOMPOSER_LOG) << "Could not dump attachment to tempfile.";
        delete file;
        return nullptr;
    }
    file->flush();
    return file;
}

AttachmentControllerBase::AttachmentControllerBase(MessageComposer::AttachmentModel *model, QWidget *wParent, KActionCollection *actionCollection)
    : QObject(wParent)
    , d(new AttachmentControllerBasePrivate(this))
{
    d->model = model;
    connect(model, &MessageComposer::AttachmentModel::attachUrlsRequested, this, &AttachmentControllerBase::addAttachments);
    connect(model, &MessageComposer::AttachmentModel::attachmentRemoved, this, [this](const MessageCore::AttachmentPart::Ptr &attr) {
        d->attachmentRemoved(attr);
    });
    connect(model, &AttachmentModel::attachmentCompressRequested, this, &AttachmentControllerBase::compressAttachment);
    connect(model, &MessageComposer::AttachmentModel::encryptEnabled, this, &AttachmentControllerBase::setEncryptEnabled);
    connect(model, &MessageComposer::AttachmentModel::signEnabled, this, &AttachmentControllerBase::setSignEnabled);

    d->wParent = wParent;
    d->mActionCollection = actionCollection;
}

AttachmentControllerBase::~AttachmentControllerBase() = default;

void AttachmentControllerBase::createActions()
{
    // Create the actions.
    d->attachPublicKeyAction = new QAction(i18nc("@action", "Attach &Public Key..."), this);
    connect(d->attachPublicKeyAction, &QAction::triggered, this, &AttachmentControllerBase::showAttachPublicKeyDialog);

    d->attachMyPublicKeyAction = new QAction(i18nc("@action", "Attach &My Public Key"), this);
    connect(d->attachMyPublicKeyAction, &QAction::triggered, this, &AttachmentControllerBase::attachMyPublicKey);

    d->attachmentMenu = new KActionMenu(QIcon::fromTheme(QStringLiteral("mail-attachment")), i18n("Attach"), this);
    connect(d->attachmentMenu, &QAction::triggered, this, &AttachmentControllerBase::showAddAttachmentFileDialog);
    d->attachmentMenu->setPopupMode(QToolButton::DelayedPopup);

    d->addAttachmentFileAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-attachment")), i18n("&Attach File..."), this);
    d->addAttachmentFileAction->setIconText(i18n("Attach"));
    d->addContextAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-attachment")), i18n("Add Attachment..."), this);
    connect(d->addAttachmentFileAction, &QAction::triggered, this, &AttachmentControllerBase::showAddAttachmentFileDialog);
    connect(d->addContextAction, &QAction::triggered, this, &AttachmentControllerBase::showAddAttachmentFileDialog);

    d->addAttachmentDirectoryAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-attachment")), i18n("&Attach Directory..."), this);
    d->addAttachmentDirectoryAction->setIconText(i18n("Attach"));
    connect(d->addAttachmentDirectoryAction, &QAction::triggered, this, &AttachmentControllerBase::showAddAttachmentCompressedDirectoryDialog);

    d->addOwnVcardAction = new QAction(i18nc("@action", "Attach Own vCard"), this);
    d->addOwnVcardAction->setIconText(i18n("Own vCard"));
    d->addOwnVcardAction->setCheckable(true);
    connect(d->addOwnVcardAction, &QAction::triggered, this, &AttachmentControllerBase::addOwnVcard);

    d->attachVCardsAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-attachment")), i18n("&Attach vCards..."), this);
    d->attachVCardsAction->setIconText(i18n("Attach"));
    connect(d->attachVCardsAction, &QAction::triggered, this, &AttachmentControllerBase::showAttachVcard);

    d->attachClipBoardAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-attachment")), i18n("&Attach Text From Clipboard..."), this);
    d->attachClipBoardAction->setIconText(i18n("Attach Text From Clipboard"));
    connect(d->attachClipBoardAction, &QAction::triggered, this, &AttachmentControllerBase::showAttachClipBoard);

    d->attachmentMenu->addAction(d->addAttachmentFileAction);
    d->attachmentMenu->addAction(d->addAttachmentDirectoryAction);
    d->attachmentMenu->addSeparator();
    d->attachmentMenu->addAction(d->addOwnVcardAction);
    d->attachmentMenu->addSeparator();
    d->attachmentMenu->addAction(d->attachVCardsAction);
    d->attachmentMenu->addSeparator();
    d->attachmentMenu->addAction(d->attachClipBoardAction);

    d->removeAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("&Remove Attachment"), this);
    d->removeContextAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove"), this); // FIXME need two texts. is there a better way?
    connect(d->removeAction, &QAction::triggered, this, [this]() {
        d->removeSelectedAttachments();
    });
    connect(d->removeContextAction, &QAction::triggered, this, [this]() {
        d->removeSelectedAttachments();
    });

    d->openContextAction = new QAction(i18nc("to open", "Open"), this);
    connect(d->openContextAction, &QAction::triggered, this, [this]() {
        d->openSelectedAttachments();
    });

    d->viewContextAction = new QAction(i18nc("to view", "View"), this);
    connect(d->viewContextAction, &QAction::triggered, this, [this]() {
        d->viewSelectedAttachments();
    });

    d->editContextAction = new QAction(i18nc("to edit", "Edit"), this);
    connect(d->editContextAction, &QAction::triggered, this, [this]() {
        d->editSelectedAttachment();
    });

    d->editWithContextAction = new QAction(i18nc("@action", "Edit With..."), this);
    connect(d->editWithContextAction, &QAction::triggered, this, [this]() {
        d->editSelectedAttachmentWith();
    });

    d->saveAsAction = new QAction(QIcon::fromTheme(QStringLiteral("document-save-as")), i18n("&Save Attachment As..."), this);
    d->saveAsContextAction = new QAction(QIcon::fromTheme(QStringLiteral("document-save-as")), i18n("Save As..."), this);
    connect(d->saveAsAction, &QAction::triggered, this, [this]() {
        d->saveSelectedAttachmentAs();
    });
    connect(d->saveAsContextAction, &QAction::triggered, this, [this]() {
        d->saveSelectedAttachmentAs();
    });

    d->propertiesAction = new QAction(i18nc("@action", "Attachment Pr&operties..."), this);
    d->propertiesContextAction = new QAction(i18nc("@action", "Properties"), this);
    connect(d->propertiesAction, &QAction::triggered, this, [this]() {
        d->selectedAttachmentProperties();
    });
    connect(d->propertiesContextAction, &QAction::triggered, this, [this]() {
        d->selectedAttachmentProperties();
    });

    d->selectAllAction = new QAction(i18nc("@action", "Select All"), this);
    connect(d->selectAllAction, &QAction::triggered, this, &AttachmentControllerBase::selectedAllAttachment);

    d->reloadAttachmentAction = new QAction(QIcon::fromTheme(QStringLiteral("view-refresh")), i18n("Reload"), this);
    connect(d->reloadAttachmentAction, &QAction::triggered, this, [this]() {
        d->reloadAttachment();
    });

    // Insert the actions into the composer window's menu.
    KActionCollection *collection = d->mActionCollection;
    collection->addAction(QStringLiteral("attach_public_key"), d->attachPublicKeyAction);
    collection->addAction(QStringLiteral("attach_my_public_key"), d->attachMyPublicKeyAction);
    collection->addAction(QStringLiteral("attach"), d->addAttachmentFileAction);
    collection->setDefaultShortcut(d->addAttachmentFileAction, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A));
    collection->addAction(QStringLiteral("attach_directory"), d->addAttachmentDirectoryAction);

    collection->addAction(QStringLiteral("remove"), d->removeAction);
    collection->addAction(QStringLiteral("attach_save"), d->saveAsAction);
    collection->addAction(QStringLiteral("attach_properties"), d->propertiesAction);
    collection->addAction(QStringLiteral("select_all_attachment"), d->selectAllAction);
    collection->addAction(QStringLiteral("attach_menu"), d->attachmentMenu);
    collection->addAction(QStringLiteral("attach_own_vcard"), d->addOwnVcardAction);
    collection->addAction(QStringLiteral("attach_vcards"), d->attachVCardsAction);

    setSelectedParts(AttachmentPart::List());
    Q_EMIT actionsCreated();
}

void AttachmentControllerBase::setEncryptEnabled(bool enabled)
{
    d->encryptEnabled = enabled;
}

void AttachmentControllerBase::setSignEnabled(bool enabled)
{
    d->signEnabled = enabled;
}

void AttachmentControllerBase::compressAttachment(const AttachmentPart::Ptr &part, bool compress)
{
    if (compress) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Compressing part.";

        auto ajob = new AttachmentCompressJob(part, this);
        connect(ajob, &AttachmentCompressJob::result, this, [this](KJob *job) {
            d->compressJobResult(job);
        });
        ajob->start();
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "Uncompressing part.";

        // Replace the compressed part with the original uncompressed part, and delete
        // the compressed part.
        AttachmentPart::Ptr originalPart = d->uncompressedParts.take(part);
        Q_ASSERT(originalPart); // Found in uncompressedParts.
        bool ok = d->model->replaceAttachment(part, originalPart);
        Q_ASSERT(ok);
        Q_UNUSED(ok)
    }
}

void AttachmentControllerBase::showContextMenu()
{
    Q_EMIT refreshSelection();

    const int numberOfParts(d->selectedParts.count());
    QMenu menu;

    const bool enableEditAction = (numberOfParts == 1) && (!d->selectedParts.first()->isMessageOrMessageCollection());

    if (numberOfParts > 0) {
        if (numberOfParts == 1) {
            const QString mimetype = QString::fromLatin1(d->selectedParts.first()->mimeType());
            QMimeDatabase mimeDb;
            auto mime = mimeDb.mimeTypeForName(mimetype);
            QStringList parentMimeType;
            if (mime.isValid()) {
                parentMimeType = mime.allAncestors();
            }
            if ((mimetype == QLatin1StringView("text/plain")) || (mimetype == QLatin1StringView("image/png")) || (mimetype == QLatin1StringView("image/jpeg"))
                || parentMimeType.contains(QLatin1StringView("text/plain")) || parentMimeType.contains(QLatin1StringView("image/png"))
                || parentMimeType.contains(QLatin1StringView("image/jpeg"))) {
                menu.addAction(d->viewContextAction);
            }
            d->createOpenWithMenu(&menu, d->selectedParts.constFirst());
        }
        menu.addAction(d->openContextAction);
    }
    if (enableEditAction) {
        menu.addAction(d->editWithContextAction);
        menu.addAction(d->editContextAction);
    }
    menu.addSeparator();
    if (numberOfParts == 1) {
        if (!d->selectedParts.first()->url().isEmpty()) {
            menu.addAction(d->reloadAttachmentAction);
        }
        menu.addAction(d->saveAsContextAction);
        menu.addSeparator();
        menu.addAction(d->propertiesContextAction);
        menu.addSeparator();
    }

    if (numberOfParts > 0) {
        menu.addAction(d->removeContextAction);
        menu.addSeparator();
    }
    const int nbAttachment = d->model->rowCount();
    if (nbAttachment != numberOfParts) {
        menu.addAction(d->selectAllAction);
        menu.addSeparator();
    }
    if (numberOfParts == 0) {
        menu.addAction(d->addContextAction);
    }

    menu.exec(QCursor::pos());
}

void AttachmentControllerBase::slotOpenWithDialog()
{
    openWith();
}

void AttachmentControllerBase::slotOpenWithAction(QAction *act)
{
    auto app = act->data().value<KService::Ptr>();
    Q_ASSERT(d->selectedParts.count() == 1);

    openWith(app);
}

void AttachmentControllerBase::openWith(const KService::Ptr &offer)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(d->selectedParts.constFirst());
    if (!tempFile) {
        KMessageBox::error(d->wParent,
                           i18n("KMail was unable to write the attachment to a temporary file."),
                           i18nc("@title:window", "Unable to open attachment"));
        return;
    }
    QUrl url = QUrl::fromLocalFile(tempFile->fileName());
    tempFile->setPermissions(QFile::ReadUser);
    // If offer is null, this will show the "open with" dialog
    auto job = new KIO::ApplicationLauncherJob(offer);
    job->setUrls({url});
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, d->wParent));
    job->start();
    connect(job, &KJob::result, this, [tempFile, job]() {
        if (job->error()) {
            delete tempFile;
        }
    });
    // Delete the file only when the composer is closed
    // (and this object is destroyed).
    tempFile->setParent(this); // Manages lifetime.
}

void AttachmentControllerBase::openAttachment(const AttachmentPart::Ptr &part)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(part);
    if (!tempFile) {
        KMessageBox::error(d->wParent,
                           i18n("KMail was unable to write the attachment to a temporary file."),
                           i18nc("@title:window", "Unable to open attachment"));
        return;
    }
    tempFile->setPermissions(QFile::ReadUser);
    auto job = new KIO::OpenUrlJob(QUrl::fromLocalFile(tempFile->fileName()), QString::fromLatin1(part->mimeType()));
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, d->wParent));
    job->setDeleteTemporaryFile(true);
    connect(job, &KIO::OpenUrlJob::result, this, [this, tempFile](KJob *job) {
        if (job->error() == KIO::ERR_USER_CANCELED) {
            KMessageBox::error(d->wParent, i18n("KMail was unable to open the attachment."), job->errorString());
            delete tempFile;
        } else {
            // The file was opened.  Delete it only when the composer is closed
            // (and this object is destroyed).
            tempFile->setParent(this); // Manages lifetime.
        }
    });
    job->start();
}

void AttachmentControllerBase::viewAttachment(const AttachmentPart::Ptr &part)
{
    auto composer = new MessageComposer::Composer;
    auto attachmentJob = new MessageComposer::AttachmentJob(part, composer);
    connect(attachmentJob, &AttachmentJob::result, this, [this](KJob *job) {
        d->slotAttachmentContentCreated(job);
    });
    attachmentJob->start();
}

void AttachmentControllerBase::AttachmentControllerBasePrivate::slotAttachmentContentCreated(KJob *job)
{
    if (!job->error()) {
        const MessageComposer::AttachmentJob *const attachmentJob = qobject_cast<MessageComposer::AttachmentJob *>(job);
        Q_ASSERT(attachmentJob);
        if (attachmentJob) {
            Q_EMIT q->showAttachment(attachmentJob->content(), QByteArray());
        }
    } else {
        // TODO: show warning to the user
        qCWarning(MESSAGECOMPOSER_LOG) << "Error creating KMime::Content for attachment:" << job->errorText();
    }
}

void AttachmentControllerBase::editAttachment(AttachmentPart::Ptr part, MessageComposer::EditorWatcher::OpenWithOption openWithOption)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(part);
    if (!tempFile) {
        KMessageBox::error(d->wParent,
                           i18n("KMail was unable to write the attachment to a temporary file."),
                           i18nc("@title:window", "Unable to edit attachment"));
        return;
    }

    auto watcher =
        new MessageComposer::EditorWatcher(QUrl::fromLocalFile(tempFile->fileName()), QString::fromLatin1(part->mimeType()), openWithOption, this, d->wParent);
    connect(watcher, &MessageComposer::EditorWatcher::editDone, this, [this](MessageComposer::EditorWatcher *watcher) {
        d->editDone(watcher);
    });

    switch (watcher->start()) {
    case MessageComposer::EditorWatcher::NoError:
        // The attachment is being edited.
        // We will clean things up in editDone().
        d->editorPart[watcher] = part;
        d->editorTempFile[watcher] = tempFile;

        // Delete the temp file if the composer is closed (and this object is destroyed).
        tempFile->setParent(this); // Manages lifetime.
        break;
    case MessageComposer::EditorWatcher::CannotStart:
        qCWarning(MESSAGECOMPOSER_LOG) << "Could not start EditorWatcher.";
        [[fallthrough]];
    case MessageComposer::EditorWatcher::Unknown:
    case MessageComposer::EditorWatcher::Canceled:
    case MessageComposer::EditorWatcher::NoServiceFound:
        delete watcher;
        delete tempFile;
        break;
    }
}

void AttachmentControllerBase::editAttachmentWith(const AttachmentPart::Ptr &part)
{
    editAttachment(part, MessageComposer::EditorWatcher::OpenWithDialog);
}

void AttachmentControllerBase::saveAttachmentAs(const AttachmentPart::Ptr &part)
{
    QString pname = part->name();
    if (pname.isEmpty()) {
        pname = i18n("unnamed");
    }

    const QUrl url = QFileDialog::getSaveFileUrl(d->wParent, i18n("Save Attachment As"), QUrl::fromLocalFile(pname));

    if (url.isEmpty()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Save Attachment As dialog canceled.";
        return;
    }

    byteArrayToRemoteFile(part->data(), url);
}

void AttachmentControllerBase::byteArrayToRemoteFile(const QByteArray &aData, const QUrl &aURL, bool overwrite)
{
    KIO::StoredTransferJob *job = KIO::storedPut(aData, aURL, -1, overwrite ? KIO::Overwrite : KIO::DefaultFlags);
    connect(job, &KIO::StoredTransferJob::result, this, &AttachmentControllerBase::slotPutResult);
}

void AttachmentControllerBase::slotPutResult(KJob *job)
{
    auto _job = qobject_cast<KIO::StoredTransferJob *>(job);

    if (job->error()) {
        if (job->error() == KIO::ERR_FILE_ALREADY_EXIST) {
            if (KMessageBox::warningContinueCancel(nullptr,
                                                   i18n("File %1 exists.\nDo you want to replace it?", _job->url().toLocalFile()),
                                                   i18nc("@title:window", "Save to File"),
                                                   KGuiItem(i18nc("@action:button", "&Replace")))
                == KMessageBox::Continue) {
                byteArrayToRemoteFile(_job->data(), _job->url(), true);
            }
        } else {
            KJobUiDelegate *ui = static_cast<KIO::Job *>(job)->uiDelegate();
            ui->showErrorMessage();
        }
    }
}

void AttachmentControllerBase::attachmentProperties(const AttachmentPart::Ptr &part)
{
    QPointer<AttachmentPropertiesDialog> dialog = new AttachmentPropertiesDialog(part, false, d->wParent);

    dialog->setEncryptEnabled(d->encryptEnabled);
    dialog->setSignEnabled(d->signEnabled);

    if (dialog->exec() && dialog) {
        d->model->updateAttachment(part);
    }
    delete dialog;
}

void AttachmentControllerBase::attachDirectory(const QUrl &url)
{
    const int rc = KMessageBox::warningTwoActions(d->wParent,
                                                  i18n("Do you really want to attach this directory \"%1\"?", url.toLocalFile()),
                                                  i18nc("@title:window", "Attach directory"),
                                                  KGuiItem(i18nc("@action:button", "Attach")),
                                                  KStandardGuiItem::cancel());
    if (rc == KMessageBox::ButtonCode::PrimaryAction) {
        addAttachment(url);
    }
}

void AttachmentControllerBase::showAttachVcard()
{
    QPointer<Akonadi::EmailAddressSelectionDialog> dlg = new Akonadi::EmailAddressSelectionDialog(d->wParent);
    dlg->view()->view()->setSelectionMode(QAbstractItemView::MultiSelection);
    if (dlg->exec()) {
        const Akonadi::EmailAddressSelection::List selectedEmail = dlg->selectedAddresses();
        for (const Akonadi::EmailAddressSelection &selected : selectedEmail) {
            auto ajob = new MessageComposer::AttachmentVcardFromAddressBookJob(selected.item(), this);
            connect(ajob, &AttachmentVcardFromAddressBookJob::result, this, [this](KJob *job) {
                d->attachVcardFromAddressBook(job);
            });
            ajob->start();
        }
    }
    delete dlg;
}

void AttachmentControllerBase::showAttachClipBoard()
{
    auto job = new MessageComposer::AttachmentClipBoardJob(this);
    connect(job, &AttachmentClipBoardJob::result, this, [this](KJob *job) {
        d->attachClipBoardElement(job);
    });
    job->start();
}

void AttachmentControllerBase::showAddAttachmentCompressedDirectoryDialog()
{
    const QUrl url = QFileDialog::getExistingDirectoryUrl(d->wParent, i18nc("@title:window", "Attach Directory"));
    if (url.isValid()) {
        attachDirectory(url);
    }
}

void AttachmentControllerBase::showAddAttachmentFileDialog()
{
    const auto urls = QFileDialog::getOpenFileUrls(d->wParent, i18nc("@title:window", "Attach File"));
    if (!urls.isEmpty()) {
        const int numberOfFiles(urls.count());
        for (int i = 0; i < numberOfFiles; ++i) {
            const QUrl url = urls.at(i);
            std::optional<QStringConverter::Encoding> encoding;

            QFile file(url.toLocalFile());
            if (file.open(QIODeviceBase::ReadOnly)) {
                auto content = file.read(1024 * 1024); // only read the first 1MB
                if (content.isEmpty()) {
                    encoding = QStringConverter::System;
                } else if (url.toLocalFile().endsWith(QStringLiteral("html"))) {
                    encoding = QStringConverter::encodingForHtml(content);
                } else {
                    encoding = QStringConverter::encodingForData(content);
                }
            }

            auto encodingName = QStringConverter::nameForEncoding(encoding.value_or(QStringConverter::System));
            if (!encodingName) {
                encodingName = "UTF-8";
            }

            if (strcmp(encodingName, "Locale") == 0) {
                encodingName = "UTF-8";
            }

            QUrl urlWithEncoding = url;
            MessageCore::StringUtil::setEncodingFile(urlWithEncoding, QLatin1StringView(encodingName));
            QMimeDatabase mimeDb;
            const auto mimeType = mimeDb.mimeTypeForUrl(urlWithEncoding);
            if (mimeType.name() == QLatin1StringView("inode/directory")) {
                const int rc = KMessageBox::warningTwoActions(d->wParent,
                                                              i18n("Do you really want to attach this directory \"%1\"?", url.toLocalFile()),
                                                              i18nc("@title:window", "Attach directory"),
                                                              KGuiItem(i18nc("@action:button", "Attach")),
                                                              KStandardGuiItem::cancel());
                if (rc == KMessageBox::ButtonCode::PrimaryAction) {
                    addAttachment(urlWithEncoding);
                }
            } else {
                addAttachment(urlWithEncoding);
            }
        }
    }
}

void AttachmentControllerBase::addAttachment(const AttachmentPart::Ptr &part)
{
    part->setEncrypted(d->model->isEncryptSelected());
    part->setSigned(d->model->isSignSelected());
    d->model->addAttachment(part);

    Q_EMIT fileAttached();
}

void AttachmentControllerBase::addAttachmentUrlSync(const QUrl &url)
{
    MessageCore::AttachmentFromUrlBaseJob *ajob = MessageCore::AttachmentFromUrlUtils::createAttachmentJob(url, this);
    if (ajob->exec()) {
        AttachmentPart::Ptr part = ajob->attachmentPart();
        addAttachment(part);
    } else {
        if (ajob->error()) {
            KMessageBox::error(d->wParent, ajob->errorString(), i18nc("@title:window", "Failed to attach file"));
        }
    }
}

void AttachmentControllerBase::addAttachment(const QUrl &url)
{
    MessageCore::AttachmentFromUrlBaseJob *ajob = MessageCore::AttachmentFromUrlUtils::createAttachmentJob(url, this);
    connect(ajob, &AttachmentFromUrlBaseJob::result, this, [this](KJob *job) {
        d->loadJobResult(job);
    });
    ajob->start();
}

void AttachmentControllerBase::addAttachments(const QList<QUrl> &urls)
{
    for (const QUrl &url : urls) {
        addAttachment(url);
    }
}

void AttachmentControllerBase::showAttachPublicKeyDialog()
{
    using Kleo::KeySelectionDialog;
    QPointer<KeySelectionDialog> dialog = new KeySelectionDialog(i18n("Attach Public OpenPGP Key"),
                                                                 i18n("Select the public key which should be attached."),
                                                                 std::vector<GpgME::Key>(),
                                                                 KeySelectionDialog::PublicKeys | KeySelectionDialog::OpenPGPKeys,
                                                                 false /* no multi selection */,
                                                                 false /* no remember choice box */,
                                                                 d->wParent);

    if (dialog->exec() == QDialog::Accepted) {
        exportPublicKey(dialog->fingerprint());
    }
    delete dialog;
}

void AttachmentControllerBase::attachMyPublicKey()
{
}

void AttachmentControllerBase::enableAttachPublicKey(bool enable)
{
    d->attachPublicKeyAction->setEnabled(enable);
}

void AttachmentControllerBase::enableAttachMyPublicKey(bool enable)
{
    d->attachMyPublicKeyAction->setEnabled(enable);
}

void AttachmentControllerBase::setAttachOwnVcard(bool attachVcard)
{
    d->addOwnVcardAction->setChecked(attachVcard);
}

bool AttachmentControllerBase::attachOwnVcard() const
{
    return d->addOwnVcardAction->isChecked();
}

void AttachmentControllerBase::setIdentityHasOwnVcard(bool state)
{
    d->addOwnVcardAction->setEnabled(state);
}

#include "moc_attachmentcontrollerbase.cpp"
