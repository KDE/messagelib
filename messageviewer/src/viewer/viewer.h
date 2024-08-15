/*
  This file is part of KMail, the KDE mail client.
  SPDX-FileCopyrightText: 1997 Markus Wuebben <markus.wuebben@kde.org>
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer/viewerplugininterface.h"
#include "messageviewer_export.h"
#include <MimeTreeParser/Enums>

#include <KMime/MDN>
#include <KMime/Message>
#include <MessageViewer/MDNWarningWidget>

#include <QWidget>
#include <memory>

namespace Akonadi
{
class Item;
class ItemFetchJob;
}

namespace KIdentityManagementCore
{
class IdentityManager;
}

class KActionCollection;
class QAction;
class KToggleAction;
class KActionMenu;

class QAbstractItemModel;
class QCloseEvent;
class QEvent;
class QResizeEvent;

namespace WebEngineViewer
{
class WebHitTestResult;
}

namespace MessageViewer
{
class WebHitTestResult;
class DKIMWidgetInfo;
class DKIMViewerMenu;
class AttachmentStrategy;
class HeaderStylePlugin;
class CSSHelper;
class ViewerPrivate;
class Viewer;
class RemoteContentMenu;
class MDNWarningWidget;

/**
 * An interface to plug in a handler that is called when
 * an message item has been loaded into the view.
 */
class MESSAGEVIEWER_EXPORT AbstractMessageLoadedHandler
{
public:
    AbstractMessageLoadedHandler();
    virtual ~AbstractMessageLoadedHandler();

    /**
     * This method is called whenever a message item has been loaded
     * into the view.
     *
     * @param item The message item that has been loaded.
     */
    virtual void setItem(const Akonadi::Item &item) = 0;

protected:
    Akonadi::Session *session() const;

private:
    MESSAGEVIEWER_NO_EXPORT void setSession(Akonadi::Session *session);

    friend class Viewer;
    class AbstractMessageLoadedHandlerPrivate;
    std::unique_ptr<AbstractMessageLoadedHandlerPrivate> const d;
};

/**
 * This is the main widget for the viewer.
 * See the documentation of ViewerPrivate for implementation details.
 * See Mainpage.dox for an overview of the classes in the messageviewer library.
 */
class MESSAGEVIEWER_EXPORT Viewer : public QWidget
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(Viewer)

public:
    /**
     * Create a mail viewer widget
     * @param parent parent widget
     * @param widget the application's main widget
     * @param actionCollection the action collection where the widget's actions will belong to
     */
    explicit Viewer(QWidget *parent, QWidget *widget = nullptr, KActionCollection *actionCollection = nullptr);
    ~Viewer() override;

    /**
     * Returns the current message displayed in the viewer.
     */
    [[nodiscard]] KMime::Message::Ptr message() const;

    /**
     * Returns the current message item displayed in the viewer.
     */
    [[nodiscard]] Akonadi::Item messageItem() const;

    enum DisplayFormatMessage {
        UseGlobalSetting = 0,
        Text = 1,
        Html = 2,
        Unknown = 3,
        ICal = 4,
    };

    enum AttachmentAction {
        Open = 1,
        OpenWith,
        View,
        Save,
        Properties,
        Delete,
        Copy,
        ScrollTo,
        ReplyMessageToAuthor,
        ReplyMessageToAll,
    };

    enum ResourceOnlineMode {
        AllResources = 0,
        SelectedResource = 1,
    };

    /**
     * Set the message that shall be shown.
     * @param message - the message to be shown. If 0, an empty page is displayed.
     * @param updateMode - update the display immediately or not. See UpdateMode.
     */
    void setMessage(const KMime::Message::Ptr &message, MimeTreeParser::UpdateMode updateMode = MimeTreeParser::Delayed);

    /**
     * Set the Akonadi item that will be displayed.
     * @param item - the Akonadi item to be displayed. If it doesn't hold a mail (KMime::Message::Ptr as payload data),
     *               an empty page is shown.
     * @param updateMode - update the display immediately or not. See UpdateMode.
     */
    void setMessageItem(const Akonadi::Item &item, MimeTreeParser::UpdateMode updateMode = MimeTreeParser::Delayed);

    /**
     * The path to the message in terms of Akonadi collection hierarchy.
     */
    [[nodiscard]] QString messagePath() const;

    /**
     * Set the path to the message in terms of Akonadi collection hierarchy.
     */
    void setMessagePath(const QString &path);

    /**
     * Instead of settings a message to be shown sets a message part
     * to be shown
     */
    void setMessagePart(KMime::Content *aMsgPart);

    /**
     * Convenience method to clear the reader and discard the current message. Sets the internal message pointer
     * returned by message() to 0.
     * @param updateMode - update the display immediately or not. See UpdateMode.
     */
    void clear(MimeTreeParser::UpdateMode updateMode = MimeTreeParser::Delayed);

    void update(MimeTreeParser::UpdateMode updateMode = MimeTreeParser::Delayed);

    /**
     * Sets a message as the current one and print it immediately.
     * @param msg the message to display and print
     */
    void printMessage(const Akonadi::Item &msg);

    void printPreviewMessage(const Akonadi::Item &message);

    /** Print the currently displayed message */
    void print();
    void printPreview();

    /** Get the html override setting */
    [[nodiscard]] Viewer::DisplayFormatMessage displayFormatMessageOverwrite() const;

    /** Override default html mail setting */
    void setDisplayFormatMessageOverwrite(Viewer::DisplayFormatMessage format);

    /** Get the load external references override setting */
    [[nodiscard]] bool htmlLoadExtOverride() const;

    /** Default behavior for loading external references.
     *  Use this for specifying the external reference loading behavior as
     *  specified in the user settings.
     *  @see setHtmlLoadExtOverride
     */
    void setHtmlLoadExtDefault(bool loadExtDefault);

    /** Override default load external references setting
     *  @warning This must only be called when the user has explicitly
     *  been asked to retrieve external references!
     *  @see setHtmlLoadExtDefault
     */
    void setHtmlLoadExtOverride(bool loadExtOverride);

    /** Is html mail to be supported? Takes into account override */
    [[nodiscard]] bool htmlMail() const;

    /** Is loading ext. references to be supported? Takes into account override */
    [[nodiscard]] bool htmlLoadExternal() const;

    /**
     * Display a generic HTML splash page instead of a message.
     * @param templateName - the template to be loaded
     * @param data - data for the template
     * @param domain the domain.
     */
    void displaySplashPage(const QString &templateName, const QVariantHash &data, const QByteArray &domain = QByteArray());

    /** Enable the displaying of messages again after an splash (or other) page was displayed */
    void enableMessageDisplay();

    /** Returns true if the message view is scrolled to the bottom. */
    void atBottom();

    [[nodiscard]] bool isFixedFont() const;
    void setUseFixedFont(bool useFixedFont);

    [[nodiscard]] QWidget *mainWindow();

    /** Enforce message decryption. */
    void setDecryptMessageOverwrite(bool overwrite = true);

    /**
     * Initiates a delete, by sending a signal to delete the message item */
    void deleteMessage();

    [[nodiscard]] const AttachmentStrategy *attachmentStrategy() const;
    void setAttachmentStrategy(const AttachmentStrategy *strategy);

    [[nodiscard]] QString overrideEncoding() const;
    void setOverrideEncoding(const QString &encoding);
    [[nodiscard]] CSSHelper *cssHelper() const;
    void setPrinting(bool enable);

    void selectAll();
    void copySelectionToClipboard();

    void setZoomFactor(qreal zoomFactor);

    [[nodiscard]] KToggleAction *toggleFixFontAction() const;

    [[nodiscard]] KToggleAction *toggleMimePartTreeAction() const;

    [[nodiscard]] QAction *selectAllAction() const;
    [[nodiscard]] QAction *copyURLAction() const;
    [[nodiscard]] QAction *copyAction() const;
    [[nodiscard]] QAction *urlOpenAction() const;
    [[nodiscard]] QAction *speakTextAction() const;
    [[nodiscard]] QAction *copyImageLocation() const;
    [[nodiscard]] QAction *viewSourceAction() const;
    [[nodiscard]] QAction *findInMessageAction() const;
    [[nodiscard]] QAction *saveAsAction() const;
    [[nodiscard]] QAction *saveMessageDisplayFormatAction() const;
    [[nodiscard]] QAction *resetMessageDisplayFormatAction() const;
    [[nodiscard]] QAction *shareTextAction() const;

    [[nodiscard]] QAction *developmentToolsAction() const;
    [[nodiscard]] KToggleAction *disableEmoticonAction() const;
    [[nodiscard]] KActionMenu *shareServiceUrlMenu() const;
    [[nodiscard]] HeaderStylePlugin *headerStylePlugin() const;
    [[nodiscard]] MessageViewer::DKIMViewerMenu *dkimViewerMenu();

    [[nodiscard]] MessageViewer::RemoteContentMenu *remoteContentMenu() const;

    void setPluginName(const QString &pluginName);

    void writeConfig(bool withSync = true);

    [[nodiscard]] QUrl urlClicked() const;
    [[nodiscard]] QUrl imageUrlClicked() const;

    void readConfig();

    /** A QAIM tree model of the message structure. */
    QAbstractItemModel *messageTreeModel() const;

    /**
     * Create an item fetch job that is suitable for using to fetch the message item that will
     * be displayed on this viewer.
     * It will set the correct fetch scope.
     * You still need to connect to the job's result signal.
     */
    Akonadi::ItemFetchJob *createFetchJob(const Akonadi::Item &item);

    /**
     * Adds a @p handler for actions that will be executed when the message
     * has been loaded into the view.
     */
    void addMessageLoadedHandler(AbstractMessageLoadedHandler *handler);

    /**
     * Removes the @p handler for actions that will be executed when the message
     * has been loaded into the view.
     */
    void removeMessageLoadedHandler(AbstractMessageLoadedHandler *handler);

    [[nodiscard]] QString selectedText() const;

    void saveMainFrameScreenshotInFile(const QString &filename);
    [[nodiscard]] bool mimePartTreeIsEmpty() const;

    void showOpenAttachmentFolderWidget(const QList<QUrl> &urls);
    [[nodiscard]] QList<QAction *> viewerPluginActionList(MessageViewer::ViewerPluginInterface::SpecificFeatureTypes features);
    [[nodiscard]] QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;

    void runJavaScript(const QString &code);
    void setPrintElementBackground(bool printElementBackground);

    [[nodiscard]] bool printingMode() const;

    [[nodiscard]] bool showSignatureDetails() const;
    void setShowSignatureDetails(bool showDetails);

    [[nodiscard]] qreal webViewZoomFactor() const;
    void setWebViewZoomFactor(qreal factor);

    [[nodiscard]] bool showEncryptionDetails() const;
    void setShowEncryptionDetails(bool showDetails);

    void hasMultiMessages(bool messages);
    void updateShowMultiMessagesButton(bool enablePreviousButton, bool enableNextButton);
    [[nodiscard]] MessageViewer::DKIMWidgetInfo *dkimWidgetInfo();

    void exportToPdf(const QString &fileName);

    void showDevelopmentTools();

    void setIdentityManager(KIdentityManagementCore::IdentityManager *ident);
    void setFolderIdentity(uint folderIdentity);
    void showMdnInformations(const QPair<QString, bool> &mdnInfo);
    void mdnWarningAnimatedHide();
Q_SIGNALS:
    void moveMessageToTrash();
    void pageIsScrolledToBottom(bool);

    /**
     * Emitted when a status bar message is shown. Note that the status bar message is also set to
     * KPIM::BroadcastStatus in addition.
     */
    void showStatusBarMessage(const QString &message);

    /** The user presses the right mouse button. 'url' may be 0. */
    void popupMenu(const Akonadi::Item &msg, const QUrl &url, const QUrl &imageUrl, const QPoint &mousePos);
    void displayPopupMenu(const Akonadi::Item &msg, const WebEngineViewer::WebHitTestResult &result, const QPoint &mousePos);
    /**
     * The message viewer handles some types of urls itself, most notably http(s)
     * and ftp(s). When it can't handle the url it will Q_EMIT this signal.
     */
    void urlClicked(const Akonadi::Item &, const QUrl &);

    void requestConfigSync();

    /// Emitted when the content should be shown in a separate window
    void showReader(KMime::Content *aMsgPart, bool aHTML, const QString &encoding);

    /// Emitted when the message should be shown in a separate window
    void showMessage(const KMime::Message::Ptr &message, const QString &encoding);

    void replyMessageTo(const KMime::Message::Ptr &message, bool replyToAll);

    void deleteMessage(const Akonadi::Item &);

    /// Emitted when the item, previously set with setMessageItem, has been removed.
    void itemRemoved();

    void makeResourceOnline(MessageViewer::Viewer::ResourceOnlineMode mode);

    void printingFinished();
    void zoomChanged(qreal zoomFactor);
    void showNextMessage();
    void showPreviousMessage();
    void sendResponse(MessageViewer::MDNWarningWidget::ResponseType type, KMime::MDN::SendingMode sendingMode);

private:
    MESSAGEVIEWER_NO_EXPORT void initialize();
    MESSAGEVIEWER_NO_EXPORT void slotGeneralPaletteChanged();

public Q_SLOTS:

    /**
     * HTML Widget scrollbar and layout handling.
     *
     * Scrolling always happens in the direction of the slot that is called. I.e.
     * the methods take the absolute value of
     */
    void slotScrollUp();
    void slotScrollDown();
    void slotScrollPrior();
    void slotScrollNext();
    void slotJumpDown();
    void slotFind();
    void slotSaveMessage();
    void slotAttachmentSaveAs();
    void slotAttachmentSaveAll();
    void slotShowMessageSource();
    void slotZoomIn();
    void slotZoomOut();
    void slotZoomReset();
    void slotChangeDisplayMail(Viewer::DisplayFormatMessage, bool);

protected:
    /** Some necessary event handling. */
    void closeEvent(QCloseEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    /** Watch for palette changes */
    [[nodiscard]] bool event(QEvent *e) override;
    [[nodiscard]] bool eventFilter(QObject *obj, QEvent *event) override;
    void changeEvent(QEvent *event) override;

    ViewerPrivate *const d_ptr;
};
}
