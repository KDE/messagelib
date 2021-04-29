/*
  SPDX-FileCopyrightText: 1997 Markus Wuebben <markus.wuebben@kde.org>
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>
  SPDX-FileCopyrightText: 2010 Torgny Nyblom <nyblom@kde.org>
  SPDX-FileCopyrightText: 2011-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "viewer_p.h"
#include "viewerpurposemenuwidget.h"

#include "messagedisplayformatattribute.h"
#include "messageviewer_debug.h"
#include "scamdetection/scamattribute.h"
#include "scamdetection/scamdetectionwarningwidget.h"
#include "utils/iconnamecache.h"
#include "utils/mimetype.h"
#include "viewer/mimeparttree/mimeparttreeview.h"
#include "viewer/objecttreeemptysource.h"
#include "viewer/objecttreeviewersource.h"
#include "viewer/renderer/messageviewerrenderer.h"

#include "messageviewer/headerstrategy.h"
#include "messageviewer/headerstyle.h"
#include "widgets/developertooldialog.h"
#include <KPIMTextEdit/SlideContainer>

#include "job/modifymessagedisplayformatjob.h"

#include "htmlwriter/webengineembedpart.h"
#include "viewerplugins/viewerplugintoolmanager.h"
#include <KContacts/VCardConverter>
// KDE includes
#include <KActionCollection>
#include <KActionMenu>
#include <KCharsets>
#include <QAction>
#include <QHBoxLayout>
#include <QPrintPreviewDialog>
#include <QVBoxLayout>

#include <Akonadi/KMime/SpecialMailCollections>
#include <AkonadiCore/ItemCreateJob>
#include <AkonadiCore/ItemModifyJob>
#include <KApplicationTrader>
#include <KEmailAddress>
#include <KFileItemActions>
#include <KIO/ApplicationLauncherJob>
#include <KIO/JobUiDelegate>
#include <KIO/OpenUrlJob>
#include <KLocalizedString>
#include <KMessageBox>
#include <KMimeTypeChooser>
#include <KSelectAction>
#include <KSharedConfig>
#include <KStandardGuiItem>
#include <KToggleAction>
#include <MailTransportAkonadi/ErrorAttribute>
#include <MessageCore/Util>
#include <QIcon>
#include <QMenu>
#include <QMimeData>
#include <QTemporaryDir>
#include <messageflags.h>

// Qt includes
#include <QClipboard>
#include <QItemSelectionModel>
#include <QMimeDatabase>
#include <QPrintDialog>
#include <QPrinter>
#include <QSplitter>
#include <QTreeView>
#include <QWheelEvent>
#include <WebEngineViewer/WebEngineExportHtmlPageJob>
// libkdepim
#include <MessageCore/AttachmentPropertiesDialog>
#include <PimCommon/BroadcastStatus>

#include <Akonadi/KMime/MessageParts>
#include <Akonadi/KMime/MessageStatus>
#include <AkonadiCore/attributefactory.h>
#include <AkonadiCore/collection.h>
#include <AkonadiCore/itemfetchjob.h>
#include <AkonadiCore/itemfetchscope.h>

#include <MessageCore/AutocryptUtils>
#include <KIdentityManagement/Identity>
#include <KIdentityManagement/IdentityManager>

// own includes
#include "csshelper.h"
#include "messageviewer/messageviewerutil.h"
#include "settings/messageviewersettings.h"
#include "utils/messageviewerutil_p.h"
#include "viewer/attachmentstrategy.h"
#include "viewer/mimeparttree/mimetreemodel.h"
#include "viewer/urlhandlermanager.h"
#include "widgets/attachmentdialog.h"
#include "widgets/htmlstatusbar.h"
#include "widgets/shownextmessagewidget.h"
#include "widgets/vcardviewer.h"

#include "header/headerstylemenumanager.h"
#include "htmlwriter/webengineparthtmlwriter.h"
#include "viewer/webengine/mailwebengineview.h"
#include "widgets/submittedformwarningwidget.h"
#include <WebEngineViewer/FindBarWebEngineView>
#include <WebEngineViewer/LocalDataBaseManager>
#include <WebEngineViewer/WebEngineExportPdfPageJob>
#include <WebEngineViewer/WebHitTestResult>
#include <widgets/mailsourcewebengineviewer.h>

#include "interfaces/htmlwriter.h"
#include <MimeTreeParser/BodyPart>
#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/ObjectTreeParser>

#include <MessageCore/StringUtil>

#include <MessageCore/MessageCoreSettings>
#include <MessageCore/NodeHelper>

#include <AkonadiCore/CollectionFetchJob>
#include <AkonadiCore/agentinstance.h>
#include <AkonadiCore/agentmanager.h>
#include <AkonadiCore/collectionfetchscope.h>

#include "widgets/mailtrackingwarningwidget.h"
#include <KJobWidgets/KJobWidgets>
#include <KPIMTextEdit/TextToSpeechWidget>
#include <QApplication>
#include <QStandardPaths>
#include <QWebEngineSettings>
#include <WebEngineViewer/ZoomActionMenu>
#include <boost/bind.hpp>
#include <header/headerstyleplugin.h>
#include <viewerplugins/viewerplugininterface.h>

#include <GrantleeTheme/GrantleeTheme>
#include <GrantleeTheme/GrantleeThemeManager>

#include "dkim-verify/dkimmanager.h"
#include "dkim-verify/dkimmanagerulesdialog.h"
#include "dkim-verify/dkimresultattribute.h"
#include "dkim-verify/dkimviewermenu.h"
#include "dkim-verify/dkimwidgetinfo.h"

#include "remote-content/remotecontentmenu.h"

using namespace boost;
using namespace MailTransport;
using namespace MessageViewer;
using namespace MessageCore;

static QAtomicInt _k_attributeInitialized;

template<typename Arg, typename R, typename C> struct InvokeWrapper {
    R *receiver;
    void (C::*memberFun)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFun)(result);
    }
};

template<typename Arg, typename R, typename C> InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
    return wrapper;
}

ViewerPrivate::ViewerPrivate(Viewer *aParent, QWidget *mainWindow, KActionCollection *actionCollection)
    : QObject(aParent)
    , mNodeHelper(new MimeTreeParser::NodeHelper)
    , mOldGlobalOverrideEncoding(QStringLiteral("---"))
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

    mDkimWidgetInfo = new MessageViewer::DKIMWidgetInfo(mMainWindow);
    if (_k_attributeInitialized.testAndSetAcquire(0, 1)) {
        Akonadi::AttributeFactory::registerAttribute<MessageViewer::MessageDisplayFormatAttribute>();
        Akonadi::AttributeFactory::registerAttribute<MessageViewer::ScamAttribute>();
    }
    mPhishingDatabase = new WebEngineViewer::LocalDataBaseManager(this);
    mPhishingDatabase->initialize();
    connect(mPhishingDatabase, &WebEngineViewer::LocalDataBaseManager::checkUrlFinished, this, &ViewerPrivate::slotCheckedUrlFinished);

    mShareServiceManager = new PimCommon::ShareServiceUrlManager(this);

    mDisplayFormatMessageOverwrite = MessageViewer::Viewer::UseGlobalSetting;

    mUpdateReaderWinTimer.setObjectName(QStringLiteral("mUpdateReaderWinTimer"));
    mResizeTimer.setObjectName(QStringLiteral("mResizeTimer"));

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
    mMonitor.setObjectName(QStringLiteral("MessageViewerMonitor"));
    mMonitor.setSession(mSession);
    Akonadi::ItemFetchScope fs;
    fs.fetchFullPayload();
    fs.fetchAttribute<MailTransport::ErrorAttribute>();
    fs.fetchAttribute<MessageViewer::MessageDisplayFormatAttribute>();
    fs.fetchAttribute<MessageViewer::ScamAttribute>();
    fs.fetchAttribute<MessageViewer::DKIMResultAttribute>();
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
            if (ct->hasParameter(QStringLiteral("url"))) {
                auto job = new KIO::OpenUrlJob(url, QStringLiteral("text/html"));
                job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, q));
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
    if (mimetype.isValid() && mimetype.inherits(KContacts::Addressee::mimeType())) {
        showVCard(node);
        return;
    }

    // special case treatment on mac and windows
    QUrl atmUrl = url;
    if (url.isEmpty()) {
        atmUrl = mNodeHelper->tempFileUrlFromNode(node);
    }
    if (Util::handleUrlWithQDesktopServices(atmUrl)) {
        return;
    }

    if (!mimetype.isValid() || mimetype.name() == QLatin1String("application/octet-stream")) {
        mimetype = MimeTreeParser::Util::mimetype(url.isLocalFile() ? url.toLocalFile() : url.fileName());
    }
    KService::Ptr offer = KApplicationTrader::preferredService(mimetype.name());

    const QString filenameText = MimeTreeParser::NodeHelper::fileName(node);

    QPointer<AttachmentDialog> dialog = new AttachmentDialog(mMainWindow, filenameText, offer, QLatin1String("askSave_") + mimetype.name());
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

bool ViewerPrivate::deleteAttachment(KMime::Content *node, bool showWarning)
{
    if (!node) {
        return true;
    }
    KMime::Content *parent = node->parent();
    if (!parent) {
        return true;
    }

    const QVector<KMime::Content *> extraNodes = mNodeHelper->extraContents(mMessage.data());
    if (extraNodes.contains(node->topLevel())) {
        KMessageBox::error(mMainWindow,
                           i18n("Deleting an attachment from an encrypted or old-style mailman message is not supported."),
                           i18n("Delete Attachment"));
        return true; // cancelled
    }

    if (showWarning
        && KMessageBox::warningContinueCancel(mMainWindow,
                                              i18n("Deleting an attachment might invalidate any digital signature on this message."),
                                              i18n("Delete Attachment"),
                                              KStandardGuiItem::del(),
                                              KStandardGuiItem::cancel(),
                                              QStringLiteral("DeleteAttachmentSignatureWarning"))
            != KMessageBox::Continue) {
        return false; // cancelled
    }
    // don't confuse the model
    mMimePartTree->clearModel();
    QString filename;
    QString name;
    QByteArray mimetype;
    if (auto cd = node->contentDisposition(false)) {
        filename = cd->filename();
    }

    if (auto ct = node->contentType(false)) {
        name = ct->name();
        mimetype = ct->mimeType();
    }

    // text/plain part:
    auto deletePart = new KMime::Content(parent);
    auto deleteCt = deletePart->contentType(true);
    deleteCt->setMimeType("text/x-moz-deleted");
    deleteCt->setName(QStringLiteral("Deleted: %1").arg(name), "utf8");
    deletePart->contentDisposition(true)->setDisposition(KMime::Headers::CDattachment);
    deletePart->contentDisposition(false)->setFilename(QStringLiteral("Deleted: %1").arg(name));

    deleteCt->setCharset("utf-8");
    deletePart->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
    QByteArray bodyMessage = QByteArrayLiteral("\nYou deleted an attachment from this message. The original MIME headers for the attachment were:");
    bodyMessage += ("\nContent-Type: ") + mimetype;
    bodyMessage += ("\nname=\"") + name.toUtf8() + "\"";
    bodyMessage += ("\nfilename=\"") + filename.toUtf8() + "\"";
    deletePart->setBody(bodyMessage);
    parent->replaceContent(node, deletePart);

    parent->assemble();

    KMime::Message *modifiedMessage = mNodeHelper->messageWithExtraContent(mMessage.data());
    mMimePartTree->mimePartModel()->setRoot(modifiedMessage);
    mMessageItem.setPayloadFromData(mMessage->encodedContent());
    auto job = new Akonadi::ItemModifyJob(mMessageItem, mSession);
    job->disableRevisionCheck();
    connect(job, &KJob::result, this, &ViewerPrivate::itemModifiedResult);
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
    const KService::List offers = KFileItemActions::associatedApplications(QStringList() << contentTypeStr, QString());
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
            menu->menuAction()->setObjectName(QStringLiteral("openWith_submenu")); // for the unittest
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
    const KService::Ptr app = act->data().value<KService::Ptr>();
    attachmentOpenWith(mCurrentContent, app);
}

void ViewerPrivate::slotOpenWithAction(QAction *act)
{
    const KService::Ptr app = act->data().value<KService::Ptr>();
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
        contentTypeStr = QLatin1String(contentType->mimeType());
    }
    if (contentTypeStr == QStringLiteral("message/global")) { // Not registred in mimetype => it's a message/rfc822
        contentTypeStr = QStringLiteral("message/rfc822");
    }
    deletedAttachment = (contentTypeStr == QStringLiteral("text/x-moz-deleted"));
    // Not necessary to show popup menu when attachment was removed
    if (deletedAttachment) {
        return;
    }

    QMenu menu;

    QAction *action = menu.addAction(QIcon::fromTheme(QStringLiteral("document-open")), i18nc("to open", "Open"));
    action->setEnabled(!deletedAttachment);
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::Open);
    });
    if (!deletedAttachment) {
        createOpenWithMenu(&menu, contentTypeStr, true);
    }

    QMimeDatabase mimeDb;
    auto mimetype = mimeDb.mimeTypeForName(contentTypeStr);
    if (mimetype.isValid()) {
        const QStringList parentMimeType = mimetype.parentMimeTypes();
        if ((contentTypeStr == QLatin1String("text/plain")) || (contentTypeStr == QLatin1String("image/png")) || (contentTypeStr == QLatin1String("image/jpeg"))
            || parentMimeType.contains(QLatin1String("text/plain")) || parentMimeType.contains(QLatin1String("image/png"))
            || parentMimeType.contains(QLatin1String("image/jpeg"))) {
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

    action = menu.addAction(QIcon::fromTheme(QStringLiteral("document-save-as")), i18n("Save As..."));
    action->setEnabled(!deletedAttachment);
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::Save);
    });

    action = menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("Copy"));
    action->setEnabled(!deletedAttachment);
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::Copy);
    });

    const bool isEncapsulatedMessage = node->parent() && node->parent()->bodyIsMessage();
    const bool canChange = mMessageItem.isValid() && mMessageItem.parentCollection().isValid()
        && (mMessageItem.parentCollection().rights() != Akonadi::Collection::ReadOnly) && !isEncapsulatedMessage;

    action = menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Delete Attachment"));
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::Delete);
    });

    action->setEnabled(canChange && !deletedAttachment);
#if 0
    menu->addSeparator();

    action
        = menu->addAction(QIcon::fromTheme(QStringLiteral("mail-reply-sender")),
                          i18n("Reply To Author"));
    connect(action, &QAction::triggered, this, [this]() {
        slotHandleAttachment(Viewer::ReplyMessageToAuthor);
    });

    menu->addSeparator();

    action = menu->addAction(QIcon::fromTheme(QStringLiteral("mail-reply-all")), i18n(
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

    const QString contentTypeStr = QLatin1String(content->contentType()->mimeType());

    // determine the MIME type of the attachment
    // prefer the value of the Content-Type header
    QMimeDatabase mimeDb;
    auto mimetype = mimeDb.mimeTypeForName(contentTypeStr);

    if (mimetype.isValid() && mimetype.inherits(KContacts::Addressee::mimeType())) {
        attachmentView(content);
        return KService::Ptr(nullptr);
    }

    if (!mimetype.isValid() || mimetype.name() == QLatin1String("application/octet-stream")) {
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
    auto tmpDir = new QTemporaryDir(QDir::tempPath() + QLatin1String("/messageviewer_attachment_XXXXXX"));
    if (tmpDir->isValid()) {
        tmpDir->setAutoRemove(false);
        const QString path = tmpDir->path();
        delete tmpDir;
        QFile f(name);
        const QUrl tmpFileName = QUrl::fromLocalFile(name);
        const QString newPath = path + QLatin1Char('/') + tmpFileName.fileName();

        if (!f.copy(newPath)) {
            qCDebug(MESSAGEVIEWER_LOG) << " File was not able to copy: filename: " << name << " to " << path;
        } else {
            name = newPath;
        }
        f.close();
    } else {
        delete tmpDir;
    }

    QList<QUrl> lst;
    const QFileDevice::Permissions perms = QFile::permissions(name);
    QFile::setPermissions(name, perms | QFileDevice::ReadUser | QFileDevice::WriteUser);
    const QUrl url = QUrl::fromLocalFile(name);
    lst.append(url);

    auto job = new KIO::ApplicationLauncherJob(offer);
    job->setUrls({url});
    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, mMainWindow));
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
    displaySplashPage(QStringLiteral("status.html"),
                      {{QStringLiteral("icon"), QStringLiteral("kmail")},
                       {QStringLiteral("name"), i18n("KMail")},
                       {QStringLiteral("subtitle"), i18n("The KDE Mail Client")},
                       {QStringLiteral("message"), message}});
}

void ViewerPrivate::displaySplashPage(const QString &templateName, const QVariantHash &data, const QByteArray &domain)
{
    if (mViewer) {
        mMsgDisplay = false;
        adjustLayout();

        GrantleeTheme::ThemeManager manager(QStringLiteral("splashPage"), QStringLiteral("splash.theme"), nullptr, QStringLiteral("messageviewer/about/"));
        GrantleeTheme::Theme theme = manager.theme(QStringLiteral("default"));
        if (theme.isValid()) {
            mViewer->setHtml(theme.render(templateName, data, domain), QUrl::fromLocalFile(theme.absolutePath() + QLatin1Char('/')));
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

    mNodeHelper->setOverrideCodec(mMessage.data(), overrideCodec());

    if (mMessageItem.hasAttribute<MessageViewer::MessageDisplayFormatAttribute>()) {
        const MessageViewer::MessageDisplayFormatAttribute *const attr = mMessageItem.attribute<MessageViewer::MessageDisplayFormatAttribute>();
        setHtmlLoadExtOverride(attr->remoteContent());
        setDisplayFormatMessageOverwrite(attr->messageFormat());
    }

    htmlWriter()->begin();
    htmlWriter()->write(cssHelper()->htmlHead(mUseFixedFont));

    if (!mMainWindow) {
        q->setWindowTitle(mMessage->subject()->asUnicodeString());
    }

    // Don't update here, parseMsg() can overwrite the HTML mode, which would lead to flicker.
    // It is updated right after parseMsg() instead.
    mColorBar->setMode(MimeTreeParser::Util::Normal, HtmlStatusBar::NoUpdate);

    if (mMessageItem.hasAttribute<ErrorAttribute>()) {
        // TODO: Insert link to clear error so that message might be resent
        const ErrorAttribute *const attr = mMessageItem.attribute<ErrorAttribute>();
        Q_ASSERT(attr);
        initializeColorFromScheme();

        htmlWriter()->write(QStringLiteral("<div style=\"background:%1;color:%2;border:1px solid %2\">%3</div>")
                                .arg(mBackgroundError.name(), mForegroundError.name(), attr->message().toHtmlEscaped()));
        htmlWriter()->write(QStringLiteral("<p></p>"));
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
    assert(content != nullptr);
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

    auto *message = dynamic_cast<KMime::Message *>(content);
    bool onlySingleNode = mMessage.data() != content;

    // Pass control to the OTP now, which does the real work
    mNodeHelper->setNodeUnprocessed(mMessage.data(), true);
    MailViewerSource otpSource(this);
    MimeTreeParser::ObjectTreeParser otp(&otpSource, mNodeHelper);

    otp.setAllowAsync(!mPrinting);
    otp.parseObjectTree(content, onlySingleNode);
    htmlWriter()->setCodec(otp.plainTextContentCharset());
    if (message) {
        htmlWriter()->write(writeMessageHeader(message, hasVCard ? vCardContent : nullptr, true));
    }

    otpSource.render(otp.parsedPart(), onlySingleNode);

    // TODO: Setting the signature state to nodehelper is not enough, it should actually
    // be added to the store, so that the message list correctly displays the signature state
    // of messages that were parsed at least once
    // store encrypted/signed status information in the KMMessage
    //  - this can only be done *after* calling parseObjectTree()
    MimeTreeParser::KMMsgEncryptionState encryptionState = mNodeHelper->overallEncryptionState(content);
    MimeTreeParser::KMMsgSignatureState signatureState = mNodeHelper->overallSignatureState(content);
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
        style->setVCardName(mNodeHelper->asHREF(vCardNode, QStringLiteral("body")));
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

void ViewerPrivate::showVCard(KMime::Content *msgPart)
{
    const QByteArray vCard = msgPart->decodedContent();

    auto vcv = new VCardViewer(mMainWindow, vCard);
    vcv->setAttribute(Qt::WA_DeleteOnClose);
    vcv->show();
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
    connect(mViewer, &MailWebEngineView::formSubmittedForbidden, mSubmittedFormWarning, &SubmittedFormWarningWidget::showWarning);
    connect(mViewer, &MailWebEngineView::mailTrackingFound, mMailTrackingWarning, &MailTrackingWarningWidget::addTracker);
    connect(mScamDetectionWarning, &ScamDetectionWarningWidget::showDetails, mViewer, &MailWebEngineView::slotShowDetails);
    connect(mScamDetectionWarning, &ScamDetectionWarningWidget::moveMessageToTrash, this, &ViewerPrivate::moveMessageToTrash);
    connect(mScamDetectionWarning, &ScamDetectionWarningWidget::messageIsNotAScam, this, &ViewerPrivate::slotMessageIsNotAScam);
    connect(mScamDetectionWarning, &ScamDetectionWarningWidget::addToWhiteList, this, &ViewerPrivate::slotAddToWhiteList);
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

    mUseFixedFont = MessageViewer::MessageViewerSettings::self()->useFixedFont();
    if (mToggleFixFontAction) {
        mToggleFixFontAction->setChecked(mUseFixedFont);
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
    mShowNextMessageWidget->setVisible(messages);
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
    MessageViewer::MessageViewerSettings::self()->setUseFixedFont(mUseFixedFont);
    if (attachmentStrategy()) {
        MessageViewer::MessageViewerSettings::self()->setAttachmentStrategy(QLatin1String(attachmentStrategy()->name()));
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
    // need to set htmlLoadExtOverride() when we set Item otherwise this settings is resetted
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
        mFindBar->closeBar();
    }
    if (!mPrinting) {
        setShowSignatureDetails(false);
    }
    mViewerPluginToolManager->closeAllTools();
    mScamDetectionWarning->setVisible(false);
    mOpenSavedFileFolderWidget->setVisible(false);
    mSubmittedFormWarning->setVisible(false);
    mMailTrackingWarning->hideAndClear();
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
        mNodeHelper->setOverrideCodec(mMessage.data(), overrideCodec());
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
        htmlWriter()->write(cssHelper()->htmlHead(mUseFixedFont));

        parseContent(node);

        htmlWriter()->write(cssHelper()->endBodyHtml());
        htmlWriter()->end();
    }
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
    mSplitter->setObjectName(QStringLiteral("mSplitter"));
    mSplitter->setChildrenCollapsible(false);
    vlay->addWidget(mSplitter);
    mMimePartTree = new MimePartTreeView(mSplitter);
    connect(mMimePartTree, &QAbstractItemView::activated, this, &ViewerPrivate::slotMimePartSelected);
    connect(mMimePartTree, &QWidget::customContextMenuRequested, this, &ViewerPrivate::slotMimeTreeContextMenuRequested);

    mBox = new QWidget(mSplitter);
    auto mBoxHBoxLayout = new QHBoxLayout(mBox);
    mBoxHBoxLayout->setContentsMargins({});

    mColorBar = new HtmlStatusBar(mBox);
    mBoxHBoxLayout->addWidget(mColorBar);
    auto readerBox = new QWidget(mBox);
    auto readerBoxVBoxLayout = new QVBoxLayout(readerBox);
    readerBoxVBoxLayout->setContentsMargins({});
    mBoxHBoxLayout->addWidget(readerBox);

    mColorBar->setObjectName(QStringLiteral("mColorBar"));
    mColorBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    mShowNextMessageWidget = new MessageViewer::ShowNextMessageWidget(readerBox);
    mShowNextMessageWidget->setObjectName(QStringLiteral("shownextmessagewidget"));
    readerBoxVBoxLayout->addWidget(mShowNextMessageWidget);
    mShowNextMessageWidget->hide();
    connect(mShowNextMessageWidget, &ShowNextMessageWidget::showPreviousMessage, this, &ViewerPrivate::showPreviousMessage);
    connect(mShowNextMessageWidget, &ShowNextMessageWidget::showNextMessage, this, &ViewerPrivate::showNextMessage);

    mSubmittedFormWarning = new SubmittedFormWarningWidget(readerBox);
    mSubmittedFormWarning->setObjectName(QStringLiteral("submittedformwarning"));
    readerBoxVBoxLayout->addWidget(mSubmittedFormWarning);

    mMailTrackingWarning = new MailTrackingWarningWidget(readerBox);
    mMailTrackingWarning->setObjectName(QStringLiteral("mailtrackingwarning"));
    readerBoxVBoxLayout->addWidget(mMailTrackingWarning);

    mScamDetectionWarning = new ScamDetectionWarningWidget(readerBox);
    mScamDetectionWarning->setObjectName(QStringLiteral("scandetectionwarning"));
    readerBoxVBoxLayout->addWidget(mScamDetectionWarning);

    mOpenSavedFileFolderWidget = new OpenSavedFileFolderWidget(readerBox);
    mOpenSavedFileFolderWidget->setObjectName(QStringLiteral("opensavefilefolderwidget"));
    readerBoxVBoxLayout->addWidget(mOpenSavedFileFolderWidget);

    mTextToSpeechWidget = new KPIMTextEdit::TextToSpeechWidget(readerBox);
    mTextToSpeechWidget->setObjectName(QStringLiteral("texttospeechwidget"));
    readerBoxVBoxLayout->addWidget(mTextToSpeechWidget);

    mViewer = new MailWebEngineView(mActionCollection, readerBox);
    mViewer->setViewer(this);
    readerBoxVBoxLayout->addWidget(mViewer);
    mViewer->setObjectName(QStringLiteral("mViewer"));

    mViewerPluginToolManager = new MessageViewer::ViewerPluginToolManager(readerBox, this);
    mViewerPluginToolManager->setActionCollection(mActionCollection);
    mViewerPluginToolManager->setPluginName(QStringLiteral("messageviewer"));
    mViewerPluginToolManager->setPluginDirectory(QStringLiteral("messageviewer/viewerplugin"));
    if (!mViewerPluginToolManager->initializePluginList()) {
        qCWarning(MESSAGEVIEWER_LOG) << " Impossible to initialize plugins";
    }
    mViewerPluginToolManager->createView();
    connect(mViewerPluginToolManager, &MessageViewer::ViewerPluginToolManager::activatePlugin, this, &ViewerPrivate::slotActivatePlugin);

    mSliderContainer = new KPIMTextEdit::SlideContainer(readerBox);
    mSliderContainer->setObjectName(QStringLiteral("slidercontainer"));
    readerBoxVBoxLayout->addWidget(mSliderContainer);
    mFindBar = new WebEngineViewer::FindBarWebEngineView(mViewer, q);
    connect(mFindBar, &WebEngineViewer::FindBarWebEngineView::hideFindBar, mSliderContainer, &KPIMTextEdit::SlideContainer::slideOut);
    mSliderContainer->setContent(mFindBar);

    mSplitter->setStretchFactor(mSplitter->indexOf(mMimePartTree), 0);
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
    ac->addAction(QStringLiteral("view_attachments"), attachmentMenu);
    MessageViewer::Util::addHelpTextAction(attachmentMenu, i18n("Choose display style of attachments"));

    auto group = new QActionGroup(this);
    auto raction = new KToggleAction(i18nc("View->attachments->", "&As Icons"), this);
    ac->addAction(QStringLiteral("view_attachments_as_icons"), raction);
    connect(raction, &QAction::triggered, this, &ViewerPrivate::slotIconicAttachments);
    MessageViewer::Util::addHelpTextAction(raction, i18n("Show all attachments as icons. Click to see them."));
    group->addAction(raction);
    attachmentMenu->addAction(raction);

    raction = new KToggleAction(i18nc("View->attachments->", "&Smart"), this);
    ac->addAction(QStringLiteral("view_attachments_smart"), raction);
    connect(raction, &QAction::triggered, this, &ViewerPrivate::slotSmartAttachments);
    MessageViewer::Util::addHelpTextAction(raction, i18n("Show attachments as suggested by sender."));
    group->addAction(raction);
    attachmentMenu->addAction(raction);

    raction = new KToggleAction(i18nc("View->attachments->", "&Inline"), this);
    ac->addAction(QStringLiteral("view_attachments_inline"), raction);
    connect(raction, &QAction::triggered, this, &ViewerPrivate::slotInlineAttachments);
    MessageViewer::Util::addHelpTextAction(raction, i18n("Show all attachments inline (if possible)"));
    group->addAction(raction);
    attachmentMenu->addAction(raction);

    raction = new KToggleAction(i18nc("View->attachments->", "&Hide"), this);
    ac->addAction(QStringLiteral("view_attachments_hide"), raction);
    connect(raction, &QAction::triggered, this, &ViewerPrivate::slotHideAttachments);
    MessageViewer::Util::addHelpTextAction(raction, i18n("Do not show attachments in the message viewer"));
    group->addAction(raction);
    attachmentMenu->addAction(raction);

    mHeaderOnlyAttachmentsAction = new KToggleAction(i18nc("View->attachments->", "In Header Only"), this);
    ac->addAction(QStringLiteral("view_attachments_headeronly"), mHeaderOnlyAttachmentsAction);
    connect(mHeaderOnlyAttachmentsAction, &QAction::triggered, this, &ViewerPrivate::slotHeaderOnlyAttachments);
    MessageViewer::Util::addHelpTextAction(mHeaderOnlyAttachmentsAction, i18n("Show Attachments only in the header of the mail"));
    group->addAction(mHeaderOnlyAttachmentsAction);
    attachmentMenu->addAction(mHeaderOnlyAttachmentsAction);

    // Set Encoding submenu
    mSelectEncodingAction = new KSelectAction(QIcon::fromTheme(QStringLiteral("character-set")), i18n("&Set Encoding"), this);
    mSelectEncodingAction->setToolBarMode(KSelectAction::MenuMode);
    ac->addAction(QStringLiteral("encoding"), mSelectEncodingAction);
    connect(mSelectEncodingAction, &KSelectAction::indexTriggered, this, &ViewerPrivate::slotSetEncoding);
    QStringList encodings = MimeTreeParser::NodeHelper::supportedEncodings(false);
    encodings.prepend(i18n("Auto"));
    mSelectEncodingAction->setItems(encodings);
    mSelectEncodingAction->setCurrentItem(0);

    //
    // Message Menu
    //

    // copy selected text to clipboard
    mCopyAction = ac->addAction(KStandardAction::Copy, QStringLiteral("kmail_copy"));
    mCopyAction->setText(i18n("Copy Text"));
    connect(mCopyAction, &QAction::triggered, this, &ViewerPrivate::slotCopySelectedText);

    connect(mViewer, &MailWebEngineView::selectionChanged, this, &ViewerPrivate::viewerSelectionChanged);
    viewerSelectionChanged();

    // copy all text to clipboard
    mSelectAllAction = new QAction(i18n("Select All Text"), this);
    ac->addAction(QStringLiteral("mark_all_text"), mSelectAllAction);
    connect(mSelectAllAction, &QAction::triggered, this, &ViewerPrivate::selectAll);
    ac->setDefaultShortcut(mSelectAllAction, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A));

    // copy Email address to clipboard
    mCopyURLAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("Copy Link Address"), this);
    ac->addAction(QStringLiteral("copy_url"), mCopyURLAction);
    connect(mCopyURLAction, &QAction::triggered, this, &ViewerPrivate::slotUrlCopy);

    // open URL
    mUrlOpenAction = new QAction(QIcon::fromTheme(QStringLiteral("document-open")), i18n("Open URL"), this);
    ac->addAction(QStringLiteral("open_url"), mUrlOpenAction);
    connect(mUrlOpenAction, &QAction::triggered, this, &ViewerPrivate::slotOpenUrl);

    // use fixed font
    mToggleFixFontAction = new KToggleAction(i18n("Use Fi&xed Font"), this);
    ac->addAction(QStringLiteral("toggle_fixedfont"), mToggleFixFontAction);
    connect(mToggleFixFontAction, &QAction::triggered, this, &ViewerPrivate::slotToggleFixedFont);
    ac->setDefaultShortcut(mToggleFixFontAction, QKeySequence(Qt::Key_X));

    // Show message structure viewer
    mToggleMimePartTreeAction = new KToggleAction(i18n("Show Message Structure"), this);
    ac->addAction(QStringLiteral("toggle_mimeparttree"), mToggleMimePartTreeAction);
    connect(mToggleMimePartTreeAction, &QAction::toggled, this, &ViewerPrivate::slotToggleMimePartTree);
    ac->setDefaultShortcut(mToggleMimePartTreeAction, QKeySequence(Qt::Key_D | Qt::CTRL | Qt::ALT));

    mViewSourceAction = new QAction(i18n("&View Source"), this);
    ac->addAction(QStringLiteral("view_source"), mViewSourceAction);
    connect(mViewSourceAction, &QAction::triggered, this, &ViewerPrivate::slotShowMessageSource);
    ac->setDefaultShortcut(mViewSourceAction, QKeySequence(Qt::Key_V));

    mSaveMessageAction = new QAction(QIcon::fromTheme(QStringLiteral("document-save-as")), i18n("&Save message..."), this);
    ac->addAction(QStringLiteral("save_message"), mSaveMessageAction);
    connect(mSaveMessageAction, &QAction::triggered, this, &ViewerPrivate::slotSaveMessage);
    // Laurent: conflict with kmail shortcut
    // mSaveMessageAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));

    mSaveMessageDisplayFormat = new QAction(i18n("&Save Display Format"), this);
    ac->addAction(QStringLiteral("save_message_display_format"), mSaveMessageDisplayFormat);
    connect(mSaveMessageDisplayFormat, &QAction::triggered, this, &ViewerPrivate::slotSaveMessageDisplayFormat);

    mResetMessageDisplayFormat = new QAction(i18n("&Reset Display Format"), this);
    ac->addAction(QStringLiteral("reset_message_display_format"), mResetMessageDisplayFormat);
    connect(mResetMessageDisplayFormat, &QAction::triggered, this, &ViewerPrivate::slotResetMessageDisplayFormat);

    //
    // Scroll actions
    //
    mScrollUpAction = new QAction(i18n("Scroll Message Up"), this);
    ac->setDefaultShortcut(mScrollUpAction, QKeySequence(Qt::Key_Up));
    ac->addAction(QStringLiteral("scroll_up"), mScrollUpAction);
    connect(mScrollUpAction, &QAction::triggered, q, &Viewer::slotScrollUp);

    mScrollDownAction = new QAction(i18n("Scroll Message Down"), this);
    ac->setDefaultShortcut(mScrollDownAction, QKeySequence(Qt::Key_Down));
    ac->addAction(QStringLiteral("scroll_down"), mScrollDownAction);
    connect(mScrollDownAction, &QAction::triggered, q, &Viewer::slotScrollDown);

    mScrollUpMoreAction = new QAction(i18n("Scroll Message Up (More)"), this);
    ac->setDefaultShortcut(mScrollUpMoreAction, QKeySequence(Qt::Key_PageUp));
    ac->addAction(QStringLiteral("scroll_up_more"), mScrollUpMoreAction);
    connect(mScrollUpMoreAction, &QAction::triggered, q, &Viewer::slotScrollPrior);

    mScrollDownMoreAction = new QAction(i18n("Scroll Message Down (More)"), this);
    ac->setDefaultShortcut(mScrollDownMoreAction, QKeySequence(Qt::Key_PageDown));
    ac->addAction(QStringLiteral("scroll_down_more"), mScrollDownMoreAction);
    connect(mScrollDownMoreAction, &QAction::triggered, q, &Viewer::slotScrollNext);

    //
    // Actions not in menu
    //

    // Toggle HTML display mode.
    mToggleDisplayModeAction = new KToggleAction(i18n("Toggle HTML Display Mode"), this);
    ac->addAction(QStringLiteral("toggle_html_display_mode"), mToggleDisplayModeAction);
    ac->setDefaultShortcut(mToggleDisplayModeAction, QKeySequence(Qt::SHIFT | Qt::Key_H));
    connect(mToggleDisplayModeAction, &QAction::triggered, this, &ViewerPrivate::slotToggleHtmlMode);
    MessageViewer::Util::addHelpTextAction(mToggleDisplayModeAction, i18n("Toggle display mode between HTML and plain text"));

    // Load external reference
    auto loadExternalReferenceAction = new QAction(i18n("Load external references"), this);
    ac->addAction(QStringLiteral("load_external_reference"), loadExternalReferenceAction);
    ac->setDefaultShortcut(loadExternalReferenceAction, QKeySequence(Qt::SHIFT | Qt::CTRL | Qt::Key_R));
    connect(loadExternalReferenceAction, &QAction::triggered, this, &ViewerPrivate::slotLoadExternalReference);
    MessageViewer::Util::addHelpTextAction(loadExternalReferenceAction, i18n("Load external references from the Internet for this message."));

    mSpeakTextAction = new QAction(i18n("Speak Text"), this);
    mSpeakTextAction->setIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop-text-to-speech")));
    ac->addAction(QStringLiteral("speak_text"), mSpeakTextAction);
    connect(mSpeakTextAction, &QAction::triggered, this, &ViewerPrivate::slotSpeakText);

    auto purposeMenuWidget = new MailfilterPurposeMenuWidget(mViewer, this);
    mShareTextAction = new QAction(i18n("Share Text..."), this);
    mShareTextAction->setMenu(purposeMenuWidget->menu());
    mShareTextAction->setIcon(QIcon::fromTheme(QStringLiteral("document-share")));
    ac->addAction(QStringLiteral("purpose_share_text_menu"), mShareTextAction);
    purposeMenuWidget->setViewer(mViewer);

    mCopyImageLocation = new QAction(i18n("Copy Image Location"), this);
    mCopyImageLocation->setIcon(QIcon::fromTheme(QStringLiteral("view-media-visualization")));
    ac->addAction(QStringLiteral("copy_image_location"), mCopyImageLocation);
    ac->setShortcutsConfigurable(mCopyImageLocation, false);
    connect(mCopyImageLocation, &QAction::triggered, this, &ViewerPrivate::slotCopyImageLocation);

    mFindInMessageAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-find")), i18n("&Find in Message..."), this);
    ac->addAction(QStringLiteral("find_in_messages"), mFindInMessageAction);
    connect(mFindInMessageAction, &QAction::triggered, this, &ViewerPrivate::slotFind);
    ac->setDefaultShortcut(mFindInMessageAction, KStandardShortcut::find().first());

    mShareServiceUrlMenu = mShareServiceManager->menu();
    ac->addAction(QStringLiteral("shareservice_menu"), mShareServiceUrlMenu);
    connect(mShareServiceManager, &PimCommon::ShareServiceUrlManager::serviceUrlSelected, this, &ViewerPrivate::slotServiceUrlSelected);

    mDisableEmoticonAction = new KToggleAction(i18n("Disable Emoticon"), this);
    ac->addAction(QStringLiteral("disable_emoticon"), mDisableEmoticonAction);
    connect(mDisableEmoticonAction, &QAction::triggered, this, &ViewerPrivate::slotToggleEmoticons);

    // Don't translate it.
    mDevelopmentToolsAction = new QAction(QStringLiteral("Development Tools"), this);
    ac->addAction(QStringLiteral("development_tools"), mDevelopmentToolsAction);
    ac->setDefaultShortcut(mDevelopmentToolsAction, QKeySequence(Qt::SHIFT | Qt::CTRL | Qt::Key_I));

    connect(mDevelopmentToolsAction, &QAction::triggered, this, &ViewerPrivate::slotShowDevelopmentTools);
}

void ViewerPrivate::slotShowDevelopmentTools()
{
    if (!mDeveloperToolDialog) {
        mDeveloperToolDialog = new DeveloperToolDialog(nullptr);
        mViewer->page()->setDevToolsPage(mDeveloperToolDialog->enginePage());
        mViewer->page()->triggerAction(QWebEnginePage::InspectElement);
        connect(mDeveloperToolDialog, &DeveloperToolDialog::rejected, mDeveloperToolDialog, &DeveloperToolDialog::deleteLater);
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
    const bool isAttachment = !content->contentType()->isMultipart() && !content->isTopLevel();
    const bool isRoot = (content == mMessage.data());
    const auto hasAttachments = KMime::hasAttachment(mMessage.data());

    QMenu popup;

    if (!isRoot) {
        popup.addAction(QIcon::fromTheme(QStringLiteral("document-save-as")), i18n("Save &As..."), this, &ViewerPrivate::slotAttachmentSaveAs);

        if (isAttachment) {
            popup.addAction(QIcon::fromTheme(QStringLiteral("document-open")), i18nc("to open", "Open"), this, &ViewerPrivate::slotAttachmentOpen);

            if (selectedContents().count() == 1) {
                createOpenWithMenu(&popup, QLatin1String(content->contentType()->mimeType()), false);
            } else {
                popup.addAction(i18n("Open With..."), this, &ViewerPrivate::slotAttachmentOpenWith);
            }
            popup.addAction(i18nc("to view something", "View"), this, &ViewerPrivate::slotAttachmentView);
        }
    }

    if (hasAttachments) {
        popup.addAction(i18n("Save All Attachments..."), this, &ViewerPrivate::slotAttachmentSaveAll);
    }

    // edit + delete only for attachments
    if (!isRoot) {
        if (isAttachment) {
            popup.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("Copy"), this, &ViewerPrivate::slotAttachmentCopy);
        }

        if (!content->isTopLevel()) {
            popup.addSeparator();
            popup.addAction(i18n("Properties"), this, &ViewerPrivate::slotAttachmentProperties);
        }
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
        actionName = QStringLiteral("view_attachments_as_icons");
    } else if (as == AttachmentStrategy::smart()) {
        actionName = QStringLiteral("view_attachments_smart");
    } else if (as == AttachmentStrategy::inlined()) {
        actionName = QStringLiteral("view_attachments_inline");
    } else if (as == AttachmentStrategy::hidden()) {
        actionName = QStringLiteral("view_attachments_hide");
    } else if (as == AttachmentStrategy::headerOnly()) {
        actionName = QStringLiteral("view_attachments_headeronly");
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

const QTextCodec *ViewerPrivate::overrideCodec() const
{
    if (mOverrideEncoding.isEmpty() || mOverrideEncoding == QLatin1String("Auto")) { // Auto
        return nullptr;
    } else {
        return MessageViewer::Util::codecForName(mOverrideEncoding.toLatin1());
    }
}

static QColor nextColor(const QColor &c)
{
    int h, s, v;
    c.getHsv(&h, &s, &v);
    return QColor::fromHsv((h + 50) % 360, qMax(s, 64), v);
}

QString ViewerPrivate::renderAttachments(KMime::Content *node, const QColor &bgColor) const
{
    if (!node) {
        return QString();
    }

    QString html;
    KMime::Content *child = MessageCore::NodeHelper::firstChild(node);

    if (child) {
        const QString subHtml = renderAttachments(child, nextColor(bgColor));
        if (!subHtml.isEmpty()) {
            QString margin;
            if (node != mMessage.data() || headerStylePlugin()->hasMargin()) {
                margin = QStringLiteral("padding:2px; margin:2px; ");
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
                html += QLatin1String("</div>");
            }
        }
    } else {
        Util::AttachmentDisplayInfo info = Util::attachmentDisplayInfo(node);
        if (info.displayInHeader) {
            html += QLatin1String("<div style=\"float:left;\">");
            html += QStringLiteral(
                        "<span style=\"white-space:nowrap; border-width: 0px; border-left-width: 5px; border-color: %1; 2px; border-left-style: solid;\">")
                        .arg(bgColor.name());
            mNodeHelper->writeNodeToTempFile(node);
            const QString href = mNodeHelper->asHREF(node, QStringLiteral("header"));
            html += QLatin1String("<a href=\"") + href + QLatin1String("\">");
            const QString imageMaxSize = QStringLiteral("width=\"16\" height=\"16\"");
#if 0
            if (!info.icon.isEmpty()) {
                QImage tmpImg(info.icon);
                if (tmpImg.width() > 48 || tmpImg.height() > 48) {
                    imageMaxSize = QStringLiteral("width=\"48\" height=\"48\"");
                }
            }
#endif
            html += QStringLiteral("<img %1 style=\"vertical-align:middle;\" src=\"").arg(imageMaxSize) + info.icon + QLatin1String("\"/>&nbsp;");
            const int elidedTextSize = headerStylePlugin()->elidedTextSize();
            if (elidedTextSize == -1) {
                html += info.label;
            } else {
                QFont bodyFont = cssHelper()->bodyFont(mUseFixedFont);
                QFontMetrics fm(bodyFont);
                html += fm.elidedText(info.label, Qt::ElideRight, elidedTextSize);
            }
            html += QLatin1String("</a></span></div> ");
        }
    }

    for (KMime::Content *extraNode : mNodeHelper->extraContents(node)) {
        html += renderAttachments(extraNode, bgColor);
    }

    KMime::Content *next = MessageCore::NodeHelper::nextSibling(node);
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
        mUpdateReaderWinTimer.setInterval(150);
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
    if (MessageViewer::MessageViewerSettings::self()->checkPhishingUrl() && (mClickedUrl.scheme() != QLatin1String("mailto"))) {
        mPhishingDatabase->checkUrl(mClickedUrl);
    } else {
        executeRunner(mClickedUrl);
    }
}

void ViewerPrivate::executeRunner(const QUrl &url)
{
    if (!MessageViewer::Util::handleUrlWithQDesktopServices(url)) {
        auto job = new KIO::OpenUrlJob(url);
        job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, viewer()));
        job->setRunExecutables(false);
        job->start();
    }
}

void ViewerPrivate::slotCheckedUrlFinished(const QUrl &url, WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status)
{
    switch (status) {
    case WebEngineViewer::CheckPhishingUrlUtil::BrokenNetwork:
        KMessageBox::error(mMainWindow, i18n("The network is broken."), i18n("Check Phishing URL"));
        break;
    case WebEngineViewer::CheckPhishingUrlUtil::InvalidUrl:
        KMessageBox::error(mMainWindow, i18n("The URL %1 is not valid.", url.toString()), i18n("Check Phishing URL"));
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
    if (KMessageBox::No == KMessageBox::warningYesNo(mMainWindow, i18n("This web site is a malware, do you want to continue to show it?"), i18n("Malware"))) {
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
    if (protocol == QLatin1String("kmail") || protocol == QLatin1String("x-kmail") || protocol == QLatin1String("attachment")
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

    if (mClickedUrl.scheme() == QLatin1String("mailto")) {
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

Viewer::DisplayFormatMessage translateToDisplayFormat(MimeTreeParser::Util::HtmlMode mode)
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
    mScamDetectionWarning->setVisible(false);
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
    mUseFixedFont = !mUseFixedFont;
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
    if (mUseFixedFont) {
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
            htmlWriter()->write(cssHelper()->htmlHead(mUseFixedFont) + cssHelper()->endBodyHtml());
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
    QTimer::singleShot(1000, this, &ViewerPrivate::slotDelayPrintPreview); // 1 second
}

void ViewerPrivate::slotDelayPrintPreview()
{
    auto dialog = new QPrintPreviewDialog(q);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->resize(800, 750);

    connect(dialog, &QPrintPreviewDialog::paintRequested, this, [=](QPrinter *printing) {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        if (!mViewer->execPrintPreviewPage(printing, 10000)) { // 10 seconds
            qCWarning(MESSAGEVIEWER_LOG) << " Impossible to generate preview";
        }
        QApplication::restoreOverrideCursor();
    });

    dialog->open(this, SIGNAL(printingFinished()));
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

void ViewerPrivate::slotOpenInBrowser()
{
    auto job = new WebEngineViewer::WebEngineExportHtmlPageJob(this);
    job->setEngineView(mViewer);
    connect(job, &WebEngineViewer::WebEngineExportHtmlPageJob::failed, this, &ViewerPrivate::slotExportHtmlPageFailed);
    connect(job, &WebEngineViewer::WebEngineExportHtmlPageJob::success, this, &ViewerPrivate::slotExportHtmlPageSuccess);
    job->start();
}

void ViewerPrivate::slotExportHtmlPageSuccess(const QString &filename)
{
    const QUrl url(QUrl::fromLocalFile(filename));
    auto job = new KIO::OpenUrlJob(url, QStringLiteral("text/html"), q);
    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, q));
    job->setDeleteTemporaryFile(true);
    job->start();

    Q_EMIT printingFinished();
}

void ViewerPrivate::slotExportHtmlPageFailed()
{
    qCDebug(MESSAGEVIEWER_LOG) << " Export HTML failed";
    Q_EMIT printingFinished();
}

void ViewerPrivate::slotPrintMessage()
{
    disconnect(mPartHtmlWriter.data(), &WebEnginePartHtmlWriter::finished, this, &ViewerPrivate::slotPrintMessage);

    if (!mMessage) {
        return;
    }
    if (mCurrentPrinter) {
        return;
    }
    mCurrentPrinter = new QPrinter();
    QPointer<QPrintDialog> dialog = new QPrintDialog(mCurrentPrinter, mMainWindow);
    dialog->setWindowTitle(i18nc("@title:window", "Print Document"));
    if (dialog->exec() != QDialog::Accepted) {
        slotHandlePagePrinted(false);
        delete dialog;
        return;
    }
    if (dialog->printer()->outputFormat() == QPrinter::PdfFormat) {
        connect(mViewer->page(), &QWebEnginePage::pdfPrintingFinished, this, &ViewerPrivate::slotPdfPrintingFinished);
        mViewer->page()->printToPdf(dialog->printer()->outputFileName(), dialog->printer()->pageLayout());
    } else {
        mViewer->page()->print(mCurrentPrinter, invoke(this, &ViewerPrivate::slotHandlePagePrinted));
    }
    delete dialog;
}

void ViewerPrivate::slotPdfPrintingFinished(const QString &filePath, bool success)
{
    Q_UNUSED(filePath)
    if (!success) {
        qCWarning(MESSAGEVIEWER_LOG) << "Print to pdf failed";
    }
    delete mCurrentPrinter;
    mCurrentPrinter = nullptr;
    Q_EMIT printingFinished();
}

void ViewerPrivate::slotHandlePagePrinted(bool result)
{
    Q_UNUSED(result)
    delete mCurrentPrinter;
    mCurrentPrinter = nullptr;
    Q_EMIT printingFinished();
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

void ViewerPrivate::initializeColorFromScheme()
{
    if (!mForegroundError.isValid()) {
        const KColorScheme scheme = KColorScheme(QPalette::Active, KColorScheme::View);
        mForegroundError = scheme.foreground(KColorScheme::NegativeText).color();
        mBackgroundError = scheme.background(KColorScheme::NegativeBackground).color();
        mBackgroundAttachment = scheme.background().color();
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
    QModelIndex index = mMimePartTree->indexAt(pos);
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
    return QList<QAction *>();
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
        mTextToSpeechWidget->say(text);
    }
}

QUrl ViewerPrivate::imageUrl() const
{
    QUrl url;
    if (mImageUrl.scheme() == QLatin1String("cid")) {
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
#ifndef QT_NO_CLIPBOARD
    QString selection = mViewer->selectedText();
    selection.replace(QChar::Nbsp, QLatin1Char(' '));
    QApplication::clipboard()->setText(selection);
#endif
}

void ViewerPrivate::viewerSelectionChanged()
{
    mActionCollection->action(QStringLiteral("kmail_copy"))->setEnabled(!mViewer->selectedText().isEmpty());
}

void ViewerPrivate::selectAll()
{
    mViewer->selectAll();
}

void ViewerPrivate::slotUrlCopy()
{
#ifndef QT_NO_CLIPBOARD
    QClipboard *clip = QApplication::clipboard();
    if (mClickedUrl.scheme() == QLatin1String("mailto")) {
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

void ViewerPrivate::scrollToAttachment(KMime::Content *node)
{
    const QString indexStr = node->index().toString();
    // The anchors for this are created in ObjectTreeParser::parseObjectTree()
    mViewer->scrollToAnchor(QLatin1String("attachmentDiv") + indexStr);

    // Remove any old color markings which might be there
    const KMime::Content *root = node->topLevel();
    const int totalChildCount = Util::allContents(root).size();
    for (int i = 0; i < totalChildCount + 1; ++i) {
        // Not optimal I need to optimize it. But for the moment it removes yellow mark
        mViewer->removeAttachmentMarking(QStringLiteral("attachmentDiv%1").arg(i + 1));
        mViewer->removeAttachmentMarking(QStringLiteral("attachmentDiv1.%1").arg(i + 1));
        mViewer->removeAttachmentMarking(QStringLiteral("attachmentDiv2.%1").arg(i + 1));
    }

    // Don't mark hidden nodes, that would just produce a strange yellow line
    if (mNodeHelper->isNodeDisplayedHidden(node)) {
        return;
    }

    // Now, color the div of the attachment in yellow, so that the user sees what happened.
    // We created a special marked div for this in writeAttachmentMarkHeader() in ObjectTreeParser,
    // find and modify that now.
    mViewer->markAttachment(QLatin1String("attachmentDiv") + indexStr, QStringLiteral("border:2px solid %1").arg(cssHelper()->pgpWarnColor().name()));
}

void ViewerPrivate::setUseFixedFont(bool useFixedFont)
{
    if (mUseFixedFont != useFixedFont) {
        mUseFixedFont = useFixedFont;
        if (mToggleFixFontAction) {
            mToggleFixFontAction->setChecked(mUseFixedFont);
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
     * some asynchronous mementos are involved in rendering. Therefor we
     * have to make sure we execute the MessageLoadedHandlers only once.
     */
    if (mMessageItem.id() == mPreviouslyViewedItemId) {
        return;
    }

    mPreviouslyViewedItemId = mMessageItem.id();

    for (AbstractMessageLoadedHandler *handler : qAsConst(mMessageLoadedHandlers)) {
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
            KMime::Message::Ptr message = mMessageItem.payload<KMime::Message::Ptr>();
            const QString email = QLatin1String(KEmailAddress::firstEmailAddress(message->from()->as7BitString(false)));
            const QStringList lst = MessageViewer::MessageViewerSettings::self()->scamDetectionWhiteList();
            if (lst.contains(email)) {
                return;
            }
        }
    }
    mScamDetectionWarning->slotShowWarning();
}

void ViewerPrivate::slotMessageIsNotAScam()
{
    if (mMessageItem.isValid()) {
        auto *attr = mMessageItem.attribute<MessageViewer::ScamAttribute>(Akonadi::Item::AddIfMissing);
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
            KMime::Message::Ptr message = mMessageItem.payload<KMime::Message::Ptr>();
            const QString email = QLatin1String(KEmailAddress::firstEmailAddress(message->from()->as7BitString(false)));
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
                    QPointer<DKIMManageRulesDialog> dlg = new DKIMManageRulesDialog(viewer());
                    dlg->exec();
                    delete dlg;
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

void ViewerPrivate::setIdentityManager(KIdentityManagement::IdentityManager *ident)
{
    mIdentityManager = ident;
}

void MessageViewer::ViewerPrivate::setFolderIdentity(uint folderIdentity)
{
    mFolderIdentity = folderIdentity;
}
