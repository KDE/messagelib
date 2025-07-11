/*
  SPDX-FileCopyrightText: 1997 Markus Wuebben <markus.wuebben@kde.org>
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>
  SPDX-FileCopyrightText: 2010 Torgny Nyblom <nyblom@kde.org>
  SPDX-FileCopyrightText: 2011-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "viewer_p.h"
#include "printmessage.h"
#include "viewerpurposemenuwidget.h"

#include "mdn/mdnwarningwidget.h"
#include "messagedisplayformatattribute.h"
#include "messageviewer_debug.h"
#include "scamdetection/scamattribute.h"
#include "scamdetection/scamdetectionwarningwidget.h"
#include "utils/mimetype.h"
#include "viewer/mimeparttree/mimeparttreeview.h"
#include "viewer/objecttreeemptysource.h"
#include "viewer/objecttreeviewersource.h"
#include "viewer/renderer/messageviewerrenderer.h"

#include "messageviewer/headerstrategy.h"
#include "messageviewer/headerstyle.h"
#include "openurlwith/openurlwithmanager.h"
#include <TextAddonsWidgets/SlideContainer>

#include "job/modifymessagedisplayformatjob.h"

#include "htmlwriter/webengineembedpart.h"
#include "viewerplugins/viewerplugintoolmanager.h"
#include <KContacts/VCardConverter>

#include <KActionCollection>
#include <KActionMenu>
#include <KStandardActions>
#include <QAction>
#include <QHBoxLayout>
#include <QPrintPreviewDialog>
#include <QVBoxLayout>

#include <Akonadi/ErrorAttribute>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemModifyJob>
#include <Akonadi/MessageFlags>
#include <Akonadi/SpecialMailCollections>
#include <KApplicationTrader>
#include <KEmailAddress>
#include <KFileItemActions>
#include <KIO/ApplicationLauncherJob>
#include <KIO/JobUiDelegateFactory>
#include <KIO/OpenUrlJob>
#include <KLocalizedString>
#include <KMessageBox>
#include <KMimeTypeChooser>
#include <KSelectAction>
#include <KSharedConfig>
#include <KStandardGuiItem>
#include <KToggleAction>
#include <MessageCore/Util>
#include <QIcon>
#include <QKeyCombination>
#include <QMenu>
#include <QMimeData>
#include <QTemporaryDir>

// Qt includes
#include <QActionGroup>
#include <QClipboard>
#include <QItemSelectionModel>
#include <QMimeDatabase>
#include <QPrintDialog>
#include <QPrinter>
#include <QSplitter>
#include <QTreeView>
#include <QWheelEvent>
// libkdepim
#include <MessageCore/AttachmentPropertiesDialog>
#include <PimCommon/BroadcastStatus>

#include <Akonadi/AttributeFactory>
#include <Akonadi/Collection>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/MessageParts>
#include <Akonadi/MessageStatus>

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <MessageCore/AutocryptUtils>

// own includes
#include "messageviewer/messageviewerutil.h"
#include "openurlwith/openurlwithjob.h"
#include "settings/messageviewersettings.h"
#include "utils/messageviewerutil_p.h"
#include "viewer/attachmentstrategy.h"
#include "viewer/mimeparttree/mimetreemodel.h"
#include "viewer/urlhandlermanager.h"
#include "widgets/attachmentdialog.h"
#include "widgets/htmlstatusbar.h"
#include "widgets/shownextmessagewidget.h"

#include "header/headerstylemenumanager.h"
#include "htmlwriter/webengineparthtmlwriter.h"
#include "viewer/webengine/mailwebengineview.h"
#include "widgets/mailsourcewebengineviewer.h"
#include <WebEngineViewer/FindBarWebEngineView>
#include <WebEngineViewer/LocalDataBaseManager>
#include <WebEngineViewer/SubmittedFormWarningWidget>
#include <WebEngineViewer/WebEngineExportPdfPageJob>
#include <WebEngineViewer/WebHitTestResult>

#include "interfaces/htmlwriter.h"
#include <MimeTreeParser/BodyPart>
#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/ObjectTreeParser>

#include <MessageCore/StringUtil>

#include <MessageCore/MessageCoreSettings>

#include <Akonadi/AgentInstance>
#include <Akonadi/AgentManager>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>

#include <PimCommon/PurposeMenuMessageWidget>

#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
#include <TextEditTextToSpeech/TextToSpeechContainerWidget>
#endif
#include "header/headerstyleplugin.h"
#include "viewerplugins/viewerplugininterface.h"
#include <Akonadi/MDNStateAttribute>
#include <QApplication>
#include <QStandardPaths>
#include <QWebEngineSettings>
#include <WebEngineViewer/DeveloperToolDialog>
#include <WebEngineViewer/TrackingWarningWidget>
#include <WebEngineViewer/ZoomActionMenu>

#include <GrantleeTheme/GrantleeTheme>
#include <GrantleeTheme/GrantleeThemeManager>

#include "dkim-verify/dkimmanager.h"
#include "dkim-verify/dkimmanagerulesdialog.h"
#include "dkim-verify/dkimresultattribute.h"
#include "dkim-verify/dkimviewermenu.h"
#include "dkim-verify/dkimwidgetinfo.h"

#include "remote-content/remotecontentmenu.h"
#include <chrono>

using namespace std::chrono_literals;
using namespace MessageViewer;
using namespace MessageCore;
using namespace Qt::Literals::StringLiterals;

static QAtomicInt _k_attributeInitialized;

template<typename Arg, typename R, typename C>
struct InvokeWrapper {
    R *receiver;
    void (C::*memberFun)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFun)(result);
    }
};

template<typename Arg, typename R, typename C>
static InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
    return wrapper;
}

ViewerPrivate::ViewerPrivate(Viewer *aParent, QWidget *mainWindow, KActionCollection *actionCollection)
    : QObject(aParent)
    , mNodeHelper(new MimeTreeParser::NodeHelper)
    , mOldGlobalOverrideEncoding(u"---"_s)
    , mMainWindow(mainWindow)
    , mActionCollection(actionCollection)
    , q(aParent)
    , mSession(new Akonadi::Session("MessageViewer-" + QByteArray::number(reinterpret_cast<quintptr>(this)), this))
{
    if (!mainWindow) {
        mMainWindow = aParent;
    }
    mMessageViewerRenderer = new MessageViewerRenderer;

    mRemoteContentMenu = new MessageViewer::RemoteContentMenu(mMainWindow);
    connect(mRemoteContentMenu, &MessageViewer::RemoteContentMenu::updateEmail, this, &ViewerPrivate::updateReaderWin);

    mDkimWidgetInfo = new MessageViewer::DKIMWidgetInfo(mMainWindow);
    if (_k_attributeInitialized.testAndSetAcquire(0, 1)) {
        Akonadi::AttributeFactory::registerAttribute<MessageViewer::MessageDisplayFormatAttribute>();
        Akonadi::AttributeFactory::registerAttribute<MessageViewer::ScamAttribute>();
    }
    mPhishingDatabase = new WebEngineViewer::LocalDataBaseManager(this);
    if (MessageViewer::MessageViewerSettings::self()->checkPhishingUrl()) {
        mPhishingDatabase->initialize();
    }
    connect(mPhishingDatabase, &WebEngineViewer::LocalDataBaseManager::checkUrlFinished, this, &ViewerPrivate::slotCheckedUrlFinished);

    mShareServiceManager = new PimCommon::ShareServiceUrlManager(this);

    mDisplayFormatMessageOverwrite = MessageViewer::Viewer::UseGlobalSetting;

    mUpdateReaderWinTimer.setObjectName(QLatin1StringView("mUpdateReaderWinTimer"));
    mResizeTimer.setObjectName(QLatin1StringView("mResizeTimer"));

    createWidgets();
    createActions();
    initHtmlWidget();
    readConfig();

    mLevelQuote = MessageViewer::MessageViewerSettings::self()->collapseQuoteLevelSpin() - 1;

    mResizeTimer.setSingleShot(true);
    connect(&mResizeTimer, &QTimer::timeout, this, &ViewerPrivate::slotDelayedResize);

    mUpdateReaderWinTimer.setSingleShot(true);
    connect(&mUpdateReaderWinTimer, &QTimer::timeout, this, &ViewerPrivate::updateReaderWin);

    connect(mNodeHelper, &MimeTreeParser::NodeHelper::update, this, &ViewerPrivate::update);

    connect(mColorBar, &HtmlStatusBar::clicked, this, &ViewerPrivate::slotToggleHtmlMode);

    // FIXME: Don't use the full payload here when attachment loading on demand is used, just
    //        like in KMMainWidget::slotMessageActivated().
    mMonitor.setObjectName(QLatin1StringView("MessageViewerMonitor"));
    mMonitor.setSession(mSession);
    Akonadi::ItemFetchScope fs;
    fs.fetchFullPayload();
    fs.fetchAttribute<Akonadi::ErrorAttribute>();
    fs.fetchAttribute<MessageViewer::MessageDisplayFormatAttribute>();
    fs.fetchAttribute<MessageViewer::ScamAttribute>();
    fs.fetchAttribute<MessageViewer::DKIMResultAttribute>();
    fs.fetchAttribute<Akonadi::MDNStateAttribute>();
    mMonitor.setItemFetchScope(fs);
    connect(&mMonitor, &Akonadi::Monitor::itemChanged, this, &ViewerPrivate::slotItemChanged);
    connect(&mMonitor, &Akonadi::Monitor::itemRemoved, this, &ViewerPrivate::slotClear);
    connect(&mMonitor, &Akonadi::Monitor::itemMoved, this, &ViewerPrivate::slotItemMoved);
}

ViewerPrivate::~ViewerPrivate()
{
    delete mDeveloperToolDialog;
    delete mMessageViewerRenderer;
    MessageViewer::MessageViewerSettings::self()->save();
    delete mHtmlWriter;
    mHtmlWriter = nullptr;
    delete mViewer;
    mViewer = nullptr;
    mNodeHelper->forceCleanTempFiles();
    qDeleteAll(mListMailSourceViewer);
    mMessage.clear();
    delete mNodeHelper;
}

//-----------------------------------------------------------------------------
KMime::Content *ViewerPrivate::nodeFromUrl(const QUrl &url) const
{
    return mNodeHelper->fromHREF(mMessage, url);
}

void ViewerPrivate::openAttachment(KMime::Content *node, const QUrl &url)
{
    if (!node) {
        return;
    }

    if (auto ct = node->contentType(false)) {
        if (ct->mimeType() == "text/x-moz-deleted") {
            return;
        }
        if (ct->mimeType() == "message/external-body") {
            if (ct->hasParameter("url")) {
                auto job = new KIO::OpenUrlJob(url, u"text/html"_s);
                job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, q));
                job->start();
                return;
            }
        }
    }

    const bool isEncapsulatedMessage = node->parent() && node->parent()->bodyIsMessage();
    if (isEncapsulatedMessage) {
        // the viewer/urlhandlermanager expects that the message (mMessage) it is passed is the root when doing index calculation
        // in urls. Simply passing the result of bodyAsMessage() does not cut it as the resulting pointer is a child in its tree.
        KMime::Message::Ptr m(new KMime::Message);
        m->setContent(node->parent()->bodyAsMessage()->encodedContent());
        m->parse();
        attachmentViewMessage(m);
        return;
    }
    // determine the MIME type of the attachment
    // prefer the value of the Content-Type header
    QMimeDatabase mimeDb;
    auto mimetype = mimeDb.mimeTypeForName(QString::fromLatin1(node->contentType()->mimeType().toLower()));

    // special case treatment on mac and windows
    QUrl atmUrl = url;
    if (url.isEmpty()) {
        atmUrl = mNodeHelper->tempFileUrlFromNode(node);
    }
    if (Util::handleUrlWithQDesktopServices(atmUrl)) {
        return;
    }

    if (!mimetype.isValid() || mimetype.name() == QLatin1StringView("application/octet-stream")) {
        mimetype = MimeTreeParser::Util::mimetype(url.isLocalFile() ? url.toLocalFile() : url.fileName());
    }
    KService::Ptr offer = KApplicationTrader::preferredService(mimetype.name());

    const QString filenameText = MimeTreeParser::NodeHelper::fileName(node);

    QPointer<AttachmentDialog> dialog = new AttachmentDialog(mMainWindow, filenameText, offer, QLatin1StringView("askSave_") + mimetype.name());
    const int choice = dialog->exec();
    delete dialog;
    if (choice == AttachmentDialog::Save) {
        QList<QUrl> urlList;
        if (Util::saveContents(mMainWindow, KMime::Content::List() << node, urlList)) {
            showSavedFileFolderWidget(urlList, MessageViewer::OpenSavedFileFolderWidget::FileType::Attachment);
        }
    } else if (choice == AttachmentDialog::Open) { // Open
        if (offer) {
            attachmentOpenWith(node, offer);
        } else {
            attachmentOpen(node);
        }
    } else if (choice == AttachmentDialog::OpenWith) {
        attachmentOpenWith(node);
    } else { // Cancel
        qCDebug(MESSAGEVIEWER_LOG) << "Canceled opening attachment";
    }
}

static bool confirmAttachmentDeletion(QWidget *parent)
{
    return KMessageBox::warningContinueCancel(parent,
                                              i18n("Deleting an attachment might invalidate any digital signature on this message."),
                                              i18nc("@title:window", "Delete Attachment"),
                                              KStandardGuiItem::del(),
                                              KStandardGuiItem::cancel(),
                                              u"DeleteAttachmentSignatureWarning"_s)
        == KMessageBox::Continue;
}

void ViewerPrivate::updateMessageAfterDeletingAttachments(KMime::Message::Ptr &message)
{
    KMime::Message *modifiedMessage = mNodeHelper->messageWithExtraContent(message.data());
    mMimePartTree->mimePartModel()->setRoot(modifiedMessage);
    mMessageItem.setPayloadFromData(message->encodedContent());
    // Modifying the payload might change the remote id (e.g. for IMAP) of the item, so don't try to force on it
    // a potentially old remote id. Without clearing the remote id, deleting multiple attachments from a message
    // stored on an IMAP server will likely fail with "Invalid attempt to modify the remoteID for item [...]".
    mMessageItem.setRemoteId({});
    auto job = new Akonadi::ItemModifyJob(mMessageItem, mSession);
    job->disableRevisionCheck();
    connect(job, &KJob::result, this, &ViewerPrivate::itemModifiedResult);
}

bool ViewerPrivate::deleteAttachment(KMime::Content *node, bool showWarning)
{
    if (!node) {
        return true;
    }
    KMime::Content *parent = node->parent();
    if (!parent) {
        return true;
    }

    const QList<KMime::Content *> extraNodes = mNodeHelper->extraContents(mMessage.data());
    if (extraNodes.contains(node->topLevel())) {
        KMessageBox::error(mMainWindow,
                           i18n("Deleting an attachment from an encrypted or old-style mailman message is not supported."),
                           i18nc("@title:window", "Delete Attachment"));
        return true; // cancelled
    }

    if (showWarning && !confirmAttachmentDeletion(mMainWindow)) {
        return false; // cancelled
    }

    // don't confuse the model
    mMimePartTree->clearModel();

    if (!Util::deleteAttachment(node)) {
        return false;
    }

    updateMessageAfterDeletingAttachments(mMessage);

    return true;
}

void ViewerPrivate::itemModifiedResult(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGEVIEWER_LOG) << "Item update failed:" << job->errorString();
    } else {
        setMessageItem(mMessageItem, MimeTreeParser::Force);
    }
}

void ViewerPrivate::scrollToAnchor(const QString &anchor)
{
    mViewer->scrollToAnchor(anchor);
}

void ViewerPrivate::createOpenWithMenu(QMenu *topMenu, const QString &contentTypeStr, bool fromCurrentContent)
{
    const KService::List offers = KFileItemActions::associatedApplications(QStringList() << contentTypeStr);
    if (!offers.isEmpty()) {
        QMenu *menu = topMenu;
        auto actionGroup = new QActionGroup(menu);

        if (fromCurrentContent) {
            connect(actionGroup, &QActionGroup::triggered, this, &ViewerPrivate::slotOpenWithActionCurrentContent);
        } else {
            connect(actionGroup, &QActionGroup::triggered, this, &ViewerPrivate::slotOpenWithAction);
        }

        if (offers.count() > 1) { // submenu 'open with'
            menu = new QMenu(i18nc("@title:menu", "&Open With"), topMenu);
            menu->menuAction()->setObjectName(QLatin1StringView("openWith_submenu")); // for the unittest
            topMenu->addMenu(menu);
        }
        // qCDebug(MESSAGEVIEWER_LOG) << offers.count() << "offers" << topMenu << menu;

        for (const KService::Ptr &ser : offers) {
            QAction *act = MessageViewer::Util::createAppAction(ser,
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
        if (fromCurrentContent) {
            connect(openWithAct, &QAction::triggered, this, &ViewerPrivate::slotOpenWithDialogCurrentContent);
        } else {
            connect(openWithAct, &QAction::triggered, this, &ViewerPrivate::slotOpenWithDialog);
        }

        menu->addAction(openWithAct);
    } else { // no app offers -> Open With...
        auto act = new QAction(topMenu);
        act->setText(i18nc("@title:menu", "&Open With..."));
        if (fromCurrentContent) {
            connect(act, &QAction::triggered, this, &ViewerPrivate::slotOpenWithDialogCurrentContent);
        } else {
            connect(act, &QAction::triggered, this, &ViewerPrivate::slotOpenWithDialog);
        }
        topMenu->addAction(act);
    }
}

void ViewerPrivate::slotOpenWithDialogCurrentContent()
{
    if (!mCurrentContent) {
        return;
    }
    attachmentOpenWith(mCurrentContent);
}

void ViewerPrivate::slotOpenWithDialog()
{
    const auto contents = selectedContents();
    if (contents.count() == 1) {
        attachmentOpenWith(contents.first());
    }
}

void ViewerPrivate::slotOpenWithActionCurrentContent(QAction *act)
{
    if (!mCurrentContent) {
        return;
    }
    const auto app = act->data().value<KService::Ptr>();
    attachmentOpenWith(mCurrentContent, app);
}

void ViewerPrivate::slotOpenWithAction(QAction *act)
{
    const auto app = act->data().value<KService::Ptr>();
    const auto contents = selectedContents();
    if (contents.count() == 1) {
        attachmentOpenWith(contents.first(), app);
    }
}

void ViewerPrivate::showAttachmentPopup(KMime::Content *node, const QString &name, const QPoint &globalPos)
{
    Q_UNUSED(name)
    prepareHandleAttachment(node);
    bool deletedAttachment = false;
    QString contentTypeStr;
    if (auto contentType = node->contentType(false)) {
        contentTypeStr = QLatin1StringView(contentType->mimeType());
    }
    if (contentTypeStr == QLatin1StringView("message/global")) { // Not registered in mimetype => it's a message/rfc822
        contentTypeStr = u"message/rfc822"_s;
    }
    deletedAttachment = (contentTypeStr == QLatin1StringView("text/x-moz-deleted"));
    // Not necessary to show popup menu when attachment was removed
    if (deletedAttachment) {
        return;
    }

    QMenu menu;

    QAction *action = menu.addAction(QIcon::fromTheme(u"document-open"_s), i18nc("to open", "Open"));
    action->setEnabled(!deletedAttachment);
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::Open);
    });
    createOpenWithMenu(&menu, contentTypeStr, true);

    QMimeDatabase mimeDb;
    auto mimetype = mimeDb.mimeTypeForName(contentTypeStr);
    if (mimetype.isValid()) {
        const QStringList parentMimeType = mimetype.parentMimeTypes();
        if ((contentTypeStr == QLatin1StringView("text/plain")) || (contentTypeStr == QLatin1StringView("image/png"))
            || (contentTypeStr == QLatin1StringView("image/jpeg")) || parentMimeType.contains(QLatin1StringView("text/plain"))
            || parentMimeType.contains(QLatin1StringView("image/png")) || parentMimeType.contains(QLatin1StringView("image/jpeg"))) {
            action = menu.addAction(i18nc("to view something", "View"));
            action->setEnabled(!deletedAttachment);
            connect(action, &QAction::triggered, this, [this]() {
                slotHandleAttachment(Viewer::View);
            });
        }
    }

    action = menu.addAction(i18n("Scroll To"));
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::ScrollTo);
    });

    action = menu.addAction(QIcon::fromTheme(u"document-save-as"_s), i18n("Save As..."));
    action->setEnabled(!deletedAttachment);
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::Save);
    });

    action = menu.addAction(QIcon::fromTheme(u"edit-copy"_s), i18n("Copy"));
    action->setEnabled(!deletedAttachment);
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::Copy);
    });

    const bool isEncapsulatedMessage = node->parent() && node->parent()->bodyIsMessage();
    const bool canChange = mMessageItem.isValid() && mMessageItem.parentCollection().isValid()
        && (mMessageItem.parentCollection().rights() != Akonadi::Collection::ReadOnly) && !isEncapsulatedMessage;

    menu.addSeparator();
    action = menu.addAction(QIcon::fromTheme(u"edit-delete"_s), i18n("Delete Attachment"));
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::Delete);
    });

    action->setEnabled(canChange && !deletedAttachment);
#if 0
    menu->addSeparator();

    action
        = menu->addAction(QIcon::fromTheme(u"mail-reply-sender"_s),
                          i18n("Reply To Author"));
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::ReplyMessageToAuthor);
    });

    menu->addSeparator();

    action = menu->addAction(QIcon::fromTheme(u"mail-reply-all"_s), i18n(
                                 "Reply To All"));
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::ReplyMessageToAll);
    });
#endif
    menu.addSeparator();
    action = menu.addAction(i18n("Properties"));
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::Properties);
    });
    menu.exec(globalPos);
}

void ViewerPrivate::prepareHandleAttachment(KMime::Content *node)
{
    mCurrentContent = node;
}

KService::Ptr ViewerPrivate::getServiceOffer(KMime::Content *content)
{
    const QString fileName = mNodeHelper->writeNodeToTempFile(content);

    const QString contentTypeStr = QLatin1StringView(content->contentType()->mimeType());

    // determine the MIME type of the attachment
    // prefer the value of the Content-Type header
    QMimeDatabase mimeDb;
    auto mimetype = mimeDb.mimeTypeForName(contentTypeStr);

    if (mimetype.isValid() && mimetype.inherits(KContacts::Addressee::mimeType())) {
        attachmentView(content);
        return KService::Ptr(nullptr);
    }

    if (!mimetype.isValid() || mimetype.name() == QLatin1StringView("application/octet-stream")) {
        /*TODO(Andris) port when on-demand loading is done   && msgPart.isComplete() */
        mimetype = MimeTreeParser::Util::mimetype(fileName);
    }
    return KApplicationTrader::preferredService(mimetype.name());
}

KMime::Content::List ViewerPrivate::selectedContents() const
{
    return mMimePartTree->selectedContents();
}

void ViewerPrivate::attachmentOpenWith(KMime::Content *node, const KService::Ptr &offer)
{
    QString name = mNodeHelper->writeNodeToTempFile(node);

    // Make sure that it will not deleted when we switch from message.
    auto tmpDir = new QTemporaryDir(QDir::tempPath() + QLatin1StringView("/messageviewer_attachment_XXXXXX"));
    if (tmpDir->isValid()) {
        tmpDir->setAutoRemove(false);
        const QString path = tmpDir->path();
        delete tmpDir;
        QFile f(name);
        const QUrl tmpFileName = QUrl::fromLocalFile(name);
        const QString newPath = path + u'/' + tmpFileName.fileName();

        if (!f.copy(newPath)) {
            qCDebug(MESSAGEVIEWER_LOG) << " File was not able to copy: filename: " << name << " to " << path;
        } else {
            name = newPath;
        }
        f.close();
    } else {
        delete tmpDir;
    }

    const QFileDevice::Permissions perms = QFile::permissions(name);
    QFile::setPermissions(name, perms | QFileDevice::ReadUser | QFileDevice::WriteUser);
    const QUrl url = QUrl::fromLocalFile(name);

    auto job = new KIO::ApplicationLauncherJob(offer);
    job->setUrls({url});
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, mMainWindow));
    job->start();
    connect(job, &KJob::result, this, [url, job]() {
        if (job->error()) {
            QFile::remove(url.toLocalFile());
        }
    });
}

void ViewerPrivate::attachmentOpen(KMime::Content *node)
{
    const KService::Ptr offer = getServiceOffer(node);
    if (!offer) {
        qCDebug(MESSAGEVIEWER_LOG) << "got no offer";
        return;
    }
    attachmentOpenWith(node, offer);
}

bool ViewerPrivate::showEmoticons() const
{
    return mForceEmoticons;
}

HtmlWriter *ViewerPrivate::htmlWriter() const
{
    return mHtmlWriter;
}

CSSHelper *ViewerPrivate::cssHelper() const
{
    return mMessageViewerRenderer->cssHelper();
}

MimeTreeParser::NodeHelper *ViewerPrivate::nodeHelper() const
{
    return mNodeHelper;
}

Viewer *ViewerPrivate::viewer() const
{
    return q;
}

Akonadi::Item ViewerPrivate::messageItem() const
{
    return mMessageItem;
}

KMime::Message::Ptr ViewerPrivate::message() const
{
    return mMessage;
}

bool ViewerPrivate::decryptMessage() const
{
    if (MessageViewer::MessageViewerSettings::self()->alwaysDecrypt()) {
        return true;
    } else {
        return mDecrytMessageOverwrite;
    }
}

void ViewerPrivate::displaySplashPage(const QString &message)
{
    displaySplashPage(u"status.html"_s,
                      {{u"icon"_s, u"kmail"_s}, {u"name"_s, i18n("KMail")}, {u"subtitle"_s, i18n("The KDE Mail Client")}, {u"message"_s, message}});
}

void ViewerPrivate::displaySplashPage(const QString &templateName, const QVariantHash &data, const QByteArray &domain)
{
    if (mViewer) {
        mMsgDisplay = false;
        adjustLayout();

        GrantleeTheme::ThemeManager manager(u"splashPage"_s, u"splash.theme"_s, nullptr, QStringLiteral("messageviewer/about/"));
        GrantleeTheme::Theme theme = manager.theme(u"default"_s);
        if (theme.isValid()) {
            mViewer->setHtml(theme.render(templateName, data, domain), QUrl::fromLocalFile(theme.absolutePath() + u'/'));
        } else {
            qCDebug(MESSAGEVIEWER_LOG) << "Theme error: failed to find splash theme";
        }
        mViewer->show();
    }
}

void ViewerPrivate::enableMessageDisplay()
{
    if (mMsgDisplay) {
        return;
    }
    mMsgDisplay = true;
    adjustLayout();
}

void ViewerPrivate::displayMessage()
{
    showHideMimeTree();

    mNodeHelper->setOverrideCodec(mMessage.data(), overrideCodecName());

    if (mMessageItem.hasAttribute<MessageViewer::MessageDisplayFormatAttribute>()) {
        const MessageViewer::MessageDisplayFormatAttribute *const attr = mMessageItem.attribute<MessageViewer::MessageDisplayFormatAttribute>();
        setHtmlLoadExtOverride(attr->remoteContent());
        setDisplayFormatMessageOverwrite(attr->messageFormat());
    }

    adaptHtmlHeadSettings();
    htmlWriter()->begin();
    htmlWriter()->write(cssHelper()->htmlHead(mHtmlHeadSettings));

    if (!mMainWindow) {
        if (auto subject = mMessage->subject(false)) {
            q->setWindowTitle(subject->asUnicodeString());
        }
    }

    // Don't update here, parseMsg() can overwrite the HTML mode, which would lead to flicker.
    // It is updated right after parseMsg() instead.
    mColorBar->setMode(MimeTreeParser::Util::Normal, HtmlStatusBar::NoUpdate);

    if (mMessageItem.hasAttribute<Akonadi::ErrorAttribute>()) {
        // TODO: Insert link to clear error so that message might be resent
        const auto *const attr = mMessageItem.attribute<Akonadi::ErrorAttribute>();
        Q_ASSERT(attr);
        initializeColorFromScheme();

        htmlWriter()->write(u"<div style=\"background:%1;color:%2;border:1px solid %2\">%3</div>"_s.arg(mBackgroundError.name(),
                                                                                                        mForegroundError.name(),
                                                                                                        attr->message().toHtmlEscaped()));
        htmlWriter()->write(u"<p></p>"_s);
    }

    parseContent(mMessage.data());
    mMimePartTree->setRoot(mNodeHelper->messageWithExtraContent(mMessage.data()));
    mColorBar->update();

    htmlWriter()->write(cssHelper()->endBodyHtml());
    connect(mViewer, &MailWebEngineView::loadFinished, this, &ViewerPrivate::executeCustomScriptsAfterLoading, Qt::UniqueConnection);
    connect(mPartHtmlWriter.data(), &WebEnginePartHtmlWriter::finished, this, &ViewerPrivate::slotMessageRendered, Qt::UniqueConnection);

    htmlWriter()->end();
}

void ViewerPrivate::parseContent(KMime::Content *content)
{
    Q_ASSERT(content != nullptr);
    mNodeHelper->removeTempFiles();

    // Check if any part of this message is a v-card
    // v-cards can be either text/x-vcard or text/directory, so we need to check
    // both.
    KMime::Content *vCardContent = findContentByType(content, "text/x-vcard");
    if (!vCardContent) {
        vCardContent = findContentByType(content, "text/directory");
    }
    bool hasVCard = false;
    if (vCardContent) {
        // ### FIXME: We should only do this if the vCard belongs to the sender,
        // ### i.e. if the sender's email address is contained in the vCard.
        const QByteArray vCard = vCardContent->decodedContent();
        KContacts::VCardConverter t;
        if (!t.parseVCards(vCard).isEmpty()) {
            hasVCard = true;
            mNodeHelper->writeNodeToTempFile(vCardContent);
        }
    }

    auto message = dynamic_cast<KMime::Message *>(content);
    bool onlySingleNode = mMessage.data() != content;

    // Pass control to the OTP now, which does the real work
    mNodeHelper->setNodeUnprocessed(mMessage.data(), true);
    MailViewerSource otpSource(this);
    MimeTreeParser::ObjectTreeParser otp(&otpSource, mNodeHelper);

    otp.setAllowAsync(!mPrinting);
    otp.parseObjectTree(content, onlySingleNode);
    if (message) {
        htmlWriter()->write(writeMessageHeader(message, hasVCard ? vCardContent : nullptr, true));
    }

    otpSource.render(otp.parsedPart(), onlySingleNode);

    // TODO: Setting the signature state to nodehelper is not enough, it should actually
    // be added to the store, so that the message list correctly displays the signature state
    // of messages that were parsed at least once
    // store encrypted/signed status information in the KMMessage
    //  - this can only be done *after* calling parseObjectTree()
    const MimeTreeParser::KMMsgEncryptionState encryptionState = mNodeHelper->overallEncryptionState(content);
    const MimeTreeParser::KMMsgSignatureState signatureState = mNodeHelper->overallSignatureState(content);
    mNodeHelper->setEncryptionState(content, encryptionState);
    // Don't reset the signature state to "not signed" (e.g. if one canceled the
    // decryption of a signed messages which has already been decrypted before).
    if (signatureState != MimeTreeParser::KMMsgNotSigned || mNodeHelper->signatureState(content) == MimeTreeParser::KMMsgSignatureStateUnknown) {
        mNodeHelper->setSignatureState(content, signatureState);
    }

    if (!onlySingleNode && isAutocryptEnabled(message)) {
        auto mixup = HeaderMixupNodeHelper(mNodeHelper, message);
        processAutocryptfromMail(mixup);
    }

    showHideMimeTree();
}

QString ViewerPrivate::writeMessageHeader(KMime::Message *aMsg, KMime::Content *vCardNode, bool topLevel)
{
    if (!headerStylePlugin()) {
        qCCritical(MESSAGEVIEWER_LOG) << "trying to writeMessageHeader() without a header style set!";
        return {};
    }
    HeaderStyle *style = headerStylePlugin()->headerStyle();
    if (vCardNode) {
        style->setVCardName(mNodeHelper->asHREF(vCardNode, u"body"_s));
    } else {
        style->setVCardName(QString());
    }
    style->setHeaderStrategy(headerStylePlugin()->headerStrategy());
    style->setPrinting(mPrinting);
    style->setTopLevel(topLevel);
    style->setAllowAsync(true);
    style->setSourceObject(this);
    style->setNodeHelper(mNodeHelper);
    style->setAttachmentHtml(attachmentHtml());
    if (mMessageItem.isValid()) {
        Akonadi::MessageStatus status;
        status.setStatusFromFlags(mMessageItem.flags());

        style->setMessageStatus(status);
    } else {
        style->setReadOnlyMessage(true);
    }

    return style->format(aMsg);
}

void ViewerPrivate::initHtmlWidget()
{
    if (!htmlWriter()) {
        mPartHtmlWriter = new WebEnginePartHtmlWriter(mViewer, nullptr);
        mHtmlWriter = mPartHtmlWriter;
    }
    connect(mViewer->page(), &QWebEnginePage::linkHovered, this, &ViewerPrivate::slotUrlOn);
    connect(mViewer, &MailWebEngineView::openUrl, this, &ViewerPrivate::slotUrlOpen, Qt::QueuedConnection);
    connect(mViewer, &MailWebEngineView::popupMenu, this, &ViewerPrivate::slotUrlPopup);
    connect(mViewer, &MailWebEngineView::wheelZoomChanged, this, &ViewerPrivate::slotWheelZoomChanged);
    connect(mViewer, &MailWebEngineView::messageMayBeAScam, this, &ViewerPrivate::slotMessageMayBeAScam);
    connect(mViewer, &MailWebEngineView::formSubmittedForbidden, this, [this]() {
        if (!mSubmittedFormWarning) {
            createSubmittedFormWarning();
        }
        mSubmittedFormWarning->showWarning();
    });
    connect(mViewer, &MailWebEngineView::mailTrackingFound, this, [this](const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &lst) {
        if (!mMailTrackingWarning) {
            createTrackingWarningWidget();
        }
        mMailTrackingWarning->addTracker(lst);
    });
    connect(mViewer, &MailWebEngineView::pageIsScrolledToBottom, this, &ViewerPrivate::pageIsScrolledToBottom);
    connect(mViewer, &MailWebEngineView::urlBlocked, this, &ViewerPrivate::slotUrlBlocked);
}

void ViewerPrivate::slotUrlBlocked(const QUrl &url)
{
    mRemoteContentMenu->appendUrl(url.adjusted(QUrl::RemovePath | QUrl::RemovePort | QUrl::RemoveQuery).toString());
}

RemoteContentMenu *ViewerPrivate::remoteContentMenu() const
{
    return mRemoteContentMenu;
}

void ViewerPrivate::applyZoomValue(qreal factor, bool saveConfig)
{
    if (mZoomActionMenu) {
        if (factor >= 10 && factor <= 300) {
            if (!qFuzzyCompare(mZoomActionMenu->zoomFactor(), factor)) {
                mZoomActionMenu->setZoomFactor(factor);
                mZoomActionMenu->setWebViewerZoomFactor(factor / 100.0);
                if (saveConfig) {
                    MessageViewer::MessageViewerSettings::self()->setZoomFactor(factor);
                }
            }
        }
    }
}

void ViewerPrivate::setWebViewZoomFactor(qreal factor)
{
    applyZoomValue(factor, false);
}

qreal ViewerPrivate::webViewZoomFactor() const
{
    qreal zoomFactor = -1;
    if (mZoomActionMenu) {
        zoomFactor = mZoomActionMenu->zoomFactor();
    }
    return zoomFactor;
}

void ViewerPrivate::slotWheelZoomChanged(int numSteps)
{
    const qreal factor = mZoomActionMenu->zoomFactor() + numSteps * 10;
    applyZoomValue(factor);
}

void ViewerPrivate::readConfig()
{
    mMessageViewerRenderer->setCurrentWidget(mViewer);
    recreateCssHelper();

    mForceEmoticons = MessageViewer::MessageViewerSettings::self()->showEmoticons();
    if (mDisableEmoticonAction) {
        mDisableEmoticonAction->setChecked(!mForceEmoticons);
    }
    if (headerStylePlugin()) {
        headerStylePlugin()->headerStyle()->setShowEmoticons(mForceEmoticons);
    }

    mHtmlHeadSettings.fixedFont = MessageViewer::MessageViewerSettings::self()->useFixedFont();
    if (mToggleFixFontAction) {
        mToggleFixFontAction->setChecked(mHtmlHeadSettings.fixedFont);
    }

    mHtmlMailGlobalSetting = MessageViewer::MessageViewerSettings::self()->htmlMail();

    MessageViewer::Util::readGravatarConfig();
    if (mHeaderStyleMenuManager) {
        mHeaderStyleMenuManager->readConfig();
    }

    setAttachmentStrategy(AttachmentStrategy::create(MessageViewer::MessageViewerSettings::self()->attachmentStrategy()));
    KToggleAction *raction = actionForAttachmentStrategy(attachmentStrategy());
    if (raction) {
        raction->setChecked(true);
    }

    adjustLayout();

    readGlobalOverrideCodec();
    mViewer->readConfig();
    mViewer->settings()->setFontSize(QWebEngineSettings::MinimumFontSize, MessageViewer::MessageViewerSettings::self()->minimumFontSize());
    mViewer->settings()->setFontSize(QWebEngineSettings::MinimumLogicalFontSize, MessageViewer::MessageViewerSettings::self()->minimumFontSize());
    if (mMessage) {
        update();
    }
    mColorBar->update();
    applyZoomValue(MessageViewer::MessageViewerSettings::self()->zoomFactor(), false);
}

void ViewerPrivate::recreateCssHelper()
{
    mMessageViewerRenderer->recreateCssHelper();
}

void ViewerPrivate::hasMultiMessages(bool messages)
{
    if (!mShowNextMessageWidget) {
        createShowNextMessageWidget();
        mShowNextMessageWidget->setVisible(messages);
    }
}

void ViewerPrivate::slotGeneralFontChanged()
{
    recreateCssHelper();
    if (mMessage) {
        update();
    }
}

void ViewerPrivate::writeConfig(bool sync)
{
    MessageViewer::MessageViewerSettings::self()->setShowEmoticons(mForceEmoticons);
    MessageViewer::MessageViewerSettings::self()->setUseFixedFont(mHtmlHeadSettings.fixedFont);
    if (attachmentStrategy()) {
        MessageViewer::MessageViewerSettings::self()->setAttachmentStrategy(QLatin1StringView(attachmentStrategy()->name()));
    }
    saveSplitterSizes();
    if (sync) {
        Q_EMIT requestConfigSync();
    }
}

const AttachmentStrategy *ViewerPrivate::attachmentStrategy() const
{
    return mAttachmentStrategy;
}

void ViewerPrivate::setAttachmentStrategy(const AttachmentStrategy *strategy)
{
    if (mAttachmentStrategy == strategy) {
        return;
    }
    mAttachmentStrategy = strategy ? strategy : AttachmentStrategy::smart();
    update(MimeTreeParser::Force);
}

QString ViewerPrivate::overrideEncoding() const
{
    return mOverrideEncoding;
}

void ViewerPrivate::setOverrideEncoding(const QString &encoding)
{
    if (encoding == mOverrideEncoding) {
        return;
    }

    mOverrideEncoding = encoding;
    if (mSelectEncodingAction) {
        if (encoding.isEmpty()) {
            mSelectEncodingAction->setCurrentItem(0);
        } else {
            const QStringList encodings = mSelectEncodingAction->items();
            int i = 0;
            for (QStringList::const_iterator it = encodings.constBegin(), end = encodings.constEnd(); it != end; ++it, ++i) {
                if (MimeTreeParser::NodeHelper::encodingForName(*it) == encoding) {
                    mSelectEncodingAction->setCurrentItem(i);
                    break;
                }
            }
            if (i == encodings.size()) {
                // the value of encoding is unknown => use Auto
                qCWarning(MESSAGEVIEWER_LOG) << "Unknown override character encoding" << encoding << ". Using Auto instead.";
                mSelectEncodingAction->setCurrentItem(0);
                mOverrideEncoding.clear();
            }
        }
    }
    update(MimeTreeParser::Force);
}

void ViewerPrivate::setPrinting(bool enable)
{
    mPrinting = enable;
}

bool ViewerPrivate::printingMode() const
{
    return mPrinting;
}

void ViewerPrivate::printMessage(const Akonadi::Item &message)
{
    disconnect(mPartHtmlWriter.data(), &WebEnginePartHtmlWriter::finished, this, &ViewerPrivate::slotPrintMessage);
    connect(mPartHtmlWriter.data(), &WebEnginePartHtmlWriter::finished, this, &ViewerPrivate::slotPrintMessage);
    // need to set htmlLoadExtOverride() when we set Item otherwise this settings is reset
    setMessageItem(message, MimeTreeParser::Force, htmlLoadExtOverride());
}

void ViewerPrivate::printPreviewMessage(const Akonadi::Item &message)
{
    disconnect(mPartHtmlWriter.data(), &WebEnginePartHtmlWriter::finished, this, &ViewerPrivate::slotPrintPreview);
    connect(mPartHtmlWriter.data(), &WebEnginePartHtmlWriter::finished, this, &ViewerPrivate::slotPrintPreview);
    setMessageItem(message, MimeTreeParser::Force, htmlLoadExtOverride());
}

void ViewerPrivate::resetStateForNewMessage()
{
    mDkimWidgetInfo->clear();
    mHtmlLoadExtOverride = false;
    mClickedUrl.clear();
    mImageUrl.clear();
    enableMessageDisplay(); // just to make sure it's on
    mMessage.reset();
    mNodeHelper->clear();
    mMessagePartNode = nullptr;
    mMimePartTree->clearModel();
    if (mViewer) {
        mViewer->clearRelativePosition();
        mViewer->hideAccessKeys();
    }
    if (!mPrinting) {
        setShowSignatureDetails(false);
    }
    mViewerPluginToolManager->closeAllTools();
    if (mScamDetectionWarning) {
        mScamDetectionWarning->setVisible(false);
    }
    if (mOpenSavedFileFolderWidget) {
        mOpenSavedFileFolderWidget->setVisible(false);
    }
    if (mSubmittedFormWarning) {
        mSubmittedFormWarning->setVisible(false);
    }
    if (mMailTrackingWarning) {
        mMailTrackingWarning->hideAndClear();
    }
    mRemoteContentMenu->clearUrls();

    if (mPrinting) {
        if (MessageViewer::MessageViewerSettings::self()->respectExpandCollapseSettings()) {
            if (MessageViewer::MessageViewerSettings::self()->showExpandQuotesMark()) {
                mLevelQuote = MessageViewer::MessageViewerSettings::self()->collapseQuoteLevelSpin() - 1;
            } else {
                mLevelQuote = -1;
            }
        } else {
            mLevelQuote = -1;
        }
    } else {
        //        mDisplayFormatMessageOverwrite
        //            = (mDisplayFormatMessageOverwrite
        //               == MessageViewer::Viewer::UseGlobalSetting) ? MessageViewer::Viewer::UseGlobalSetting
        //              :
        //              MessageViewer::Viewer::Unknown;
    }
}

void ViewerPrivate::setMessageInternal(const KMime::Message::Ptr &message, MimeTreeParser::UpdateMode updateMode)
{
    mViewerPluginToolManager->updateActions(mMessageItem);
    mMessage = message;
    if (message) {
        mNodeHelper->setOverrideCodec(mMessage.data(), overrideCodecName());
    }

    mMimePartTree->setRoot(mNodeHelper->messageWithExtraContent(message.data()));
    update(updateMode);
}

void ViewerPrivate::assignMessageItem(const Akonadi::Item &item)
{
    mMessageItem = item;
}

void ViewerPrivate::setMessageItem(const Akonadi::Item &item, MimeTreeParser::UpdateMode updateMode, bool forceHtmlLoadExtOverride)
{
    resetStateForNewMessage();
    if (forceHtmlLoadExtOverride) {
        setHtmlLoadExtOverride(true);
    }

    const auto itemsMonitoredEx = mMonitor.itemsMonitoredEx();

    for (const Akonadi::Item::Id monitoredId : itemsMonitoredEx) {
        mMonitor.setItemMonitored(Akonadi::Item(monitoredId), false);
    }
    Q_ASSERT(mMonitor.itemsMonitoredEx().isEmpty());

    assignMessageItem(item);
    if (mMessageItem.isValid()) {
        mMonitor.setItemMonitored(mMessageItem, true);
    }

    if (!mMessageItem.hasPayload<KMime::Message::Ptr>()) {
        if (mMessageItem.isValid()) {
            qCWarning(MESSAGEVIEWER_LOG) << "Payload is not a MessagePtr!";
        }
        return;
    }
    if (!mPrinting) {
        if (MessageViewer::MessageViewerSettings::self()->enabledDkim()) {
            if (messageIsInSpecialFolder()) {
                mDkimWidgetInfo->clear();
            } else {
                mDkimWidgetInfo->setCurrentItemId(mMessageItem.id());
                MessageViewer::DKIMManager::self()->checkDKim(mMessageItem);
            }
        }
    }

    setMessageInternal(mMessageItem.payload<KMime::Message::Ptr>(), updateMode);
}

bool ViewerPrivate::messageIsInSpecialFolder() const
{
    const Akonadi::Collection parentCollection = mMessageItem.parentCollection();
    if ((Akonadi::SpecialMailCollections::self()->defaultCollection(Akonadi::SpecialMailCollections::SentMail) != parentCollection)
        && (Akonadi::SpecialMailCollections::self()->defaultCollection(Akonadi::SpecialMailCollections::Outbox) != parentCollection)
        && (Akonadi::SpecialMailCollections::self()->defaultCollection(Akonadi::SpecialMailCollections::Templates) != parentCollection)
        && (Akonadi::SpecialMailCollections::self()->defaultCollection(Akonadi::SpecialMailCollections::Drafts) != parentCollection)) {
        return false;
    } else {
        return true;
    }
}

void ViewerPrivate::setMessage(const KMime::Message::Ptr &aMsg, MimeTreeParser::UpdateMode updateMode)
{
    resetStateForNewMessage();

    Akonadi::Item item;
    item.setMimeType(KMime::Message::mimeType());
    item.setPayload(aMsg);
    assignMessageItem(item);

    setMessageInternal(aMsg, updateMode);
}

void ViewerPrivate::setMessagePart(KMime::Content *node)
{
    // Cancel scheduled updates of the reader window, as that would stop the
    // timer of the HTML writer, which would make viewing attachment not work
    // anymore as not all HTML is written to the HTML part.
    // We're updating the reader window here ourselves anyway.
    mUpdateReaderWinTimer.stop();

    if (node) {
        mMessagePartNode = node;
        if (node->bodyIsMessage()) {
            mMainWindow->setWindowTitle(node->bodyAsMessage()->subject()->asUnicodeString());
        } else {
            QString windowTitle = MimeTreeParser::NodeHelper::fileName(node);
            if (windowTitle.isEmpty()) {
                windowTitle = node->contentDescription()->asUnicodeString();
            }
            if (!windowTitle.isEmpty()) {
                mMainWindow->setWindowTitle(i18nc("@title:window", "View Attachment: %1", windowTitle));
            }
        }

        htmlWriter()->begin();
        adaptHtmlHeadSettings();
        htmlWriter()->write(cssHelper()->htmlHead(mHtmlHeadSettings));

        parseContent(node);

        htmlWriter()->write(cssHelper()->endBodyHtml());
        htmlWriter()->end();
    }
}

void ViewerPrivate::adaptHtmlHeadSettings()
{
    mHtmlHeadSettings.htmlFormat = htmlMail();
}

void ViewerPrivate::showHideMimeTree()
{
    if (mimePartTreeIsEmpty()) {
        mMimePartTree->hide();
        return;
    }
    bool showMimeTree = false;
    if (MessageViewer::MessageViewerSettings::self()->mimeTreeMode2() == MessageViewer::MessageViewerSettings::EnumMimeTreeMode2::Always) {
        mMimePartTree->show();
        showMimeTree = true;
    } else {
        // don't rely on QSplitter maintaining sizes for hidden widgets:
        saveSplitterSizes();
        mMimePartTree->hide();
        showMimeTree = false;
    }
    if (mToggleMimePartTreeAction && (mToggleMimePartTreeAction->isChecked() != showMimeTree)) {
        mToggleMimePartTreeAction->setChecked(showMimeTree);
    }
}

void ViewerPrivate::attachmentViewMessage(const KMime::Message::Ptr &message)
{
    Q_ASSERT(message);
    Q_EMIT showMessage(message, overrideEncoding());
}

void ViewerPrivate::adjustLayout()
{
    const int mimeH = MessageViewer::MessageViewerSettings::self()->mimePaneHeight();
    const int messageH = MessageViewer::MessageViewerSettings::self()->messagePaneHeight();
    const QList<int> splitterSizes{messageH, mimeH};

    mSplitter->addWidget(mMimePartTree);
    mSplitter->setSizes(splitterSizes);

    if (MessageViewer::MessageViewerSettings::self()->mimeTreeMode2() == MessageViewer::MessageViewerSettings::EnumMimeTreeMode2::Always && mMsgDisplay) {
        mMimePartTree->show();
    } else {
        mMimePartTree->hide();
    }

    if (mMsgDisplay) {
        mColorBar->show();
    } else {
        mColorBar->hide();
    }
}

void ViewerPrivate::saveSplitterSizes() const
{
    if (!mSplitter || !mMimePartTree) {
        return;
    }
    if (mMimePartTree->isHidden()) {
        return; // don't rely on QSplitter maintaining sizes for hidden widgets.
    }
    MessageViewer::MessageViewerSettings::self()->setMimePaneHeight(mSplitter->sizes().at(1));
    MessageViewer::MessageViewerSettings::self()->setMessagePaneHeight(mSplitter->sizes().at(0));
}

void ViewerPrivate::createWidgets()
{
    // TODO: Make a MDN bar similar to Mozillas password bar and show MDNs here as soon as a
    //      MDN enabled message is shown.
    auto vlay = new QVBoxLayout(q);
    vlay->setContentsMargins({});
    mSplitter = new QSplitter(Qt::Vertical, q);
    connect(mSplitter, &QSplitter::splitterMoved, this, &ViewerPrivate::saveSplitterSizes);
    mSplitter->setObjectName(QLatin1StringView("mSplitter"));
    mSplitter->setChildrenCollapsible(false);
    vlay->addWidget(mSplitter);
    mMimePartTree = new MimePartTreeView(mSplitter);
    mMimePartTree->setMinimumHeight(10);
    connect(mMimePartTree, &QAbstractItemView::activated, this, &ViewerPrivate::slotMimePartSelected);
    connect(mMimePartTree, &QWidget::customContextMenuRequested, this, &ViewerPrivate::slotMimeTreeContextMenuRequested);

    mBox = new QWidget(mSplitter);
    auto mBoxHBoxLayout = new QHBoxLayout(mBox);
    mBoxHBoxLayout->setContentsMargins({});
    mBoxHBoxLayout->setSpacing(0);

    mColorBar = new HtmlStatusBar(mBox);
    mBoxHBoxLayout->addWidget(mColorBar);
    mReaderBox = new QWidget(mBox);
    mReaderBoxVBoxLayout = new QVBoxLayout(mReaderBox);
    mReaderBoxVBoxLayout->setContentsMargins({});
    mReaderBoxVBoxLayout->setSpacing(0);
    mBoxHBoxLayout->addWidget(mReaderBox);

    mColorBar->setObjectName(QLatin1StringView("mColorBar"));
    mColorBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);

#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    mTextToSpeechContainerWidget = new TextEditTextToSpeech::TextToSpeechContainerWidget(mReaderBox);
    mTextToSpeechContainerWidget->setObjectName(QLatin1StringView("TextToSpeechContainerWidget"));
    mReaderBoxVBoxLayout->addWidget(mTextToSpeechContainerWidget);
#endif
    mViewer = new MailWebEngineView(mActionCollection, mReaderBox);
    mViewer->setViewer(this);
    mReaderBoxVBoxLayout->addWidget(mViewer);
    mViewer->setObjectName(QLatin1StringView("mViewer"));

    mViewerPluginToolManager = new MessageViewer::ViewerPluginToolManager(mReaderBox, this);
    mViewerPluginToolManager->setActionCollection(mActionCollection);
    mViewerPluginToolManager->setPluginName(u"messageviewer"_s);
    mViewerPluginToolManager->setPluginDirectory(u"pim6/messageviewer/viewerplugin"_s);
    if (!mViewerPluginToolManager->initializePluginList()) {
        qCWarning(MESSAGEVIEWER_LOG) << " Impossible to initialize plugins";
    }
    mViewerPluginToolManager->createView();
    connect(mViewerPluginToolManager, &MessageViewer::ViewerPluginToolManager::activatePlugin, this, &ViewerPrivate::slotActivatePlugin);

    mSliderContainer = new TextAddonsWidgets::SlideContainer(mReaderBox);
    mSliderContainer->setObjectName(QLatin1StringView("slidercontainer"));
    mReaderBoxVBoxLayout->addWidget(mSliderContainer);
    mFindBar = new WebEngineViewer::FindBarWebEngineView(mViewer, q);
    connect(mFindBar, &WebEngineViewer::FindBarWebEngineView::hideFindBar, mSliderContainer, &TextAddonsWidgets::SlideContainer::slideOut);
    mSliderContainer->setContent(mFindBar);

    mSplitter->setStretchFactor(mSplitter->indexOf(mMimePartTree), 0);
}

void ViewerPrivate::createScamDetectionWarningWidget()
{
    mScamDetectionWarning = new ScamDetectionWarningWidget(mReaderBox);
    mScamDetectionWarning->setObjectName(QLatin1StringView("scandetectionwarning"));
    connect(mScamDetectionWarning, &ScamDetectionWarningWidget::showDetails, mViewer, &MailWebEngineView::slotShowDetails);
    connect(mScamDetectionWarning, &ScamDetectionWarningWidget::moveMessageToTrash, this, &ViewerPrivate::moveMessageToTrash);
    connect(mScamDetectionWarning, &ScamDetectionWarningWidget::messageIsNotAScam, this, &ViewerPrivate::slotMessageIsNotAScam);
    connect(mScamDetectionWarning, &ScamDetectionWarningWidget::addToWhiteList, this, &ViewerPrivate::slotAddToWhiteList);
    mReaderBoxVBoxLayout->insertWidget(0, mScamDetectionWarning);
}

void ViewerPrivate::createTrackingWarningWidget()
{
    mMailTrackingWarning = new WebEngineViewer::TrackingWarningWidget(mReaderBox);
    mMailTrackingWarning->setObjectName(QLatin1StringView("mailtrackingwarning"));
    mReaderBoxVBoxLayout->insertWidget(0, mMailTrackingWarning);
}

void ViewerPrivate::createOpenSavedFileFolderWidget()
{
    mOpenSavedFileFolderWidget = new OpenSavedFileFolderWidget(mReaderBox);
    mOpenSavedFileFolderWidget->setObjectName(QLatin1StringView("opensavefilefolderwidget"));
    mReaderBoxVBoxLayout->insertWidget(0, mOpenSavedFileFolderWidget);
}

void ViewerPrivate::createSubmittedFormWarning()
{
    mSubmittedFormWarning = new WebEngineViewer::SubmittedFormWarningWidget(mReaderBox);
    mSubmittedFormWarning->setObjectName(QLatin1StringView("submittedformwarning"));
    mReaderBoxVBoxLayout->insertWidget(0, mSubmittedFormWarning);
}

void ViewerPrivate::slotStyleChanged(MessageViewer::HeaderStylePlugin *plugin)
{
    cssHelper()->setHeaderPlugin(plugin);
    mHeaderStylePlugin = plugin;
    update(MimeTreeParser::Force);
}

void ViewerPrivate::slotStyleUpdated()
{
    update(MimeTreeParser::Force);
}

void ViewerPrivate::createActions()
{
    KActionCollection *ac = mActionCollection;
    mHeaderStyleMenuManager = new MessageViewer::HeaderStyleMenuManager(ac, this);
    connect(mHeaderStyleMenuManager, &MessageViewer::HeaderStyleMenuManager::styleChanged, this, &ViewerPrivate::slotStyleChanged);
    connect(mHeaderStyleMenuManager, &MessageViewer::HeaderStyleMenuManager::styleUpdated, this, &ViewerPrivate::slotStyleUpdated);
    if (!ac) {
        return;
    }
    mZoomActionMenu = new WebEngineViewer::ZoomActionMenu(this);
    connect(mZoomActionMenu, &WebEngineViewer::ZoomActionMenu::zoomChanged, this, &ViewerPrivate::slotZoomChanged);
    mZoomActionMenu->setActionCollection(ac);
    mZoomActionMenu->createZoomActions();

    // attachment style
    auto attachmentMenu = new KActionMenu(i18nc("View->", "&Attachments"), this);
    ac->addAction(u"view_attachments"_s, attachmentMenu);
    MessageViewer::Util::addHelpTextAction(attachmentMenu, i18n("Choose display style of attachments"));

    auto group = new QActionGroup(this);
    auto raction = new KToggleAction(i18nc("View->attachments->", "&As Icons"), this);
    ac->addAction(u"view_attachments_as_icons"_s, raction);
    connect(raction, &QAction::triggered, this, &ViewerPrivate::slotIconicAttachments);
    MessageViewer::Util::addHelpTextAction(raction, i18n("Show all attachments as icons. Click to see them."));
    group->addAction(raction);
    attachmentMenu->addAction(raction);

    raction = new KToggleAction(i18nc("View->attachments->", "&Smart"), this);
    ac->addAction(u"view_attachments_smart"_s, raction);
    connect(raction, &QAction::triggered, this, &ViewerPrivate::slotSmartAttachments);
    MessageViewer::Util::addHelpTextAction(raction, i18n("Show attachments as suggested by sender."));
    group->addAction(raction);
    attachmentMenu->addAction(raction);

    raction = new KToggleAction(i18nc("View->attachments->", "&Inline"), this);
    ac->addAction(u"view_attachments_inline"_s, raction);
    connect(raction, &QAction::triggered, this, &ViewerPrivate::slotInlineAttachments);
    MessageViewer::Util::addHelpTextAction(raction, i18n("Show all attachments inline (if possible)"));
    group->addAction(raction);
    attachmentMenu->addAction(raction);

    raction = new KToggleAction(i18nc("View->attachments->", "&Hide"), this);
    ac->addAction(u"view_attachments_hide"_s, raction);
    connect(raction, &QAction::triggered, this, &ViewerPrivate::slotHideAttachments);
    MessageViewer::Util::addHelpTextAction(raction, i18n("Do not show attachments in the message viewer"));
    group->addAction(raction);
    attachmentMenu->addAction(raction);

    mHeaderOnlyAttachmentsAction = new KToggleAction(i18nc("View->attachments->", "In Header Only"), this);
    ac->addAction(u"view_attachments_headeronly"_s, mHeaderOnlyAttachmentsAction);
    connect(mHeaderOnlyAttachmentsAction, &QAction::triggered, this, &ViewerPrivate::slotHeaderOnlyAttachments);
    MessageViewer::Util::addHelpTextAction(mHeaderOnlyAttachmentsAction, i18n("Show Attachments only in the header of the mail"));
    group->addAction(mHeaderOnlyAttachmentsAction);
    attachmentMenu->addAction(mHeaderOnlyAttachmentsAction);

    // Set Encoding submenu
    mSelectEncodingAction = new KSelectAction(QIcon::fromTheme(u"character-set"_s), i18n("&Set Encoding"), this);
    mSelectEncodingAction->setToolBarMode(KSelectAction::MenuMode);
    ac->addAction(u"encoding"_s, mSelectEncodingAction);
    connect(mSelectEncodingAction, &KSelectAction::indexTriggered, this, &ViewerPrivate::slotSetEncoding);
    QStringList encodings = MimeTreeParser::NodeHelper::supportedEncodings();
    encodings.prepend(i18n("Auto"));
    mSelectEncodingAction->setItems(encodings);
    mSelectEncodingAction->setCurrentItem(0);

    //
    // Message Menu
    //

    // copy selected text to clipboard
    mCopyAction = ac->addAction(KStandardActions::Copy, u"kmail_copy"_s, this, &ViewerPrivate::slotCopySelectedText);
    mCopyAction->setText(i18n("Copy Text"));

    connect(mViewer, &MailWebEngineView::selectionChanged, this, &ViewerPrivate::viewerSelectionChanged);
    viewerSelectionChanged();

    // copy all text to clipboard
    mSelectAllAction = new QAction(QIcon::fromTheme(u"edit-select-all"_s), i18nc("@action", "Select All Text"), this);
    ac->addAction(u"mark_all_text"_s, mSelectAllAction);
    connect(mSelectAllAction, &QAction::triggered, this, &ViewerPrivate::selectAll);
    ac->setDefaultShortcut(mSelectAllAction, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A));

    // copy Email address to clipboard
    mCopyURLAction = new QAction(QIcon::fromTheme(u"edit-copy"_s), i18n("Copy Link Address"), this);
    ac->addAction(u"copy_url"_s, mCopyURLAction);
    connect(mCopyURLAction, &QAction::triggered, this, &ViewerPrivate::slotUrlCopy);

    // open URL
    mUrlOpenAction = new QAction(QIcon::fromTheme(u"document-open"_s), i18n("Open URL"), this);
    ac->addAction(u"open_url"_s, mUrlOpenAction);
    connect(mUrlOpenAction, &QAction::triggered, this, &ViewerPrivate::slotOpenUrl);

    // use fixed font
    mToggleFixFontAction = new KToggleAction(i18n("Use Fi&xed Font"), this);
    ac->addAction(u"toggle_fixedfont"_s, mToggleFixFontAction);
    connect(mToggleFixFontAction, &QAction::triggered, this, &ViewerPrivate::slotToggleFixedFont);
    ac->setDefaultShortcut(mToggleFixFontAction, QKeySequence(Qt::Key_X));

    // Show message structure viewer
    mToggleMimePartTreeAction = new KToggleAction(i18n("Show Message Structure"), this);
    ac->addAction(u"toggle_mimeparttree"_s, mToggleMimePartTreeAction);
    connect(mToggleMimePartTreeAction, &QAction::toggled, this, &ViewerPrivate::slotToggleMimePartTree);
    QKeyCombination combinationKeys(Qt::CTRL | Qt::ALT, Qt::Key_D);
    ac->setDefaultShortcut(mToggleMimePartTreeAction, combinationKeys);
    mViewSourceAction = new QAction(i18nc("@action", "&View Source"), this);
    ac->addAction(u"view_source"_s, mViewSourceAction);
    connect(mViewSourceAction, &QAction::triggered, this, &ViewerPrivate::slotShowMessageSource);
    ac->setDefaultShortcut(mViewSourceAction, QKeySequence(Qt::Key_V));

    mSaveMessageAction = new QAction(QIcon::fromTheme(u"document-save-as"_s), i18n("&Save Message..."), this);
    ac->addAction(u"save_message"_s, mSaveMessageAction);
    connect(mSaveMessageAction, &QAction::triggered, this, &ViewerPrivate::slotSaveMessage);
    // Laurent: conflict with kmail shortcut
    // mSaveMessageAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));

    mSaveMessageDisplayFormat = new QAction(i18nc("@action", "&Save Display Format"), this);
    ac->addAction(u"save_message_display_format"_s, mSaveMessageDisplayFormat);
    connect(mSaveMessageDisplayFormat, &QAction::triggered, this, &ViewerPrivate::slotSaveMessageDisplayFormat);

    mResetMessageDisplayFormat = new QAction(i18nc("@action", "&Reset Display Format"), this);
    ac->addAction(u"reset_message_display_format"_s, mResetMessageDisplayFormat);
    connect(mResetMessageDisplayFormat, &QAction::triggered, this, &ViewerPrivate::slotResetMessageDisplayFormat);

    //
    // Scroll actions
    //
    mScrollUpAction = new QAction(i18nc("@action", "Scroll Message Up"), this);
    ac->setDefaultShortcut(mScrollUpAction, QKeySequence(Qt::Key_Up));
    ac->addAction(u"scroll_up"_s, mScrollUpAction);
    connect(mScrollUpAction, &QAction::triggered, q, &Viewer::slotScrollUp);

    mScrollDownAction = new QAction(i18nc("@action", "Scroll Message Down"), this);
    ac->setDefaultShortcut(mScrollDownAction, QKeySequence(Qt::Key_Down));
    ac->addAction(u"scroll_down"_s, mScrollDownAction);
    connect(mScrollDownAction, &QAction::triggered, q, &Viewer::slotScrollDown);

    mScrollUpMoreAction = new QAction(i18nc("@action", "Scroll Message Up (More)"), this);
    ac->setDefaultShortcut(mScrollUpMoreAction, QKeySequence(Qt::Key_PageUp));
    ac->addAction(u"scroll_up_more"_s, mScrollUpMoreAction);
    connect(mScrollUpMoreAction, &QAction::triggered, q, &Viewer::slotScrollPrior);

    mScrollDownMoreAction = new QAction(i18nc("@action", "Scroll Message Down (More)"), this);
    ac->setDefaultShortcut(mScrollDownMoreAction, QKeySequence(Qt::Key_PageDown));
    ac->addAction(u"scroll_down_more"_s, mScrollDownMoreAction);
    connect(mScrollDownMoreAction, &QAction::triggered, q, &Viewer::slotScrollNext);

    //
    // Actions not in menu
    //

    // Toggle HTML display mode.
    mToggleDisplayModeAction = new KToggleAction(i18n("Toggle HTML Display Mode"), this);
    ac->addAction(u"toggle_html_display_mode"_s, mToggleDisplayModeAction);
    ac->setDefaultShortcut(mToggleDisplayModeAction, QKeySequence(Qt::SHIFT | Qt::Key_H));
    connect(mToggleDisplayModeAction, &QAction::triggered, this, &ViewerPrivate::slotToggleHtmlMode);
    MessageViewer::Util::addHelpTextAction(mToggleDisplayModeAction, i18n("Toggle display mode between HTML and plain text"));

    // Load external reference
    auto loadExternalReferenceAction = new QAction(i18nc("@action", "Load external references"), this);
    ac->addAction(u"load_external_reference"_s, loadExternalReferenceAction);
    ac->setDefaultShortcut(loadExternalReferenceAction, QKeySequence(Qt::SHIFT | Qt::CTRL | Qt::Key_R));
    connect(loadExternalReferenceAction, &QAction::triggered, this, &ViewerPrivate::slotLoadExternalReference);
    MessageViewer::Util::addHelpTextAction(loadExternalReferenceAction, i18n("Load external references from the Internet for this message."));
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
    mSpeakTextAction = new QAction(i18nc("@action", "Speak Text"), this);
    mSpeakTextAction->setIcon(QIcon::fromTheme(u"preferences-desktop-text-to-speech"_s));
    ac->addAction(u"speak_text"_s, mSpeakTextAction);
    connect(mSpeakTextAction, &QAction::triggered, this, &ViewerPrivate::slotSpeakText);
#endif
    auto purposeMenuWidget = new ViewerPurposeMenuWidget(mViewer, this);
    mShareTextAction = new QAction(i18nc("@action", "Share Text..."), this);
    mShareTextAction->setMenu(purposeMenuWidget->menu());
    mShareTextAction->setIcon(QIcon::fromTheme(u"document-share"_s));
    ac->addAction(u"purpose_share_text_menu"_s, mShareTextAction);
    purposeMenuWidget->setViewer(mViewer);
    connect(purposeMenuWidget, &ViewerPurposeMenuWidget::shareError, this, [this](const QString &message) {
        if (!mPurposeMenuMessageWidget) {
            createPurposeMenuMessageWidget();
        }
        mPurposeMenuMessageWidget->slotShareError(message);
    });
    connect(purposeMenuWidget, &ViewerPurposeMenuWidget::shareSuccess, this, [this](const QString &message) {
        if (!mPurposeMenuMessageWidget) {
            createPurposeMenuMessageWidget();
        }
        mPurposeMenuMessageWidget->slotShareSuccess(message);
    });

    mCopyImageLocation = new QAction(i18nc("@action", "Copy Image Location"), this);
    mCopyImageLocation->setIcon(QIcon::fromTheme(u"view-media-visualization"_s));
    ac->addAction(u"copy_image_location"_s, mCopyImageLocation);
    ac->setShortcutsConfigurable(mCopyImageLocation, false);
    connect(mCopyImageLocation, &QAction::triggered, this, &ViewerPrivate::slotCopyImageLocation);

    mFindInMessageAction = new QAction(QIcon::fromTheme(u"edit-find"_s), i18n("&Find in Message..."), this);
    ac->addAction(u"find_in_messages"_s, mFindInMessageAction);
    connect(mFindInMessageAction, &QAction::triggered, this, &ViewerPrivate::slotFind);
    ac->setDefaultShortcut(mFindInMessageAction, KStandardShortcut::find().first());

    mShareServiceUrlMenu = mShareServiceManager->menu();
    ac->addAction(u"shareservice_menu"_s, mShareServiceUrlMenu);
    connect(mShareServiceManager, &PimCommon::ShareServiceUrlManager::serviceUrlSelected, this, &ViewerPrivate::slotServiceUrlSelected);

    mDisableEmoticonAction = new KToggleAction(i18n("Disable Emoticon"), this);
    ac->addAction(u"disable_emoticon"_s, mDisableEmoticonAction);
    connect(mDisableEmoticonAction, &QAction::triggered, this, &ViewerPrivate::slotToggleEmoticons);

    // Don't translate it.
    mDevelopmentToolsAction = new QAction(u"Development Tools"_s, this);
    ac->addAction(u"development_tools"_s, mDevelopmentToolsAction);
    ac->setDefaultShortcut(mDevelopmentToolsAction, QKeySequence(Qt::SHIFT | Qt::CTRL | Qt::Key_I));

    connect(mDevelopmentToolsAction, &QAction::triggered, this, &ViewerPrivate::slotShowDevelopmentTools);
}

void ViewerPrivate::createShowNextMessageWidget()
{
    mShowNextMessageWidget = new MessageViewer::ShowNextMessageWidget(mReaderBox);
    mShowNextMessageWidget->setObjectName(QLatin1StringView("shownextmessagewidget"));
    mReaderBoxVBoxLayout->insertWidget(0, mShowNextMessageWidget);
    connect(mShowNextMessageWidget, &ShowNextMessageWidget::showPreviousMessage, this, &ViewerPrivate::showPreviousMessage);
    connect(mShowNextMessageWidget, &ShowNextMessageWidget::showNextMessage, this, &ViewerPrivate::showNextMessage);
}

void ViewerPrivate::createPurposeMenuMessageWidget()
{
    mPurposeMenuMessageWidget = new PimCommon::PurposeMenuMessageWidget(mReaderBox);
    mPurposeMenuMessageWidget->setPosition(KMessageWidget::Header);
    mPurposeMenuMessageWidget->setObjectName(QLatin1StringView("mPurposeMenuMessageWidget"));
    mReaderBoxVBoxLayout->insertWidget(0, mPurposeMenuMessageWidget);
}

void ViewerPrivate::slotShowDevelopmentTools()
{
    if (!mDeveloperToolDialog) {
        mDeveloperToolDialog = new WebEngineViewer::DeveloperToolDialog(nullptr);
        mViewer->page()->setDevToolsPage(mDeveloperToolDialog->enginePage());
        mViewer->page()->triggerAction(QWebEnginePage::InspectElement);
        connect(mDeveloperToolDialog,
                &WebEngineViewer::DeveloperToolDialog::rejected,
                mDeveloperToolDialog,
                &WebEngineViewer::DeveloperToolDialog::deleteLater);
    }
    if (mDeveloperToolDialog->isHidden()) {
        mDeveloperToolDialog->show();
    }

    mDeveloperToolDialog->raise();
    mDeveloperToolDialog->activateWindow();
}

void ViewerPrivate::showContextMenu(KMime::Content *content, const QPoint &pos)
{
    if (!content) {
        return;
    }

    if (auto ct = content->contentType(false)) {
        if (ct->mimeType() == "text/x-moz-deleted") {
            return;
        }
    }
    const bool isAttachment = !content->contentType()->isMultipart();
    const bool isExtraContent = !mMessage->content(content->index());
    const auto hasAttachments = KMime::hasAttachment(mMessage.data());

    QMenu popup;

    if (!content->isTopLevel() || isAttachment) {
        popup.addAction(QIcon::fromTheme(u"document-save-as"_s), i18n("Save &As..."), this, &ViewerPrivate::slotAttachmentSaveAs);

        if (isAttachment) {
            popup.addAction(QIcon::fromTheme(u"document-open"_s), i18nc("to open", "Open"), this, &ViewerPrivate::slotAttachmentOpen);

            if (selectedContents().count() == 1) {
                createOpenWithMenu(&popup, QLatin1StringView(content->contentType()->mimeType()), false);
            } else {
                popup.addAction(i18n("Open With..."), this, &ViewerPrivate::slotAttachmentOpenWith);
            }
            popup.addAction(i18nc("to view something", "View"), this, &ViewerPrivate::slotAttachmentView);
        }
    }

    if (hasAttachments) {
        popup.addAction(i18n("Save All Attachments..."), this, &ViewerPrivate::slotAttachmentSaveAll);
    }

    if (!content->isTopLevel()) {
        if (isAttachment) {
            popup.addAction(QIcon::fromTheme(u"edit-copy"_s), i18n("Copy"), this, &ViewerPrivate::slotAttachmentCopy);
        }

        popup.addSeparator();
        auto deleteAction = popup.addAction(QIcon::fromTheme(u"edit-delete"_s), i18n("Delete Attachment"), this, &ViewerPrivate::slotAttachmentDelete);
        // body parts can only be deleted one at a time, and extra content cannot be delete
        deleteAction->setEnabled(selectedContents().size() == 1 && !isExtraContent);

        popup.addSeparator();
        popup.addAction(i18n("Properties"), this, &ViewerPrivate::slotAttachmentProperties);
    }
    popup.exec(mMimePartTree->viewport()->mapToGlobal(pos));
}

KToggleAction *ViewerPrivate::actionForAttachmentStrategy(const AttachmentStrategy *as)
{
    if (!mActionCollection) {
        return nullptr;
    }
    QString actionName;
    if (as == AttachmentStrategy::iconic()) {
        actionName = u"view_attachments_as_icons"_s;
    } else if (as == AttachmentStrategy::smart()) {
        actionName = u"view_attachments_smart"_s;
    } else if (as == AttachmentStrategy::inlined()) {
        actionName = u"view_attachments_inline"_s;
    } else if (as == AttachmentStrategy::hidden()) {
        actionName = u"view_attachments_hide"_s;
    } else if (as == AttachmentStrategy::headerOnly()) {
        actionName = u"view_attachments_headeronly"_s;
    } else {
        qCWarning(MESSAGEVIEWER_LOG) << "actionForAttachmentStrategy invalid attachment type";
    }

    if (actionName.isEmpty()) {
        return nullptr;
    } else {
        return static_cast<KToggleAction *>(mActionCollection->action(actionName));
    }
}

void ViewerPrivate::readGlobalOverrideCodec()
{
    // if the global character encoding wasn't changed then there's nothing to do
    if (MessageCore::MessageCoreSettings::self()->overrideCharacterEncoding() == mOldGlobalOverrideEncoding) {
        return;
    }

    setOverrideEncoding(MessageCore::MessageCoreSettings::self()->overrideCharacterEncoding());
    mOldGlobalOverrideEncoding = MessageCore::MessageCoreSettings::self()->overrideCharacterEncoding();
}

QByteArray ViewerPrivate::overrideCodecName() const
{
    if (!mOverrideEncoding.isEmpty() && mOverrideEncoding != QLatin1StringView("Auto")) { // Auto
        QStringDecoder codec(mOverrideEncoding.toUtf8().constData());
        if (!codec.isValid()) {
            return "UTF-8";
        }
        return mOverrideEncoding.toUtf8();
    }
    return {};
}

static QColor nextColor(const QColor &c)
{
    int h;
    int s;
    int v;
    c.getHsv(&h, &s, &v);
    return QColor::fromHsv((h + 50) % 360, qMax(s, 64), v);
}

[[nodiscard]] static KMime::Content *nextSibling(KMime::Content *node)
{
    if (KMime::Content *parent = node->parent()) {
        const auto contents = parent->contents();
        const int index = contents.indexOf(node) + 1;
        if (index < contents.size()) { // next on the same level
            return contents.at(index);
        }
    }
    return nullptr;
}

QString ViewerPrivate::renderAttachments(KMime::Content *node, const QColor &bgColor) const
{
    if (!node) {
        return {};
    }

    QString html;
    KMime::Content *child = node->contents().isEmpty() ? nullptr : node->contents().at(0);

    if (child) {
        const QString subHtml = renderAttachments(child, nextColor(bgColor));
        if (!subHtml.isEmpty()) {
            QString margin;
            if (node != mMessage.data() || headerStylePlugin()->hasMargin()) {
                margin = u"padding:2px; margin:2px; "_s;
            }
            const QString align = headerStylePlugin()->alignment();
            const QByteArray mediaTypeLower = node->contentType()->mediaType().toLower();
            const bool result = (mediaTypeLower == "message" || mediaTypeLower == "multipart" || node == mMessage.data());
            if (result) {
                html += QStringLiteral(
                            "<div style=\"background:%1; %2"
                            "vertical-align:middle; float:%3;\">")
                            .arg(bgColor.name())
                            .arg(margin, align);
            }
            html += subHtml;
            if (result) {
                html += QLatin1StringView("</div>");
            }
        }
    } else {
        Util::AttachmentDisplayInfo info = Util::attachmentDisplayInfo(node);
        if (info.displayInHeader) {
            html += QLatin1StringView("<div style=\"float:left;\">");
            html += QStringLiteral(
                        "<span style=\"white-space:nowrap; border-width: 0px; border-left-width: 5px; border-color: %1; 2px; border-left-style: solid;\">")
                        .arg(bgColor.name());
            mNodeHelper->writeNodeToTempFile(node);
            const QString href = mNodeHelper->asHREF(node, u"header"_s);
            html += QLatin1StringView("<a href=\"") + href + QLatin1StringView("\">");
            const QString imageMaxSize = u"width=\"16\" height=\"16\""_s;
#if 0
            if (!info.icon.isEmpty()) {
                QImage tmpImg(info.icon);
                if (tmpImg.width() > 48 || tmpImg.height() > 48) {
                    imageMaxSize = u"width=\"48\" height=\"48\""_s;
                }
            }
#endif
            html += u"<img %1 style=\"vertical-align:middle;\" src=\""_s.arg(imageMaxSize) + info.icon + QLatin1StringView("\"/>&nbsp;");
            const int elidedTextSize = headerStylePlugin()->elidedTextSize();
            if (elidedTextSize == -1) {
                html += info.label;
            } else {
                QFont bodyFont = cssHelper()->bodyFont(mHtmlHeadSettings.fixedFont);
                QFontMetrics fm(bodyFont);
                html += fm.elidedText(info.label, Qt::ElideRight, elidedTextSize);
            }
            html += QLatin1StringView("</a></span></div> ");
        }
    }

    for (KMime::Content *extraNode : mNodeHelper->extraContents(node)) {
        html += renderAttachments(extraNode, bgColor);
    }

    KMime::Content *next = nextSibling(node);
    if (next) {
        html += renderAttachments(next, nextColor(bgColor));
    }

    return html;
}

KMime::Content *ViewerPrivate::findContentByType(KMime::Content *content, const QByteArray &type)
{
    const auto list = content->contents();
    for (KMime::Content *c : list) {
        if (c->contentType()->mimeType() == type) {
            return c;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
void ViewerPrivate::update(MimeTreeParser::UpdateMode updateMode)
{
    // Avoid flicker, somewhat of a cludge
    if (updateMode == MimeTreeParser::Force) {
        // stop the timer to avoid calling updateReaderWin twice
        mUpdateReaderWinTimer.stop();
        saveRelativePosition();
        updateReaderWin();
    } else if (mUpdateReaderWinTimer.isActive()) {
        mUpdateReaderWinTimer.setInterval(150ms);
    } else {
        mUpdateReaderWinTimer.start(0);
    }
}

void ViewerPrivate::slotOpenUrl()
{
    slotUrlOpen();
}

void ViewerPrivate::slotUrlOpen(const QUrl &url)
{
    if (!url.isEmpty()) {
        mClickedUrl = url;
    }
    const OpenWithUrlInfo openWithInfo = OpenUrlWithManager::self()->openWith(url);
    if (openWithInfo.isValid()) {
        auto job = new OpenUrlWithJob(this);
        job->setInfo(openWithInfo);
        job->setUrl(mClickedUrl);
        job->start();
        return;
    }
    // First, let's see if the URL handler manager can handle the URL. If not, try KRun for some
    // known URLs, otherwise fallback to emitting a signal.
    // That signal is caught by KMail, and in case of mailto URLs, a composer is shown.

    if (URLHandlerManager::instance()->handleClick(mClickedUrl, this)) {
        return;
    }
    Q_EMIT urlClicked(mMessageItem, mClickedUrl);
}

void ViewerPrivate::checkPhishingUrl()
{
    if (MessageViewer::MessageViewerSettings::self()->checkPhishingUrl() && (mClickedUrl.scheme() != QLatin1StringView("mailto"))) {
        mPhishingDatabase->checkUrl(mClickedUrl);
    } else {
        executeRunner(mClickedUrl);
    }
}

void ViewerPrivate::executeRunner(const QUrl &url)
{
    if (!MessageViewer::Util::handleUrlWithQDesktopServices(url)) {
        auto job = new KIO::OpenUrlJob(url);
        job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, viewer()));
        job->setRunExecutables(false);
        job->start();
    }
}

void ViewerPrivate::slotCheckedUrlFinished(const QUrl &url, WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status)
{
    switch (status) {
    case WebEngineViewer::CheckPhishingUrlUtil::BrokenNetwork:
        KMessageBox::error(mMainWindow, i18n("The network is broken."), i18nc("@title:window", "Check Phishing URL"));
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::InvalidUrl:
        KMessageBox::error(mMainWindow, i18n("The URL %1 is not valid.", url.toString()), i18nc("@title:window", "Check Phishing URL"));
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::Ok:
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::MalWare:
        if (!urlIsAMalwareButContinue()) {
            return;
        }
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::Unknown:
        qCWarning(MESSAGEVIEWER_LOG) << "WebEngineViewer::slotCheckedUrlFinished unknown error ";
        break;
    }
    executeRunner(url);
}

bool ViewerPrivate::urlIsAMalwareButContinue()
{
    if (KMessageBox::ButtonCode::SecondaryAction
        == KMessageBox::warningTwoActions(mMainWindow,
                                          i18n("This web site is a malware, do you want to continue to show it?"),
                                          i18nc("@title:window", "Malware"),
                                          KStandardGuiItem::cont(),
                                          KStandardGuiItem::cancel())) {
        return false;
    }
    return true;
}

void ViewerPrivate::slotUrlOn(const QString &link)
{
    // The "link" we get here is not URL-encoded, and therefore there is no way QUrl could
    // parse it correctly. To workaround that, we use QWebFrame::hitTestContent() on the mouse position
    // to get the URL before WebKit managed to mangle it.
    QUrl url(link);
    const QString protocol = url.scheme();
    if (protocol == QLatin1StringView("kmail") || protocol == QLatin1StringView("x-kmail") || protocol == QLatin1StringView("attachment")
        || (protocol.isEmpty() && url.path().isEmpty())) {
        mViewer->setAcceptDrops(false);
    } else {
        mViewer->setAcceptDrops(true);
    }

    mViewer->setLinkHovered(url);
    if (link.trimmed().isEmpty()) {
        PimCommon::BroadcastStatus::instance()->reset();
        Q_EMIT showStatusBarMessage(QString());
        return;
    }

    QString msg = URLHandlerManager::instance()->statusBarMessage(url, this);
    if (msg.isEmpty()) {
        msg = link;
    }

    Q_EMIT showStatusBarMessage(msg);
}

void ViewerPrivate::slotUrlPopup(const WebEngineViewer::WebHitTestResult &result)
{
    if (!mMsgDisplay) {
        return;
    }
    mClickedUrl = result.linkUrl();
    mImageUrl = result.imageUrl();
    const QPoint aPos = mViewer->mapToGlobal(result.pos());
    if (URLHandlerManager::instance()->handleContextMenuRequest(mClickedUrl, aPos, this)) {
        return;
    }

    if (!mActionCollection) {
        return;
    }

    if (mClickedUrl.scheme() == QLatin1StringView("mailto")) {
        mCopyURLAction->setText(i18n("Copy Email Address"));
    } else {
        mCopyURLAction->setText(i18n("Copy Link Address"));
    }
    Q_EMIT displayPopupMenu(mMessageItem, result, aPos);
    Q_EMIT popupMenu(mMessageItem, mClickedUrl, mImageUrl, aPos);
}

void ViewerPrivate::slotLoadExternalReference()
{
    if (mColorBar->isNormal() || htmlLoadExtOverride()) {
        return;
    }
    setHtmlLoadExtOverride(true);
    update(MimeTreeParser::Force);
}

static Viewer::DisplayFormatMessage translateToDisplayFormat(MimeTreeParser::Util::HtmlMode mode)
{
    switch (mode) {
    case MimeTreeParser::Util::Normal:
        return Viewer::Unknown;
    case MimeTreeParser::Util::Html:
        return Viewer::Html;
    case MimeTreeParser::Util::MultipartPlain:
        return Viewer::Text;
    case MimeTreeParser::Util::MultipartHtml:
        return Viewer::Html;
    case MimeTreeParser::Util::MultipartIcal:
        return Viewer::ICal;
    }
    return Viewer::Unknown;
}

void ViewerPrivate::slotToggleHtmlMode()
{
    const auto availableModes = mColorBar->availableModes();
    const int availableModeSize(availableModes.size());
    if (mColorBar->isNormal() || availableModeSize < 2) {
        return;
    }
    if (mScamDetectionWarning) {
        mScamDetectionWarning->setVisible(false);
    }
    const MimeTreeParser::Util::HtmlMode mode = mColorBar->mode();
    const int pos = (availableModes.indexOf(mode) + 1) % availableModeSize;
    setDisplayFormatMessageOverwrite(translateToDisplayFormat(availableModes[pos]));
    update(MimeTreeParser::Force);
    mColorBar->setAvailableModes(availableModes);
}

void ViewerPrivate::slotFind()
{
    if (mViewer->hasSelection()) {
        mFindBar->setText(mViewer->selectedText());
    }
    mSliderContainer->slideIn();
    mFindBar->focusAndSetCursor();
}

void ViewerPrivate::slotToggleFixedFont()
{
    mHtmlHeadSettings.fixedFont = !mHtmlHeadSettings.fixedFont;
    update(MimeTreeParser::Force);
}

void ViewerPrivate::slotToggleMimePartTree()
{
    if (mToggleMimePartTreeAction->isChecked()) {
        MessageViewer::MessageViewerSettings::self()->setMimeTreeMode2(MessageViewer::MessageViewerSettings::EnumMimeTreeMode2::Always);
    } else {
        MessageViewer::MessageViewerSettings::self()->setMimeTreeMode2(MessageViewer::MessageViewerSettings::EnumMimeTreeMode2::Never);
    }
    showHideMimeTree();
}

void ViewerPrivate::slotShowMessageSource()
{
    if (!mMessage) {
        return;
    }
    QPointer<MailSourceWebEngineViewer> viewer = new MailSourceWebEngineViewer; // deletes itself upon close
    mListMailSourceViewer.append(viewer);
    viewer->setWindowTitle(i18nc("@title:window", "Message as Plain Text"));
    const QString rawMessage = QString::fromLatin1(mMessage->encodedContent());
    viewer->setRawSource(rawMessage);
    viewer->setDisplayedSource(mViewer->page());
    if (mHtmlHeadSettings.fixedFont) {
        viewer->setFixedFont();
    }
    viewer->show();
}

void ViewerPrivate::updateReaderWin()
{
    if (!mMsgDisplay) {
        return;
    }

    if (mRecursionCountForDisplayMessage + 1 > 1) {
        // This recursion here can happen because the ObjectTreeParser in parseMsg() can exec() an
        // eventloop.
        // This happens in two cases:
        //   1) The ContactSearchJob started by FancyHeaderStyle::format
        //   2) Various modal passphrase dialogs for decryption of a message (bug 96498)
        //
        // While the exec() eventloop is running, it is possible that a timer calls updateReaderWin(),
        // and not aborting here would confuse the state terribly.
        qCWarning(MESSAGEVIEWER_LOG) << "Danger, recursion while displaying a message!";
        return;
    }
    mRecursionCountForDisplayMessage++;

    if (mViewer) {
        mViewer->setAllowExternalContent(htmlLoadExternal());
        htmlWriter()->reset();
        // TODO: if the item doesn't have the payload fetched, try to fetch it? Maybe not here, but in setMessageItem.
        if (mMessage) {
            mColorBar->show();
            displayMessage();
        } else if (mMessagePartNode) {
            setMessagePart(mMessagePartNode);
        } else {
            mColorBar->hide();
            mMimePartTree->hide();
            htmlWriter()->begin();
            htmlWriter()->write(cssHelper()->htmlHead(mHtmlHeadSettings) + cssHelper()->endBodyHtml());
            htmlWriter()->end();
        }
    }
    mRecursionCountForDisplayMessage--;
}

void ViewerPrivate::slotMimePartSelected(const QModelIndex &index)
{
    auto content = static_cast<KMime::Content *>(index.internalPointer());
    if (!mMimePartTree->mimePartModel()->parent(index).isValid() && index.row() == 0) {
        update(MimeTreeParser::Force);
    } else {
        setMessagePart(content);
    }
}

void ViewerPrivate::slotIconicAttachments()
{
    setAttachmentStrategy(AttachmentStrategy::iconic());
}

void ViewerPrivate::slotSmartAttachments()
{
    setAttachmentStrategy(AttachmentStrategy::smart());
}

void ViewerPrivate::slotInlineAttachments()
{
    setAttachmentStrategy(AttachmentStrategy::inlined());
}

void ViewerPrivate::slotHideAttachments()
{
    setAttachmentStrategy(AttachmentStrategy::hidden());
}

void ViewerPrivate::slotHeaderOnlyAttachments()
{
    setAttachmentStrategy(AttachmentStrategy::headerOnly());
}

void ViewerPrivate::attachmentView(KMime::Content *atmNode)
{
    if (atmNode) {
        const bool isEncapsulatedMessage = atmNode->parent() && atmNode->parent()->bodyIsMessage();
        if (isEncapsulatedMessage) {
            attachmentViewMessage(atmNode->parent()->bodyAsMessage());
        } else if ((qstricmp(atmNode->contentType()->mediaType().constData(), "text") == 0)
                   && ((qstricmp(atmNode->contentType()->subType().constData(), "x-vcard") == 0)
                       || (qstricmp(atmNode->contentType()->subType().constData(), "directory") == 0))) {
            setMessagePart(atmNode);
        } else {
            Q_EMIT showReader(atmNode, htmlMail(), overrideEncoding());
        }
    }
}

void ViewerPrivate::slotDelayedResize()
{
    mSplitter->setGeometry(0, 0, q->width(), q->height());
}

void ViewerPrivate::slotPrintPreview()
{
    disconnect(mPartHtmlWriter.data(), &WebEnginePartHtmlWriter::finished, this, &ViewerPrivate::slotPrintPreview);
    if (!mMessage) {
        return;
    }
    // Need to delay
    QTimer::singleShot(1s, this, &ViewerPrivate::slotDelayPrintPreview); // 1 second
}

void ViewerPrivate::slotDelayPrintPreview()
{
    auto printMessage = new PrintMessage(this);
    printMessage->setParentWidget(q);
    printMessage->setView(mViewer);
    printMessage->printPreview();
    connect(printMessage, &PrintMessage::printingFinished, this, &ViewerPrivate::printingFinished);
}

void ViewerPrivate::exportToPdf(const QString &fileName)
{
    auto job = new WebEngineViewer::WebEngineExportPdfPageJob(this);
    connect(job, &WebEngineViewer::WebEngineExportPdfPageJob::exportToPdfSuccess, this, [this, fileName]() {
        showSavedFileFolderWidget({QUrl::fromLocalFile(fileName)}, MessageViewer::OpenSavedFileFolderWidget::FileType::Pdf);
    });
    job->setEngineView(mViewer);
    job->setPdfPath(fileName);
    job->start();
}

static QString filterCharsFromFilename(const QString &name)
{
    QString value = name;

    value.replace(u'/', u'-');
    value.remove(u'\\');
    value.remove(u':');
    value.remove(u'*');
    value.remove(u'?');
    value.remove(u'"');
    value.remove(u'<');
    value.remove(u'>');
    value.remove(u'|');

    return value;
}

void ViewerPrivate::slotPrintMessage()
{
    disconnect(mPartHtmlWriter.data(), &WebEnginePartHtmlWriter::finished, this, &ViewerPrivate::slotPrintMessage);

    if (!mMessage) {
        return;
    }
    auto printMessage = new PrintMessage(this);
    connect(printMessage, &PrintMessage::printingFinished, this, &ViewerPrivate::printingFinished);
    printMessage->setParentWidget(q);
    printMessage->setView(mViewer);
    printMessage->setDocumentName(filterCharsFromFilename(mMessage->subject()->asUnicodeString()));
    printMessage->print();
}

void ViewerPrivate::slotSetEncoding()
{
    if (mSelectEncodingAction) {
        if (mSelectEncodingAction->currentItem() == 0) { // Auto
            mOverrideEncoding.clear();
        } else {
            mOverrideEncoding = MimeTreeParser::NodeHelper::encodingForName(mSelectEncodingAction->currentText());
        }
        update(MimeTreeParser::Force);
    }
}

HeaderStylePlugin *ViewerPrivate::headerStylePlugin() const
{
    return mHeaderStylePlugin;
}

void ViewerPrivate::updatePalette()
{
    updateColorFromScheme();
    cssHelper()->updateColor();
    recreateCssHelper();
    mDkimWidgetInfo->updatePalette();
    // update(MimeTreeParser::Force);
}

void ViewerPrivate::updateColorFromScheme()
{
    const KColorScheme scheme = KColorScheme(QPalette::Active, KColorScheme::View);
    mForegroundError = scheme.foreground(KColorScheme::NegativeText).color();
    mBackgroundError = scheme.background(KColorScheme::NegativeBackground).color();
    mBackgroundAttachment = scheme.background().color();
}

void ViewerPrivate::createMdnWarningWidget()
{
    mMdnWarning = new MDNWarningWidget(mReaderBox);
    mMdnWarning->setObjectName(QLatin1StringView("mMdnWarning"));
    mReaderBoxVBoxLayout->insertWidget(0, mMdnWarning);
}

void ViewerPrivate::mdnWarningAnimatedHide()
{
    if (!mMdnWarning) {
        createMdnWarningWidget();
    }
    mMdnWarning->animatedHide();
}

void ViewerPrivate::showMdnInformations(const QPair<QString, bool> &mdnInfo)
{
    if (!mMdnWarning) {
        createMdnWarningWidget();
    }
    if (!mdnInfo.first.isEmpty()) {
        mMdnWarning->setCanDeny(mdnInfo.second);
        mMdnWarning->setInformation(mdnInfo.first);
    } else {
        mMdnWarning->animatedHide();
    }
}

void ViewerPrivate::initializeColorFromScheme()
{
    if (!mForegroundError.isValid()) {
        updateColorFromScheme();
    }
}

QString ViewerPrivate::attachmentHtml()
{
    initializeColorFromScheme();
    QString html = renderAttachments(mMessage.data(), mBackgroundAttachment);
    if (!html.isEmpty()) {
        html.prepend(headerStylePlugin()->attachmentHtml());
    }
    return html;
}

void ViewerPrivate::executeCustomScriptsAfterLoading()
{
    disconnect(mViewer, &MailWebEngineView::loadFinished, this, &ViewerPrivate::executeCustomScriptsAfterLoading);
    // inject attachments in header view
    // we have to do that after the otp has run so we also see encrypted parts

    mViewer->scrollToRelativePosition(mViewer->relativePosition());
    mViewer->clearRelativePosition();
}

void ViewerPrivate::slotSettingsChanged()
{
    update(MimeTreeParser::Force);
}

void ViewerPrivate::slotMimeTreeContextMenuRequested(const QPoint &pos)
{
    const QModelIndex index = mMimePartTree->indexAt(pos);
    if (index.isValid()) {
        auto content = static_cast<KMime::Content *>(index.internalPointer());
        showContextMenu(content, pos);
    }
}

void ViewerPrivate::slotAttachmentOpenWith()
{
    QItemSelectionModel *selectionModel = mMimePartTree->selectionModel();
    const QModelIndexList selectedRows = selectionModel->selectedRows();

    for (const QModelIndex &index : selectedRows) {
        auto content = static_cast<KMime::Content *>(index.internalPointer());
        attachmentOpenWith(content);
    }
}

void ViewerPrivate::slotAttachmentOpen()
{
    QItemSelectionModel *selectionModel = mMimePartTree->selectionModel();
    const QModelIndexList selectedRows = selectionModel->selectedRows();

    for (const QModelIndex &index : selectedRows) {
        auto content = static_cast<KMime::Content *>(index.internalPointer());
        attachmentOpen(content);
    }
}

void ViewerPrivate::showSavedFileFolderWidget(const QList<QUrl> &urls, OpenSavedFileFolderWidget::FileType fileType)
{
    if (!mOpenSavedFileFolderWidget) {
        createOpenSavedFileFolderWidget();
    }
    mOpenSavedFileFolderWidget->setUrls(urls, fileType);
    mOpenSavedFileFolderWidget->slotShowWarning();
}

bool ViewerPrivate::mimePartTreeIsEmpty() const
{
    return mMimePartTree->model()->rowCount() == 0;
}

void ViewerPrivate::setPluginName(const QString &pluginName)
{
    mHeaderStyleMenuManager->setPluginName(pluginName);
}

QList<QAction *> ViewerPrivate::viewerPluginActionList(ViewerPluginInterface::SpecificFeatureTypes features)
{
    if (mViewerPluginToolManager) {
        return mViewerPluginToolManager->viewerPluginActionList(features);
    }
    return {};
}

void ViewerPrivate::slotActivatePlugin(ViewerPluginInterface *interface)
{
    interface->setMessage(mMessage);
    interface->setMessageItem(mMessageItem);
    interface->setUrl(mClickedUrl);
    interface->setCurrentCollection(mMessageItem.parentCollection());
    const QString text = mViewer->selectedText();
    if (!text.isEmpty()) {
        interface->setText(text);
    }
    interface->execute();
}

void ViewerPrivate::slotAttachmentSaveAs()
{
    const auto contents = selectedContents();
    QList<QUrl> urlList;
    if (Util::saveAttachments(contents, mMainWindow, urlList)) {
        showSavedFileFolderWidget(urlList, MessageViewer::OpenSavedFileFolderWidget::FileType::Attachment);
    }
}

void ViewerPrivate::slotAttachmentSaveAll()
{
    const auto contents = mMessage->attachments();
    QList<QUrl> urlList;
    if (Util::saveAttachments(contents, mMainWindow, urlList)) {
        showSavedFileFolderWidget(urlList, MessageViewer::OpenSavedFileFolderWidget::FileType::Attachment);
    }
}

void ViewerPrivate::slotAttachmentView()
{
    const auto contents = selectedContents();

    for (KMime::Content *content : contents) {
        attachmentView(content);
    }
}

void ViewerPrivate::slotAttachmentProperties()
{
    const auto contents = selectedContents();

    for (KMime::Content *content : contents) {
        attachmentProperties(content);
    }
}

void ViewerPrivate::attachmentProperties(KMime::Content *content)
{
    auto dialog = new MessageCore::AttachmentPropertiesDialog(content, mMainWindow);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void ViewerPrivate::slotAttachmentCopy()
{
#ifndef QT_NO_CLIPBOARD
    attachmentCopy(selectedContents());
#endif
}

void ViewerPrivate::attachmentCopy(const KMime::Content::List &contents)
{
#ifndef QT_NO_CLIPBOARD
    if (contents.isEmpty()) {
        return;
    }

    QList<QUrl> urls;
    for (KMime::Content *content : contents) {
        auto url = QUrl::fromLocalFile(mNodeHelper->writeNodeToTempFile(content));
        if (!url.isValid()) {
            continue;
        }
        urls.append(url);
    }

    if (urls.isEmpty()) {
        return;
    }

    auto mimeData = new QMimeData;
    mimeData->setUrls(urls);
    QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
#endif
}

void ViewerPrivate::slotAttachmentDelete()
{
    const auto contents = selectedContents();
    if (contents.size() != 1) {
        return;
    }
    // look up the selected content node of the mime part tree in the node tree of the original message;
    // since deleting extra content (e.g. attachments inside encrypted message parts) is not supported,
    // we do not need to consider the extra content in the lookup
    const auto contentIndex = contents[0]->index();
    const auto contentInOriginalMessage = mMessage->content(contentIndex);
    if (contentInOriginalMessage) {
        (void)deleteAttachment(contentInOriginalMessage);
    }
}

void ViewerPrivate::slotLevelQuote(int l)
{
    if (mLevelQuote != l) {
        mLevelQuote = l;
        update(MimeTreeParser::Force);
    }
}

void ViewerPrivate::slotHandleAttachment(int choice)
{
    if (!mCurrentContent) {
        return;
    }
    switch (choice) {
    case Viewer::Delete:
        if (!deleteAttachment(mCurrentContent)) {
            qCWarning(MESSAGEVIEWER_LOG) << "Impossible to delete attachment";
        }
        break;
    case Viewer::Properties:
        attachmentProperties(mCurrentContent);
        break;
    case Viewer::Save: {
        const bool isEncapsulatedMessage = mCurrentContent->parent() && mCurrentContent->parent()->bodyIsMessage();
        if (isEncapsulatedMessage) {
            KMime::Message::Ptr message(new KMime::Message);
            message->setContent(mCurrentContent->parent()->bodyAsMessage()->encodedContent());
            message->parse();
            Akonadi::Item item;
            item.setPayload<KMime::Message::Ptr>(message);
            Akonadi::MessageFlags::copyMessageFlags(*message, item);
            item.setMimeType(KMime::Message::mimeType());
            QUrl url;
            if (MessageViewer::Util::saveMessageInMboxAndGetUrl(url, Akonadi::Item::List() << item, mMainWindow)) {
                showSavedFileFolderWidget({url}, MessageViewer::OpenSavedFileFolderWidget::FileType::Attachment);
            }
        } else {
            QList<QUrl> urlList;
            if (Util::saveContents(mMainWindow, KMime::Content::List() << mCurrentContent, urlList)) {
                showSavedFileFolderWidget(urlList, MessageViewer::OpenSavedFileFolderWidget::FileType::Attachment);
            }
        }
        break;
    }
    case Viewer::OpenWith:
        attachmentOpenWith(mCurrentContent);
        break;
    case Viewer::Open:
        attachmentOpen(mCurrentContent);
        break;
    case Viewer::View:
        attachmentView(mCurrentContent);
        break;
    case Viewer::Copy:
        attachmentCopy(KMime::Content::List() << mCurrentContent);
        break;
    case Viewer::ScrollTo:
        scrollToAttachment(mCurrentContent);
        break;
    case Viewer::ReplyMessageToAuthor:
        replyMessageToAuthor(mCurrentContent);
        break;
    case Viewer::ReplyMessageToAll:
        replyMessageToAll(mCurrentContent);
        break;
    }
}

void ViewerPrivate::replyMessageToAuthor(KMime::Content *atmNode)
{
    replyMessage(atmNode, false);
}

void ViewerPrivate::replyMessageToAll(KMime::Content *atmNode)
{
    replyMessage(atmNode, true);
}

void ViewerPrivate::replyMessage(KMime::Content *atmNode, bool replyToAll)
{
    if (atmNode) {
        const bool isEncapsulatedMessage = atmNode->parent() && atmNode->parent()->bodyIsMessage();
        if (isEncapsulatedMessage) {
            Q_EMIT replyMessageTo(atmNode->parent()->bodyAsMessage(), replyToAll);
        }
    }
}

void ViewerPrivate::slotSpeakText()
{
    const QString text = mViewer->selectedText();
    if (!text.isEmpty()) {
#ifdef HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT
        mTextToSpeechContainerWidget->say(text);
#endif
    }
}

QUrl ViewerPrivate::imageUrl() const
{
    QUrl url;
    if (mImageUrl.scheme() == QLatin1StringView("cid")) {
        url = QUrl(MessageViewer::WebEngineEmbedPart::self()->contentUrl(mImageUrl.path()));
    } else {
        url = mImageUrl;
    }
    return url;
}

void ViewerPrivate::slotCopyImageLocation()
{
#ifndef QT_NO_CLIPBOARD
    QApplication::clipboard()->setText(imageUrl().url());
#endif
}

void ViewerPrivate::slotCopySelectedText()
{
    mViewer->triggerPageAction(QWebEnginePage::Copy);
}

void ViewerPrivate::viewerSelectionChanged()
{
    mActionCollection->action(u"kmail_copy"_s)->setEnabled(!mViewer->selectedText().isEmpty());
}

void ViewerPrivate::selectAll()
{
    mViewer->selectAll();
}

void ViewerPrivate::slotUrlCopy()
{
#ifndef QT_NO_CLIPBOARD
    QClipboard *clip = QApplication::clipboard();
    if (mClickedUrl.scheme() == QLatin1StringView("mailto")) {
        // put the url into the mouse selection and the clipboard
        const QString address = KEmailAddress::decodeMailtoUrl(mClickedUrl);
        clip->setText(address, QClipboard::Clipboard);
        clip->setText(address, QClipboard::Selection);
        PimCommon::BroadcastStatus::instance()->setStatusMsg(i18n("Address copied to clipboard."));
    } else {
        // put the url into the mouse selection and the clipboard
        const QString clickedUrl = mClickedUrl.url();
        clip->setText(clickedUrl, QClipboard::Clipboard);
        clip->setText(clickedUrl, QClipboard::Selection);
        PimCommon::BroadcastStatus::instance()->setStatusMsg(i18n("URL copied to clipboard."));
    }
#endif
}

void ViewerPrivate::slotSaveMessage()
{
    if (!mMessageItem.hasPayload<KMime::Message::Ptr>()) {
        if (mMessageItem.isValid()) {
            qCWarning(MESSAGEVIEWER_LOG) << "Payload is not a MessagePtr!";
        }
        return;
    }

    if (!Util::saveMessageInMbox(Akonadi::Item::List() << mMessageItem, mMainWindow)) {
        qCWarning(MESSAGEVIEWER_LOG) << "Impossible to save as mbox";
    }
}

void ViewerPrivate::saveRelativePosition()
{
    if (mViewer) {
        mViewer->saveRelativePosition();
    }
}

bool ViewerPrivate::htmlMail() const
{
    if (mDisplayFormatMessageOverwrite == Viewer::UseGlobalSetting) {
        return mHtmlMailGlobalSetting;
    } else {
        return mDisplayFormatMessageOverwrite == Viewer::Html;
    }
}

bool ViewerPrivate::htmlLoadExternal() const
{
    if (!mNodeHelper || !mMessage) {
        return mHtmlLoadExtOverride;
    }

    // when displaying an encrypted message, only load external resources on explicit request
    if (mNodeHelper->overallEncryptionState(mMessage.data()) != MimeTreeParser::KMMsgNotEncrypted) {
        return mHtmlLoadExtOverride;
    }

    const bool loadExternal = (mHtmlLoadExternalDefaultSetting && !mHtmlLoadExtOverride) || (!mHtmlLoadExternalDefaultSetting && mHtmlLoadExtOverride);

    return loadExternal;
}

void ViewerPrivate::setDisplayFormatMessageOverwrite(Viewer::DisplayFormatMessage format)
{
    if (mDisplayFormatMessageOverwrite != format) {
        mDisplayFormatMessageOverwrite = format;
        // keep toggle display mode action state in sync.
        if (mToggleDisplayModeAction) {
            mToggleDisplayModeAction->setChecked(htmlMail());
        }
    }
}

bool ViewerPrivate::htmlMailGlobalSetting() const
{
    return mHtmlMailGlobalSetting;
}

Viewer::DisplayFormatMessage ViewerPrivate::displayFormatMessageOverwrite() const
{
    return mDisplayFormatMessageOverwrite;
}

void ViewerPrivate::setHtmlLoadExtDefault(bool loadExtDefault)
{
    mHtmlLoadExternalDefaultSetting = loadExtDefault;
}

void ViewerPrivate::setHtmlLoadExtOverride(bool loadExtOverride)
{
    mHtmlLoadExtOverride = loadExtOverride;
}

bool ViewerPrivate::htmlLoadExtOverride() const
{
    return mHtmlLoadExtOverride;
}

void ViewerPrivate::setDecryptMessageOverwrite(bool overwrite)
{
    mDecrytMessageOverwrite = overwrite;
}

bool ViewerPrivate::showSignatureDetails() const
{
    return mShowSignatureDetails;
}

void ViewerPrivate::setShowSignatureDetails(bool showDetails)
{
    mShowSignatureDetails = showDetails;
}

void ViewerPrivate::setShowEncryptionDetails(bool showEncDetails)
{
    mShowEncryptionDetails = showEncDetails;
}

bool ViewerPrivate::showEncryptionDetails() const
{
    return mShowEncryptionDetails;
}

[[nodiscard]] static int countNodes(const KMime::Content *node)
{
    int count = 1;

    const auto children = node->contents();
    for (const auto child : children) {
        count += countNodes(child);
    }
    return count;
}

void ViewerPrivate::scrollToAttachment(KMime::Content *node)
{
    const QString indexStr = node->index().toString();
    // The anchors for this are created in ObjectTreeParser::parseObjectTree()
    mViewer->scrollToAnchor(QLatin1StringView("attachmentDiv") + indexStr);

    // Remove any old color markings which might be there
    const int totalChildCount = countNodes(node->topLevel());
    for (int i = 0; i < totalChildCount + 1; ++i) {
        // Not optimal I need to optimize it. But for the moment it removes yellow mark
        mViewer->removeAttachmentMarking(u"attachmentDiv%1"_s.arg(i + 1));
        mViewer->removeAttachmentMarking(u"attachmentDiv1.%1"_s.arg(i + 1));
        mViewer->removeAttachmentMarking(u"attachmentDiv2.%1"_s.arg(i + 1));
    }

    // Don't mark hidden nodes, that would just produce a strange yellow line
    if (mNodeHelper->isNodeDisplayedHidden(node)) {
        return;
    }

    // Now, color the div of the attachment in yellow, so that the user sees what happened.
    // We created a special marked div for this in writeAttachmentMarkHeader() in ObjectTreeParser,
    // find and modify that now.
    mViewer->markAttachment(QLatin1StringView("attachmentDiv") + indexStr, u"border:2px solid %1"_s.arg(cssHelper()->pgpWarnColor().name()));
}

void ViewerPrivate::setUseFixedFont(bool useFixedFont)
{
    if (mHtmlHeadSettings.fixedFont != useFixedFont) {
        mHtmlHeadSettings.fixedFont = useFixedFont;
        if (mToggleFixFontAction) {
            mToggleFixFontAction->setChecked(mHtmlHeadSettings.fixedFont);
        }
    }
}

void ViewerPrivate::itemFetchResult(KJob *job)
{
    if (job->error()) {
        displaySplashPage(i18n("Message loading failed: %1.", job->errorText()));
    } else {
        auto fetch = qobject_cast<Akonadi::ItemFetchJob *>(job);
        Q_ASSERT(fetch);
        if (fetch->items().isEmpty()) {
            displaySplashPage(i18n("Message not found."));
        } else {
            setMessageItem(fetch->items().constFirst());
        }
    }
}

void ViewerPrivate::slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    if (item.id() != messageItem().id()) {
        qCDebug(MESSAGEVIEWER_LOG) << "Update for an already forgotten item. Weird.";
        return;
    }
    if (parts.contains("PLD:RFC822")) {
        setMessageItem(item, MimeTreeParser::Force);
    }
}

void ViewerPrivate::slotItemMoved(const Akonadi::Item &item, const Akonadi::Collection &, const Akonadi::Collection &)
{
    // clear the view after the current item has been moved somewhere else (e.g. to trash)
    if (item.id() == messageItem().id()) {
        slotClear();
    }
}

void ViewerPrivate::slotClear()
{
    q->clear(MimeTreeParser::Force);
    Q_EMIT itemRemoved();
}

void ViewerPrivate::slotMessageRendered()
{
    if (!mMessageItem.isValid()) {
        return;
    }

    /**
     * This slot might be called multiple times for the same message if
     * some asynchronous mementos are involved in rendering. Therefore we
     * have to make sure we execute the MessageLoadedHandlers only once.
     */
    if (mMessageItem.id() == mPreviouslyViewedItemId) {
        return;
    }

    mPreviouslyViewedItemId = mMessageItem.id();

    for (AbstractMessageLoadedHandler *handler : std::as_const(mMessageLoadedHandlers)) {
        handler->setItem(mMessageItem);
    }
}

void ViewerPrivate::setZoomFactor(qreal zoomFactor)
{
    mZoomActionMenu->setWebViewerZoomFactor(zoomFactor);
}

void ViewerPrivate::goOnline()
{
    Q_EMIT makeResourceOnline(Viewer::AllResources);
}

void ViewerPrivate::goResourceOnline()
{
    Q_EMIT makeResourceOnline(Viewer::SelectedResource);
}

void ViewerPrivate::slotSaveMessageDisplayFormat()
{
    if (mMessageItem.isValid()) {
        auto job = new MessageViewer::ModifyMessageDisplayFormatJob(mSession, this);
        job->setMessageFormat(displayFormatMessageOverwrite());
        job->setMessageItem(mMessageItem);
        job->setRemoteContent(htmlLoadExtOverride());
        job->start();
    }
}

void ViewerPrivate::slotResetMessageDisplayFormat()
{
    if (mMessageItem.isValid()) {
        if (mMessageItem.hasAttribute<MessageViewer::MessageDisplayFormatAttribute>()) {
            auto job = new MessageViewer::ModifyMessageDisplayFormatJob(mSession, this);
            job->setMessageItem(mMessageItem);
            job->setResetFormat(true);
            job->start();
        }
    }
}

void ViewerPrivate::slotMessageMayBeAScam()
{
    if (mMessageItem.isValid()) {
        if (mMessageItem.hasAttribute<MessageViewer::ScamAttribute>()) {
            const MessageViewer::ScamAttribute *const attr = mMessageItem.attribute<MessageViewer::ScamAttribute>();
            if (attr && !attr->isAScam()) {
                return;
            }
        }
        if (mMessageItem.hasPayload<KMime::Message::Ptr>()) {
            auto message = mMessageItem.payload<KMime::Message::Ptr>();
            const QString email = QLatin1StringView(KEmailAddress::firstEmailAddress(message->from()->as7BitString(false)));
            const QStringList lst = MessageViewer::MessageViewerSettings::self()->scamDetectionWhiteList();
            if (lst.contains(email)) {
                return;
            }
        }
    }
    if (!mScamDetectionWarning) {
        createScamDetectionWarningWidget();
    }
    mScamDetectionWarning->slotShowWarning();
}

void ViewerPrivate::slotMessageIsNotAScam()
{
    if (mMessageItem.isValid()) {
        auto attr = mMessageItem.attribute<MessageViewer::ScamAttribute>(Akonadi::Item::AddIfMissing);
        attr->setIsAScam(false);
        auto modify = new Akonadi::ItemModifyJob(mMessageItem, mSession);
        modify->setIgnorePayload(true);
        modify->disableRevisionCheck();
        connect(modify, &KJob::result, this, &ViewerPrivate::slotModifyItemDone);
    }
}

void ViewerPrivate::slotModifyItemDone(KJob *job)
{
    if (job && job->error()) {
        qCWarning(MESSAGEVIEWER_LOG) << " Error trying to change attribute:" << job->errorText();
    }
}

void ViewerPrivate::saveMainFrameScreenshotInFile(const QString &filename)
{
    mViewer->saveMainFrameScreenshotInFile(filename);
}

void ViewerPrivate::slotAddToWhiteList()
{
    if (mMessageItem.isValid()) {
        if (mMessageItem.hasPayload<KMime::Message::Ptr>()) {
            auto message = mMessageItem.payload<KMime::Message::Ptr>();
            const QString email = QLatin1StringView(KEmailAddress::firstEmailAddress(message->from()->as7BitString(false)));
            QStringList lst = MessageViewer::MessageViewerSettings::self()->scamDetectionWhiteList();
            if (lst.contains(email)) {
                return;
            }
            lst << email;
            MessageViewer::MessageViewerSettings::self()->setScamDetectionWhiteList(lst);
            MessageViewer::MessageViewerSettings::self()->save();
        }
    }
}

void ViewerPrivate::slotRefreshMessage(const Akonadi::Item &item)
{
    if (item.id() == mMessageItem.id()) {
        setMessageItem(item, MimeTreeParser::Force);
    }
}

void ViewerPrivate::slotServiceUrlSelected(PimCommon::ShareServiceUrlManager::ServiceType serviceType)
{
    const QUrl url = mShareServiceManager->generateServiceUrl(mClickedUrl.toString(), QString(), serviceType);
    mShareServiceManager->openUrl(url);
}

QList<QAction *> ViewerPrivate::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    return mViewer->interceptorUrlActions(result);
}

void ViewerPrivate::setPrintElementBackground(bool printElementBackground)
{
    mViewer->setPrintElementBackground(printElementBackground);
}

void ViewerPrivate::slotToggleEmoticons()
{
    mForceEmoticons = !mForceEmoticons;
    // Save value
    MessageViewer::MessageViewerSettings::self()->setShowEmoticons(mForceEmoticons);
    headerStylePlugin()->headerStyle()->setShowEmoticons(mForceEmoticons);
    update(MimeTreeParser::Force);
}

void ViewerPrivate::slotZoomChanged(qreal zoom)
{
    mViewer->slotZoomChanged(zoom);
    const qreal zoomFactor = zoom * 100;
    MessageViewer::MessageViewerSettings::self()->setZoomFactor(zoomFactor);
    Q_EMIT zoomChanged(zoomFactor);
}

void ViewerPrivate::updateShowMultiMessagesButton(bool enablePreviousButton, bool enableNextButton)
{
    mShowNextMessageWidget->updateButton(enablePreviousButton, enableNextButton);
}

DKIMViewerMenu *ViewerPrivate::dkimViewerMenu()
{
    if (MessageViewer::MessageViewerSettings::self()->enabledDkim()) {
        if (!messageIsInSpecialFolder()) {
            if (!mDkimViewerMenu) {
                mDkimViewerMenu = new MessageViewer::DKIMViewerMenu(this);
                connect(mDkimViewerMenu, &DKIMViewerMenu::recheckSignature, this, [this]() {
                    MessageViewer::DKIMManager::self()->checkDKim(mMessageItem);
                });
                connect(mDkimViewerMenu, &DKIMViewerMenu::updateDkimKey, this, []() {
                    qWarning() << " Unimplemented yet updateDkimKey";
                });
                connect(mDkimViewerMenu, &DKIMViewerMenu::showDkimRules, this, [this]() {
                    DKIMManageRulesDialog dlg(viewer());
                    dlg.exec();
                });
            }
            mDkimViewerMenu->setEnableUpdateDkimKeyMenu(MessageViewer::MessageViewerSettings::saveKey()
                                                        == MessageViewer::MessageViewerSettings::EnumSaveKey::Save);
            return mDkimViewerMenu;
        }
    }
    return nullptr;
}

bool ViewerPrivate::isAutocryptEnabled(KMime::Message *message)
{
    if (!mIdentityManager) {
        return false;
    }

    const auto id = MessageCore::Util::identityForMessage(message, mIdentityManager, mFolderIdentity);
    return id.autocryptEnabled();
}

void ViewerPrivate::setIdentityManager(KIdentityManagementCore::IdentityManager *ident)
{
    mIdentityManager = ident;
}

void MessageViewer::ViewerPrivate::setFolderIdentity(uint folderIdentity)
{
    mFolderIdentity = folderIdentity;
}

#include "moc_viewer_p.cpp"
