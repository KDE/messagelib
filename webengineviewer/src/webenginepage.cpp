/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webenginepage.h"
#include "webenginemanagescript.h"
#include "webhittest.h"

#include <KLocalizedString>

#include <KColorScheme>
#include <QEventLoop>
#include <QFileDialog>
#include <QGuiApplication>
#include <QPointer>
#include <QPrinter>
#include <QTimer>
#include <QWebEngineDownloadRequest>
#include <QWebEngineProfile>
#include <QWebEngineScriptCollection>
#include <QWebEngineView>

using namespace WebEngineViewer;

class WebEnginePage::Private
{
public:
    std::optional<QPalette> lastPalette;
    std::optional<KColorScheme> lastColorScheme;
    QWebEngineScript refreshScript;
};

WebEnginePage::WebEnginePage(QObject *parent)
    : QWebEnginePage(/*new QWebEngineProfile, */ parent)
    , d(std::make_unique<WebEnginePage::Private>())
{
    // Create a private (off the record) QWebEngineProfile here to isolate the
    // browsing settings, and adopt it as a child so that it will be deleted
    // when we are destroyed.  The profile must remain active for as long as
    // any QWebEnginePage's belonging to it exist, see the API documentation
    // of QWebEnginePage::QWebEnginePage(QWebEngineProfile *, QObject *).
    // Deleting it as our child on destruction is safe.
    //
    // Do not try to save a line of code by setting the parent on construction:
    //
    //    WebEnginePage::WebEnginePage(QObject *parent)
    //      : QWebEnginePage(new QWebEngineProfile(this), parent)
    //
    // because the QWebEngineProfile constructor will call out to the QWebEnginePage
    // and crash because the QWebEnginePage is not fully constructed yet.
    // profile()->setParent(this);

    init();
}

WebEnginePage::WebEnginePage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent)
    , d(std::make_unique<WebEnginePage::Private>())
{
    init();
}

WebEnginePage::~WebEnginePage() = default;

void WebEnginePage::init()
{
    connect(profile(), &QWebEngineProfile::downloadRequested, this, &WebEnginePage::saveHtml);
    qGuiApp->installEventFilter(this);

    // Expose main colors from KColorScheme
    d->refreshScript.setName(QStringLiteral("injectColorScheme"));
    d->refreshScript.setSourceCode(refreshCssVariablesScript());
    d->refreshScript.setInjectionPoint(QWebEngineScript::DocumentReady);
    d->refreshScript.setRunsOnSubFrames(true);
    d->refreshScript.setWorldId(QWebEngineScript::ApplicationWorld);
    scripts().insert(d->refreshScript);
}

bool WebEnginePage::eventFilter(QObject *obj, QEvent *event)
{
    if (obj != qGuiApp) {
        return false;
    }

    if (event->type() == QEvent::ApplicationPaletteChange) {
        // Refresh main colors from KColorScheme
        auto cssVariablesScript = refreshCssVariablesScript();
        runJavaScript(cssVariablesScript, WebEngineViewer::WebEngineManageScript::scriptWordId());

        d->refreshScript.setSourceCode(cssVariablesScript);
        scripts().remove(d->refreshScript);
        scripts().insert(d->refreshScript);
    }
    return QObject::eventFilter(obj, event);
}

QString WebEnginePage::refreshCssVariablesScript()
{
    // Same logic as KIconColors
    auto palette = qGuiApp->palette();
    if (!d->lastColorScheme || !d->lastPalette || palette != d->lastPalette) {
        d->lastPalette = palette;
        d->lastColorScheme = KColorScheme(QPalette::Active, KColorScheme::View);
    }

    const auto script = QStringLiteral(R"RAW(
{
    const root = document.querySelector(':root');
    root.style.setProperty('--kcolorscheme-background', '%1');
    root.style.setProperty('--kcolorscheme-text', '%2');
    root.style.setProperty('--kcolorscheme-highlight', '%3');
    root.style.setProperty('--kcolorscheme-highlight-text', '%4');
    root.style.setProperty('--kcolorscheme-accent', '%5');
    root.style.setProperty('--kcolorscheme-positive-text', '%6');
    root.style.setProperty('--kcolorscheme-positive-background', '%7');
    root.style.setProperty('--kcolorscheme-neutral-text', '%8');
    root.style.setProperty('--kcolorscheme-neutral-background', '%9');
    root.style.setProperty('--kcolorscheme-negative-text', '%10');
    root.style.setProperty('--kcolorscheme-negative-background', '%11');
    root.style.setProperty('--kcolorscheme-active-text', '%12');
    root.style.setProperty('--kcolorscheme-active-background', '%13');
    root.style.setProperty('--kcolorscheme-text-disabled', '%14');
    root.style.setProperty('--kcolorscheme-text-link', '%15');
}
    )RAW")
                            .arg(d->lastColorScheme->background(KColorScheme::NormalBackground).color().name(),
                                 d->lastColorScheme->foreground(KColorScheme::NormalText).color().name(),
                                 palette.highlight().color().name(),
                                 palette.highlightedText().color().name(),
                                 palette.accent().color().name(),
                                 d->lastColorScheme->foreground(KColorScheme::PositiveText).color().name(),
                                 d->lastColorScheme->background(KColorScheme::PositiveBackground).color().name(),
                                 d->lastColorScheme->foreground(KColorScheme::NeutralText).color().name(),
                                 d->lastColorScheme->background(KColorScheme::NeutralBackground).color().name(),
                                 d->lastColorScheme->foreground(KColorScheme::NegativeText).color().name(),
                                 d->lastColorScheme->background(KColorScheme::NegativeBackground).color().name(),
                                 d->lastColorScheme->foreground(KColorScheme::ActiveText).color().name(),
                                 d->lastColorScheme->background(KColorScheme::ActiveBackground).color().name(),
                                 d->lastColorScheme->foreground(KColorScheme::InactiveText).color().name(),
                                 d->lastColorScheme->foreground(KColorScheme::LinkText).color().name());

    return script;
}

WebEngineViewer::WebHitTest *WebEnginePage::hitTestContent(const QPoint &pos)
{
    return new WebHitTest(this, mapToViewport(pos), pos);
}

QPoint WebEnginePage::mapToViewport(const QPoint &pos) const
{
    return QPoint(pos.x() / zoomFactor(), pos.y() / zoomFactor());
}

void WebEnginePage::saveHtml(QWebEngineDownloadRequest *download)
{
    const QString fileName = QFileDialog::getSaveFileName(QWebEngineView::forPage(this), i18n("Save HTML Page"));
    if (!fileName.isEmpty()) {
        download->setSavePageFormat(QWebEngineDownloadRequest::SingleHtmlSaveFormat);
        download->setDownloadDirectory(QFileInfo(fileName).path());
        download->setDownloadFileName(QFileInfo(fileName).fileName());
        download->accept();
    }
}

bool WebEnginePage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
{
    if (isMainFrame && type == NavigationTypeLinkClicked) {
        Q_EMIT urlClicked(url);
        return false;
    }
    return true;
}

void WebEnginePage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                                             const QString &message,
                                             int lineNumber,
                                             const QString &sourceID)
{
    Q_UNUSED(level)
    Q_UNUSED(sourceID)
    // Don't convert to debug categories
    qDebug() << "WebEnginePage::javaScriptConsoleMessage lineNumber: " << lineNumber << " message: " << message;
    Q_EMIT showConsoleMessage(message);
}

#include "moc_webenginepage.cpp"
