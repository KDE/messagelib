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
#include "abstractattachmentcontrollerbase_p.h"

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

#include <AkonadiCore/itemfetchjob.h>
#include <QIcon>
#include <kio/jobuidelegate.h>

#include <QMenu>
#include <QPointer>
#include <QTreeView>

#include "messagecomposer_debug.h"
#include <KActionCollection>
#include <KActionMenu>
#include <KEncodingFileDialog>
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

#include "attachment/attachmentfrommimecontentjob.h"
#include "attachment/attachmentfromurljob.h"
#include "messagecore/attachmentupdatejob.h"
#include <Akonadi/Contact/EmailAddressSelectionDialog>
#include <Akonadi/Contact/EmailAddressSelectionWidget>
#include <KIO/Job>
#include <KIO/JobUiDelegate>
#include <KIO/OpenUrlJob>
#include <MessageCore/AttachmentCompressJob>
#include <MessageCore/AttachmentFromUrlUtils>
#include <MessageCore/AttachmentPropertiesDialog>
#include <settings/messagecomposersettings.h>

#include <KMime/Content>
#include <QFileDialog>

using namespace MessageComposer;
using namespace MessageCore;

class Q_DECL_HIDDEN MessageComposer::AttachmentControllerBase::Private : public MessageComposer::AbstractAttachmentControllerBase::Private
{
public:
    Private(AttachmentControllerBase *qq);
    ~Private();

    void editSelectedAttachment(); // slot
    void editSelectedAttachmentWith(); // slot
    void saveSelectedAttachmentAs(); // slot
    void selectedAttachmentProperties(); // slot
    void createOpenWithMenu(QMenu *topMenu, const AttachmentPart::Ptr &part);
    void editDone(MessageViewer::EditorWatcher *watcher); // slot

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
};

AttachmentControllerBase::Private::Private(AttachmentControllerBase *qq)
    : AbstractAttachmentControllerBase::Private(qq)
{
}

AttachmentControllerBase::Private::~Private()
{
}

void AttachmentControllerBase::setSelectedParts(const AttachmentPart::List &selectedParts)
{
    AbstractAttachmentControllerBase::setSelectedParts(selectedParts);
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

void AttachmentControllerBase::Private::editSelectedAttachment()
{
    Q_ASSERT(selectedParts.count() == 1);
    static_cast<AttachmentControllerBase *>(q)->editAttachment(selectedParts.constFirst(), MessageViewer::EditorWatcher::NoOpenWithDialog);
}

void AttachmentControllerBase::Private::editSelectedAttachmentWith()
{
    Q_ASSERT(selectedParts.count() == 1);
    static_cast<AttachmentControllerBase *>(q)->editAttachment(selectedParts.constFirst(), MessageViewer::EditorWatcher::OpenWithDialog);
}

void AttachmentControllerBase::Private::saveSelectedAttachmentAs()
{
    Q_ASSERT(selectedParts.count() == 1);
    static_cast<AttachmentControllerBase *>(q)->saveAttachmentAs(selectedParts.constFirst());
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

void AttachmentControllerBase::Private::selectedAttachmentProperties()
{
    Q_ASSERT(selectedParts.count() == 1);
    static_cast<AttachmentControllerBase *>(q)->attachmentProperties(selectedParts.constFirst());
}

void AttachmentControllerBase::Private::createOpenWithMenu(QMenu *topMenu, const AttachmentPart::Ptr &part)
{
    const QString contentTypeStr = QString::fromLatin1(part->mimeType());
    const KService::List offers = KFileItemActions::associatedApplications(QStringList() << contentTypeStr, QString());
    if (!offers.isEmpty()) {
        QMenu *menu = topMenu;
        auto actionGroup = new QActionGroup(menu);
        connect(actionGroup, &QActionGroup::triggered, static_cast<AttachmentControllerBase *>(q), &AttachmentControllerBase::slotOpenWithAction);

        if (offers.count() > 1) { // submenu 'open with'
            menu = new QMenu(i18nc("@title:menu", "&Open With"), topMenu);
            menu->menuAction()->setObjectName(QStringLiteral("openWith_submenu")); // for the unittest
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
        QObject::connect(openWithAct, &QAction::triggered, static_cast<AttachmentControllerBase *>(q), &AttachmentControllerBase::slotOpenWithDialog);
        menu->addAction(openWithAct);
    } else { // no app offers -> Open With...
        auto act = new QAction(topMenu);
        act->setText(i18nc("@title:menu", "&Open With..."));
        QObject::connect(act, &QAction::triggered, static_cast<AttachmentControllerBase *>(q), &AttachmentControllerBase::slotOpenWithDialog);
        topMenu->addAction(act);
    }
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
    : AbstractAttachmentControllerBase(model, wParent)
    , d(new Private(this))
{
    d->model = model;

    // Add UI for handling cases where compressed attachments are larger than original.
    connect(this, &AttachmentControllerBase::compressedAttachmentLargerOriginalOccured, this, [this, wParent](KJob *job) {
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
        overwriteAttachment(job);
    });

    // Add UI for handling error
    connect(this, &AttachmentControllerBase::errorOccured, this, [wParent](const QString &text, const QString &caption) {
        KMessageBox::sorry(wParent, text, caption);
    });

    connect(this, &AttachmentControllerBase::canAttachPublicKeyChanged, this, [this]() {
        d->attachPublicKeyAction->setEnabled(canAttachPublicKey());
    });

    connect(this, &AttachmentControllerBase::attachOwnVcardChanged, this, [this]() {
        d->addOwnVcardAction->setChecked(attachOwnVcard());
    });

    connect(d->addOwnVcardAction, &QAction::toggled, this, [this](bool checked) {
        setAttachOwnVcard(checked);
    });

    connect(this, &AttachmentControllerBase::identityHasOwnVcardChanged, this, [this]() {
        d->addOwnVcardAction->setEnabled(identityHasOwnVcard());
    });

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
    connect(d->attachPublicKeyAction, &QAction::triggered, this, &AttachmentControllerBase::showAttachPublicKeyDialog);

    d->attachMyPublicKeyAction = new QAction(i18n("Attach &My Public Key"), this);
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

    d->editWithContextAction = new QAction(i18n("Edit With..."), this);
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

    d->propertiesAction = new QAction(i18n("Attachment Pr&operties..."), this);
    d->propertiesContextAction = new QAction(i18n("Properties"), this);
    connect(d->propertiesAction, &QAction::triggered, this, [this]() {
        d->selectedAttachmentProperties();
    });
    connect(d->propertiesContextAction, &QAction::triggered, this, [this]() {
        d->selectedAttachmentProperties();
    });

    d->selectAllAction = new QAction(i18n("Select All"), this);
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
            if ((mimetype == QLatin1String("text/plain")) || (mimetype == QLatin1String("image/png")) || (mimetype == QLatin1String("image/jpeg"))
                || parentMimeType.contains(QLatin1String("text/plain")) || parentMimeType.contains(QLatin1String("image/png"))
                || parentMimeType.contains(QLatin1String("image/jpeg"))) {
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

void AttachmentControllerBase::slotOpenWithAction(QAction *act)
{
    KService::Ptr app = act->data().value<KService::Ptr>();
    Q_ASSERT(d->selectedParts.count() == 1);

    openWith(app);
}

void AttachmentControllerBase::openWith(const KService::Ptr &offer)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(d->selectedParts.constFirst());
    if (!tempFile) {
        KMessageBox::sorry(d->wParent, i18n("KMail was unable to write the attachment to a temporary file."), i18n("Unable to open attachment"));
        return;
    }
    QUrl url = QUrl::fromLocalFile(tempFile->fileName());
    tempFile->setPermissions(QFile::ReadUser);
    // If offer is null, this will show the "open with" dialog
    auto job = new KIO::ApplicationLauncherJob(offer);
    job->setUrls({url});
    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, d->wParent));
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
        KMessageBox::sorry(d->wParent, i18n("KMail was unable to write the attachment to a temporary file."), i18n("Unable to open attachment"));
        return;
    }
    tempFile->setPermissions(QFile::ReadUser);
    auto job = new KIO::OpenUrlJob(QUrl::fromLocalFile(tempFile->fileName()), QString::fromLatin1(part->mimeType()));
    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, d->wParent));
    job->setDeleteTemporaryFile(true);
    connect(job, &KIO::OpenUrlJob::result, this, [this, tempFile](KJob *job) {
        if (job->error() == KIO::ERR_USER_CANCELED) {
            KMessageBox::sorry(d->wParent, i18n("KMail was unable to open the attachment."), job->errorString());
            delete tempFile;
        } else {
            // The file was opened.  Delete it only when the composer is closed
            // (and this object is destroyed).
            tempFile->setParent(this); // Manages lifetime.
        }
    });
    job->start();
}

void AttachmentControllerBase::editAttachment(AttachmentPart::Ptr part, MessageViewer::EditorWatcher::OpenWithOption openWithOption)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(part);
    if (!tempFile) {
        KMessageBox::sorry(d->wParent, i18n("KMail was unable to write the attachment to a temporary file."), i18n("Unable to edit attachment"));
        return;
    }

    auto watcher =
        new MessageViewer::EditorWatcher(QUrl::fromLocalFile(tempFile->fileName()), QString::fromLatin1(part->mimeType()), openWithOption, this, d->wParent);
    connect(watcher, &MessageViewer::EditorWatcher::editDone, this, [this](MessageViewer::EditorWatcher *watcher) {
        d->editDone(watcher);
    });

    switch (watcher->start()) {
    case MessageViewer::EditorWatcher::NoError:
        // The attachment is being edited.
        // We will clean things up in editDone().
        d->editorPart[watcher] = part;
        d->editorTempFile[watcher] = tempFile;

        // Delete the temp file if the composer is closed (and this object is destroyed).
        tempFile->setParent(this); // Manages lifetime.
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
                                                   i18n("Save to File"),
                                                   KGuiItem(i18n("&Replace")))
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
    const int rc = KMessageBox::warningYesNo(d->wParent,
                                             i18n("Do you really want to attach this directory \"%1\"?", url.toLocalFile()),
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
    const KEncodingFileDialog::Result result =
        KEncodingFileDialog::getOpenUrlsAndEncoding(QString(), QUrl(), QString(), d->wParent, i18nc("@title:window", "Attach File"));
    if (!result.URLs.isEmpty()) {
        const QString encoding = MimeTreeParser::NodeHelper::fixEncoding(result.encoding);
        const int numberOfFiles(result.URLs.count());
        for (int i = 0; i < numberOfFiles; ++i) {
            const QUrl url = result.URLs.at(i);
            QUrl urlWithEncoding = url;
            MessageCore::StringUtil::setEncodingFile(urlWithEncoding, encoding);
            QMimeDatabase mimeDb;
            const auto mimeType = mimeDb.mimeTypeForUrl(urlWithEncoding);
            if (mimeType.name() == QLatin1String("inode/directory")) {
                const int rc = KMessageBox::warningYesNo(d->wParent,
                                                         i18n("Do you really want to attach this directory \"%1\"?", url.toLocalFile()),
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
