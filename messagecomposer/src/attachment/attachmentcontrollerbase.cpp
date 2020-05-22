/*
 * This file is part of KMail.
 * Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * Parts based on KMail code by:
 * Various authors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "attachmentcontrollerbase.h"

#include <MessageComposer/AttachmentModel>
#include "MessageComposer/AttachmentJob"
#include "MessageComposer/AttachmentFromPublicKeyJob"
#include "MessageComposer/AttachmentVcardFromAddressBookJob"
#include "MessageComposer/AttachmentClipBoardJob"
#include "MessageComposer/Composer"
#include "MessageComposer/GlobalPart"

#include <MessageViewer/EditorWatcher>
#include <MessageViewer/MessageViewerUtil>

#include <MimeTreeParser/NodeHelper>

#include <MessageCore/StringUtil>

#include <AkonadiCore/itemfetchjob.h>
#include <kio/jobuidelegate.h>
#include <QIcon>

#include <QMenu>
#include <QPointer>
#include <QTreeView>
#include <QUrlQuery>

#include <QAction>
#include <KActionCollection>
#include "messagecomposer_debug.h"
#include <KEncodingFileDialog>
#include <KLocalizedString>
#include <KMessageBox>
#include <KMimeTypeTrader>
#include <QPushButton>
#include <KRun>
#include <QTemporaryFile>
#include <QMimeDatabase>
#include <KFileItemActions>
#include <KActionMenu>

#include <QGpgME/Protocol>
#include <Libkleo/KeySelectionDialog>

#include <MessageCore/AttachmentCompressJob>
#include "attachment/attachmentfrommimecontentjob.h"
#include "attachment/attachmentfromurljob.h"
#include <MessageCore/AttachmentPropertiesDialog>
#include "messagecore/attachmentupdatejob.h"
#include <MessageCore/AttachmentFromUrlUtils>
#include <settings/messagecomposersettings.h>
#include <KIO/Job>
#include <kio_version.h>
#if KIO_VERSION >= QT_VERSION_CHECK(5, 71, 0)
#include <KIO/JobUiDelegate>
#include <KIO/OpenUrlJob>
#endif
#include <Akonadi/Contact/EmailAddressSelectionDialog>
#include <Akonadi/Contact/EmailAddressSelectionWidget>

#include <KMime/Content>
#include <QFileDialog>

using namespace MessageComposer;
using namespace MessageCore;

class Q_DECL_HIDDEN MessageComposer::AttachmentControllerBase::Private
{
public:
    Private(AttachmentControllerBase *qq);
    ~Private();

    void attachmentRemoved(const AttachmentPart::Ptr &part);   // slot
    void compressJobResult(KJob *job);   // slot
    void loadJobResult(KJob *job);   // slot
    void openSelectedAttachments(); // slot
    void viewSelectedAttachments(); // slot
    void editSelectedAttachment(); // slot
    void editSelectedAttachmentWith(); // slot
    void removeSelectedAttachments(); // slot
    void saveSelectedAttachmentAs(); // slot
    void selectedAttachmentProperties(); // slot
    void editDone(MessageViewer::EditorWatcher *watcher);   // slot
    void attachPublicKeyJobResult(KJob *job);   // slot
    void slotAttachmentContentCreated(KJob *job);   // slot
    void addAttachmentPart(AttachmentPart::Ptr part);
    void attachVcardFromAddressBook(KJob *job);   //slot
    void attachClipBoardElement(KJob *job);
    void selectedAllAttachment();
    void createOpenWithMenu(QMenu *topMenu, const AttachmentPart::Ptr &part);
    void reloadAttachment();
    void updateJobResult(KJob *);

    AttachmentPart::List selectedParts;
    AttachmentControllerBase *const q;
    MessageComposer::AttachmentModel *model = nullptr;
    QWidget *wParent = nullptr;
    QHash<MessageViewer::EditorWatcher *, AttachmentPart::Ptr> editorPart;
    QHash<MessageViewer::EditorWatcher *, QTemporaryFile *> editorTempFile;

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

AttachmentControllerBase::Private::Private(AttachmentControllerBase *qq)
    : q(qq)
{
}

AttachmentControllerBase::Private::~Private()
{
}

void AttachmentControllerBase::setSelectedParts(const AttachmentPart::List &selectedParts)
{
    d->selectedParts = selectedParts;
    const int selectedCount = selectedParts.count();
    const bool enableEditAction = (selectedCount == 1)
                                  && (!selectedParts.first()->isMessageOrMessageCollection());

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

void AttachmentControllerBase::Private::attachmentRemoved(const AttachmentPart::Ptr &part)
{
    uncompressedParts.remove(part);
}

void AttachmentControllerBase::Private::compressJobResult(KJob *job)
{
    if (job->error()) {
        KMessageBox::sorry(wParent, job->errorString(), i18n("Failed to compress attachment"));
        return;
    }

    Q_ASSERT(dynamic_cast<AttachmentCompressJob *>(job));
    AttachmentCompressJob *ajob = static_cast<AttachmentCompressJob *>(job);
    AttachmentPart::Ptr originalPart = ajob->originalPart();
    AttachmentPart::Ptr compressedPart = ajob->compressedPart();

    if (ajob->isCompressedPartLarger()) {
        const int result = KMessageBox::questionYesNo(wParent,
                                                      i18n("The compressed attachment is larger than the original. "
                                                           "Do you want to keep the original one?"),
                                                      QString(/*caption*/),
                                                      KGuiItem(i18nc("Do not compress", "Keep")),
                                                      KGuiItem(i18n("Compress")));
        if (result == KMessageBox::Yes) {
            // The user has chosen to keep the uncompressed file.
            return;
        }
    }

    qCDebug(MESSAGECOMPOSER_LOG) << "Replacing uncompressed part in model.";
    uncompressedParts[ compressedPart ] = originalPart;
    bool ok = model->replaceAttachment(originalPart, compressedPart);
    if (!ok) {
        // The attachment was removed from the model while we were compressing.
        qCDebug(MESSAGECOMPOSER_LOG) << "Compressed a zombie.";
    }
}

void AttachmentControllerBase::Private::loadJobResult(KJob *job)
{
    if (job->error()) {
        KMessageBox::sorry(wParent, job->errorString(), i18n("Failed to attach file"));
        return;
    }

    Q_ASSERT(dynamic_cast<AttachmentLoadJob *>(job));
    AttachmentLoadJob *ajob = static_cast<AttachmentLoadJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

void AttachmentControllerBase::Private::openSelectedAttachments()
{
    Q_ASSERT(selectedParts.count() >= 1);
    for (const AttachmentPart::Ptr &part : qAsConst(selectedParts)) {
        q->openAttachment(part);
    }
}

void AttachmentControllerBase::Private::viewSelectedAttachments()
{
    Q_ASSERT(selectedParts.count() >= 1);
    for (const AttachmentPart::Ptr &part : qAsConst(selectedParts)) {
        q->viewAttachment(part);
    }
}

void AttachmentControllerBase::Private::editSelectedAttachment()
{
    Q_ASSERT(selectedParts.count() == 1);
    q->editAttachment(selectedParts.first(), MessageViewer::EditorWatcher::NoOpenWithDialog);
}

void AttachmentControllerBase::Private::editSelectedAttachmentWith()
{
    Q_ASSERT(selectedParts.count() == 1);
    q->editAttachment(selectedParts.first(), MessageViewer::EditorWatcher::OpenWithDialog);
}

void AttachmentControllerBase::Private::removeSelectedAttachments()
{
    Q_ASSERT(selectedParts.count() >= 1);
    //We must store list, otherwise when we remove it changes selectedParts (as selection changed) => it will crash.
    const AttachmentPart::List toRemove = selectedParts;
    for (const AttachmentPart::Ptr &part : toRemove) {
        model->removeAttachment(part);
    }
}

void AttachmentControllerBase::Private::saveSelectedAttachmentAs()
{
    Q_ASSERT(selectedParts.count() == 1);
    q->saveAttachmentAs(selectedParts.first());
}

void AttachmentControllerBase::Private::selectedAttachmentProperties()
{
    Q_ASSERT(selectedParts.count() == 1);
    q->attachmentProperties(selectedParts.first());
}

void AttachmentControllerBase::Private::reloadAttachment()
{
    Q_ASSERT(selectedParts.count() == 1);
    AttachmentUpdateJob *ajob = new AttachmentUpdateJob(selectedParts.first(), q);
    connect(ajob, &AttachmentUpdateJob::result, q, [this](KJob *job) {
        updateJobResult(job);
    });
    ajob->start();
}

void AttachmentControllerBase::Private::updateJobResult(KJob *job)
{
    if (job->error()) {
        KMessageBox::sorry(wParent, job->errorString(), i18n("Failed to reload attachment"));
        return;
    }
    Q_ASSERT(dynamic_cast<AttachmentUpdateJob *>(job));
    AttachmentUpdateJob *ajob = static_cast<AttachmentUpdateJob *>(job);
    AttachmentPart::Ptr originalPart = ajob->originalPart();
    AttachmentPart::Ptr updatedPart = ajob->updatedPart();

    attachmentRemoved(originalPart);
    bool ok = model->replaceAttachment(originalPart, updatedPart);
    if (!ok) {
        // The attachment was removed from the model while we were compressing.
        qCDebug(MESSAGECOMPOSER_LOG) << "Updated a zombie.";
    }
}

void AttachmentControllerBase::Private::editDone(MessageViewer::EditorWatcher *watcher)
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

void AttachmentControllerBase::Private::createOpenWithMenu(QMenu *topMenu, const AttachmentPart::Ptr &part)
{
    const QString contentTypeStr = QString::fromLatin1(part->mimeType());
    const KService::List offers = KFileItemActions::associatedApplications(QStringList() << contentTypeStr, QString());
    if (!offers.isEmpty()) {
        QMenu *menu = topMenu;
        QActionGroup *actionGroup = new QActionGroup(menu);
        connect(actionGroup, &QActionGroup::triggered, q, &AttachmentControllerBase::slotOpenWithAction);

        if (offers.count() > 1) { // submenu 'open with'
            menu = new QMenu(i18nc("@title:menu", "&Open With"), topMenu);
            menu->menuAction()->setObjectName(QStringLiteral("openWith_submenu")); // for the unittest
            topMenu->addMenu(menu);
        }
        //qCDebug(MESSAGECOMPOSER_LOG) << offers.count() << "offers" << topMenu << menu;

        KService::List::ConstIterator it = offers.constBegin();
        KService::List::ConstIterator end = offers.constEnd();
        for (; it != end; ++it) {
            QAction *act = MessageViewer::Util::createAppAction(*it,
                                                                // no submenu -> prefix single offer
                                                                menu == topMenu, actionGroup, menu);
            menu->addAction(act);
        }

        QString openWithActionName;
        if (menu != topMenu) { // submenu
            menu->addSeparator();
            openWithActionName = i18nc("@action:inmenu Open With", "&Other...");
        } else {
            openWithActionName = i18nc("@title:menu", "&Open With...");
        }
        QAction *openWithAct = new QAction(menu);
        openWithAct->setText(openWithActionName);
        QObject::connect(openWithAct, &QAction::triggered, q, &AttachmentControllerBase::slotOpenWithDialog);
        menu->addAction(openWithAct);
    } else { // no app offers -> Open With...
        QAction *act = new QAction(topMenu);
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

    MessageComposer::AttachmentFromPublicKeyJob *ajob = new MessageComposer::AttachmentFromPublicKeyJob(fingerprint, this);
    connect(ajob, &AttachmentFromPublicKeyJob::result, this, [this](KJob *job) {
        d->attachPublicKeyJobResult(job);
    });
    ajob->start();
}

void AttachmentControllerBase::Private::attachPublicKeyJobResult(KJob *job)
{
    // The only reason we can't use loadJobResult() and need a separate method
    // is that we want to show the proper caption ("public key" instead of "file")...

    if (job->error()) {
        KMessageBox::sorry(wParent, job->errorString(), i18n("Failed to attach public key"));
        return;
    }

    Q_ASSERT(dynamic_cast<MessageComposer::AttachmentFromPublicKeyJob *>(job));
    MessageComposer::AttachmentFromPublicKeyJob *ajob = static_cast<MessageComposer::AttachmentFromPublicKeyJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

void AttachmentControllerBase::Private::attachVcardFromAddressBook(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << " Error during when get vCard";
        KMessageBox::sorry(wParent, job->errorString(), i18n("Failed to attach vCard"));
        return;
    }

    MessageComposer::AttachmentVcardFromAddressBookJob *ajob = static_cast<MessageComposer::AttachmentVcardFromAddressBookJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

void AttachmentControllerBase::Private::attachClipBoardElement(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << " Error during when get try to attach text from clipboard";
        KMessageBox::sorry(wParent, job->errorString(), i18n("Failed to attach text from clipboard"));
        return;
    }

    MessageComposer::AttachmentClipBoardJob *ajob = static_cast<MessageComposer::AttachmentClipBoardJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

static QTemporaryFile *dumpAttachmentToTempFile(const AttachmentPart::Ptr &part)   // local
{
    QTemporaryFile *file = new QTemporaryFile;
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
    , d(new Private(this))
{
    d->model = model;
    connect(model, &MessageComposer::AttachmentModel::attachUrlsRequested, this, &AttachmentControllerBase::addAttachments);
    connect(model, &MessageComposer::AttachmentModel::attachmentRemoved,
            this, [this](const MessageCore::AttachmentPart::Ptr &attr) {
        d->attachmentRemoved(attr);
    });
    connect(model, &AttachmentModel::attachmentCompressRequested,
            this, &AttachmentControllerBase::compressAttachment);
    connect(model, &MessageComposer::AttachmentModel::encryptEnabled, this, &AttachmentControllerBase::setEncryptEnabled);
    connect(model, &MessageComposer::AttachmentModel::signEnabled, this, &AttachmentControllerBase::setSignEnabled);

    d->wParent = wParent;
    d->mActionCollection = actionCollection;
}

AttachmentControllerBase::~AttachmentControllerBase()
{
    delete d;
}

void AttachmentControllerBase::createActions()
{
    // Create the actions.
    d->attachPublicKeyAction = new QAction(i18n("Attach &Public Key..."), this);
    connect(d->attachPublicKeyAction, &QAction::triggered,
            this, &AttachmentControllerBase::showAttachPublicKeyDialog);

    d->attachMyPublicKeyAction = new QAction(i18n("Attach &My Public Key"), this);
    connect(d->attachMyPublicKeyAction, &QAction::triggered, this, &AttachmentControllerBase::attachMyPublicKey);

    d->attachmentMenu = new KActionMenu(QIcon::fromTheme(QStringLiteral("mail-attachment")), i18n("Attach"), this);
    connect(d->attachmentMenu, &QAction::triggered, this, &AttachmentControllerBase::showAddAttachmentFileDialog);

    d->attachmentMenu->setDelayed(true);

    d->addAttachmentFileAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-attachment")), i18n("&Attach File..."), this);
    d->addAttachmentFileAction->setIconText(i18n("Attach"));
    d->addContextAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-attachment")),
                                      i18n("Add Attachment..."), this);
    connect(d->addAttachmentFileAction, &QAction::triggered, this, &AttachmentControllerBase::showAddAttachmentFileDialog);
    connect(d->addContextAction, &QAction::triggered, this, &AttachmentControllerBase::showAddAttachmentFileDialog);

    d->addAttachmentDirectoryAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-attachment")), i18n("&Attach Directory..."), this);
    d->addAttachmentDirectoryAction->setIconText(i18n("Attach"));
    connect(d->addAttachmentDirectoryAction, &QAction::triggered, this, &AttachmentControllerBase::showAddAttachmentCompressedDirectoryDialog);

    d->addOwnVcardAction = new QAction(i18n("Attach Own vCard"), this);
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
    d->removeContextAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove"), this);     // FIXME need two texts. is there a better way?
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

    d->editWithContextAction = new QAction(i18n("Edit With..."), this);
    connect(d->editWithContextAction, &QAction::triggered, this, [this]() {
        d->editSelectedAttachmentWith();
    });

    d->saveAsAction = new QAction(QIcon::fromTheme(QStringLiteral("document-save-as")),
                                  i18n("&Save Attachment As..."), this);
    d->saveAsContextAction = new QAction(QIcon::fromTheme(QStringLiteral("document-save-as")),
                                         i18n("Save As..."), this);
    connect(d->saveAsAction, &QAction::triggered,
            this, [this]() {
        d->saveSelectedAttachmentAs();
    });
    connect(d->saveAsContextAction, &QAction::triggered,
            this, [this]() {
        d->saveSelectedAttachmentAs();
    });

    d->propertiesAction = new QAction(i18n("Attachment Pr&operties..."), this);
    d->propertiesContextAction = new QAction(i18n("Properties"), this);
    connect(d->propertiesAction, &QAction::triggered,
            this, [this]() {
        d->selectedAttachmentProperties();
    });
    connect(d->propertiesContextAction, &QAction::triggered,
            this, [this]() {
        d->selectedAttachmentProperties();
    });

    d->selectAllAction = new QAction(i18n("Select All"), this);
    connect(d->selectAllAction, &QAction::triggered,
            this, &AttachmentControllerBase::selectedAllAttachment);

    d->reloadAttachmentAction = new QAction(QIcon::fromTheme(QStringLiteral("view-refresh")), i18n("Reload"), this);
    connect(d->reloadAttachmentAction, &QAction::triggered,
            this, [this]() {
        d->reloadAttachment();
    });

    // Insert the actions into the composer window's menu.
    KActionCollection *collection = d->mActionCollection;
    collection->addAction(QStringLiteral("attach_public_key"), d->attachPublicKeyAction);
    collection->addAction(QStringLiteral("attach_my_public_key"), d->attachMyPublicKeyAction);
    collection->addAction(QStringLiteral("attach"), d->addAttachmentFileAction);
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

        AttachmentCompressJob *ajob = new AttachmentCompressJob(part, this);
        connect(ajob, &AttachmentCompressJob::result, this, [this](KJob *job) {
            d->compressJobResult(job);
        });
        ajob->start();
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "Uncompressing part.";

        // Replace the compressed part with the original uncompressed part, and delete
        // the compressed part.
        AttachmentPart::Ptr originalPart = d->uncompressedParts.take(part);
        Q_ASSERT(originalPart);   // Found in uncompressedParts.
        bool ok = d->model->replaceAttachment(part, originalPart);
        Q_ASSERT(ok);
        Q_UNUSED(ok);
    }
}

void AttachmentControllerBase::showContextMenu()
{
    Q_EMIT refreshSelection();

    const int numberOfParts(d->selectedParts.count());
    QMenu menu;

    const bool enableEditAction = (numberOfParts == 1)
                                  && (!d->selectedParts.first()->isMessageOrMessageCollection());

    if (numberOfParts > 0) {
        if (numberOfParts == 1) {
            const QString mimetype = QString::fromLatin1(d->selectedParts.first()->mimeType());
            QMimeDatabase mimeDb;
            auto mime = mimeDb.mimeTypeForName(mimetype);
            QStringList parentMimeType;
            if (mime.isValid()) {
                parentMimeType = mime.allAncestors();
            }
            if ((mimetype == QLatin1String("text/plain"))
                || (mimetype == QLatin1String("image/png"))
                || (mimetype == QLatin1String("image/jpeg"))
                || parentMimeType.contains(QLatin1String("text/plain"))
                || parentMimeType.contains(QLatin1String("image/png"))
                || parentMimeType.contains(QLatin1String("image/jpeg"))
                ) {
                menu.addAction(d->viewContextAction);
            }
            d->createOpenWithMenu(&menu, d->selectedParts.first());
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
    KService::Ptr app = act->data().value<KService::Ptr>();
    Q_ASSERT(d->selectedParts.count() == 1);

    openWith(app);
}

void AttachmentControllerBase::openWith(const KService::Ptr &offer)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(d->selectedParts.first());
    if (!tempFile) {
        KMessageBox::sorry(d->wParent,
                           i18n("KMail was unable to write the attachment to a temporary file."),
                           i18n("Unable to open attachment"));
        return;
    }
    QList<QUrl> lst;
    QUrl url = QUrl::fromLocalFile(tempFile->fileName());
    lst.append(url);
    tempFile->setPermissions(QFile::ReadUser);
    bool result = false;
    if (offer) {
        result = KRun::runService(*offer, lst, d->wParent, false);
    } else {
        result = KRun::displayOpenWithDialog(lst, d->wParent, false);
    }
    if (!result) {
        delete tempFile;
        tempFile = nullptr;
    } else {
        // The file was opened.  Delete it only when the composer is closed
        // (and this object is destroyed).
        tempFile->setParent(this);   // Manages lifetime.
    }
}

void AttachmentControllerBase::openAttachment(const AttachmentPart::Ptr &part)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(part);
    if (!tempFile) {
        KMessageBox::sorry(d->wParent,
                           i18n("KMail was unable to write the attachment to a temporary file."),
                           i18n("Unable to open attachment"));
        return;
    }
    tempFile->setPermissions(QFile::ReadUser);
#if KIO_VERSION < QT_VERSION_CHECK(5, 71, 0)
    KRun::RunFlags flags;
    flags |= KRun::DeleteTemporaryFiles;
    bool success = KRun::runUrl(QUrl::fromLocalFile(tempFile->fileName()),
                                QString::fromLatin1(part->mimeType()),
                                d->wParent,
                                flags);
    if (!success) {
        if (!KMimeTypeTrader::self()->preferredService(QString::fromLatin1(part->mimeType())).data()) {
            // KRun showed an Open-With dialog, and it was canceled.
        } else {
            // KRun failed.
            KMessageBox::sorry(d->wParent,
                               i18n("KMail was unable to open the attachment."),
                               i18n("Unable to open attachment"));
        }
        delete tempFile;
        tempFile = nullptr;
    } else {
        // The file was opened.  Delete it only when the composer is closed
        // (and this object is destroyed).
        tempFile->setParent(this);   // Manages lifetime.
    }
#else
    KIO::OpenUrlJob *job = new KIO::OpenUrlJob(QUrl::fromLocalFile(tempFile->fileName()), QString::fromLatin1(part->mimeType()));
    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, d->wParent));
    job->setDeleteTemporaryFile(true);
    connect(job, &KIO::OpenUrlJob::result, this, [this, tempFile](KJob *job) {
        if (job->error() == KIO::ERR_USER_CANCELED) {
            KMessageBox::sorry(d->wParent,
                               i18n("KMail was unable to open the attachment."),
                               job->errorString());
            delete tempFile;
        } else {
            // The file was opened.  Delete it only when the composer is closed
            // (and this object is destroyed).
            tempFile->setParent(this);   // Manages lifetime.
        }
    });
    job->start();
#endif
}

void AttachmentControllerBase::viewAttachment(const AttachmentPart::Ptr &part)
{
    MessageComposer::Composer *composer = new MessageComposer::Composer;
    composer->globalPart()->setFallbackCharsetEnabled(true);
    MessageComposer::AttachmentJob *attachmentJob = new MessageComposer::AttachmentJob(part, composer);
    connect(attachmentJob, &AttachmentJob::result, this, [this](KJob *job) {
        d->slotAttachmentContentCreated(job);
    });
    attachmentJob->start();
}

void AttachmentControllerBase::Private::slotAttachmentContentCreated(KJob *job)
{
    if (!job->error()) {
        const MessageComposer::AttachmentJob *const attachmentJob = dynamic_cast<MessageComposer::AttachmentJob *>(job);
        Q_ASSERT(attachmentJob);
        if (attachmentJob) {
            Q_EMIT q->showAttachment(attachmentJob->content(), QByteArray());
        }
    } else {
        // TODO: show warning to the user
        qCWarning(MESSAGECOMPOSER_LOG) << "Error creating KMime::Content for attachment:" << job->errorText();
    }
}

void AttachmentControllerBase::editAttachment(AttachmentPart::Ptr part, MessageViewer::EditorWatcher::OpenWithOption openWithOption)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(part);
    if (!tempFile) {
        KMessageBox::sorry(d->wParent,
                           i18n("KMail was unable to write the attachment to a temporary file."),
                           i18n("Unable to edit attachment"));
        return;
    }

    MessageViewer::EditorWatcher *watcher = new MessageViewer::EditorWatcher(
        QUrl::fromLocalFile(tempFile->fileName()),
        QString::fromLatin1(part->mimeType()), openWithOption,
        this, d->wParent);
    connect(watcher, &MessageViewer::EditorWatcher::editDone,
            this, [this](MessageViewer::EditorWatcher *watcher)  {
        d->editDone(watcher);
    });

    switch (watcher->start()) {
    case MessageViewer::EditorWatcher::NoError:
        // The attachment is being edited.
        // We will clean things up in editDone().
        d->editorPart[ watcher ] = part;
        d->editorTempFile[ watcher ] = tempFile;

        // Delete the temp file if the composer is closed (and this object is destroyed).
        tempFile->setParent(this);   // Manages lifetime.
        break;
    case MessageViewer::EditorWatcher::CannotStart:
        qCWarning(MESSAGECOMPOSER_LOG) << "Could not start EditorWatcher.";
        Q_FALLTHROUGH();
    case MessageViewer::EditorWatcher::Unknown:
    case MessageViewer::EditorWatcher::Canceled:
    case MessageViewer::EditorWatcher::NoServiceFound:
        delete watcher;
        delete tempFile;
        break;
    }
}

void AttachmentControllerBase::editAttachmentWith(const AttachmentPart::Ptr &part)
{
    editAttachment(part, MessageViewer::EditorWatcher::OpenWithDialog);
}

void AttachmentControllerBase::saveAttachmentAs(const AttachmentPart::Ptr &part)
{
    QString pname = part->name();
    if (pname.isEmpty()) {
        pname = i18n("unnamed");
    }

    QUrl url = QFileDialog::getSaveFileUrl(d->wParent, i18n("Save Attachment As"), QUrl::fromLocalFile(pname));

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
    KIO::StoredTransferJob *_job = qobject_cast<KIO::StoredTransferJob *>(job);

    if (job->error()) {
        if (job->error() == KIO::ERR_FILE_ALREADY_EXIST) {
            if (KMessageBox::warningContinueCancel(nullptr,
                                                   i18n("File %1 exists.\nDo you want to replace it?", _job->url().toLocalFile()), i18n("Save to File"), KGuiItem(i18n("&Replace")))
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
    QPointer<AttachmentPropertiesDialog> dialog = new AttachmentPropertiesDialog(
        part, false, d->wParent);

    dialog->setEncryptEnabled(d->encryptEnabled);
    dialog->setSignEnabled(d->signEnabled);

    if (dialog->exec() && dialog) {
        d->model->updateAttachment(part);
    }
    delete dialog;
}

void AttachmentControllerBase::attachDirectory(const QUrl &url)
{
    const int rc = KMessageBox::warningYesNo(d->wParent, i18n("Do you really want to attach this directory \"%1\"?", url.toLocalFile()),
                                             i18nc("@title:window", "Attach directory"));
    if (rc == KMessageBox::Yes) {
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
            MessageComposer::AttachmentVcardFromAddressBookJob *ajob = new MessageComposer::AttachmentVcardFromAddressBookJob(selected.item(), this);
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
    MessageComposer::AttachmentClipBoardJob *job = new MessageComposer::AttachmentClipBoardJob(this);
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
    KEncodingFileDialog::Result result = KEncodingFileDialog::getOpenUrlsAndEncoding(QString(),
                                                                                     QUrl(),
                                                                                     QString(),
                                                                                     d->wParent,
                                                                                     i18nc("@title:window", "Attach File"));
    if (!result.URLs.isEmpty()) {
        const QString encoding = MimeTreeParser::NodeHelper::fixEncoding(result.encoding);
        const int numberOfFiles(result.URLs.count());
        for (int i = 0; i < numberOfFiles; ++i) {
            const QUrl url = result.URLs.at(i);
            QUrl urlWithEncoding = url;
            MessageCore::StringUtil::setEncodingFile(urlWithEncoding, encoding);
            QMimeDatabase mimeDb;
            auto mimeType = mimeDb.mimeTypeForUrl(urlWithEncoding);
            if (mimeType.name() == QLatin1String("inode/directory")) {
                const int rc = KMessageBox::warningYesNo(d->wParent, i18n("Do you really want to attach this directory \"%1\"?", url.toLocalFile()),
                                                         i18nc("@title:window", "Attach directory"));
                if (rc == KMessageBox::Yes) {
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
            KMessageBox::sorry(d->wParent, ajob->errorString(), i18nc("@title:window", "Failed to attach file"));
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
    QPointer<KeySelectionDialog> dialog = new KeySelectionDialog(
        i18n("Attach Public OpenPGP Key"),
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
