/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
#include <memory>

class KActionCollection;
class QWheelEvent;
class QResizeEvent;
class QKeyEvent;
class QWebEngineView;
namespace WebEngineViewer
{
class WebEngineAccessKeyPrivate;
/*!
 * \class WebEngineViewer::WebEngineAccessKey
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/WebEngineAccessKey
 *
 * \brief The WebEngineAccessKey class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEngineAccessKey : public QObject
{
    Q_OBJECT
public:
    /*!
     * Constructs a WebEngineAccessKey object for the given webEngine with the specified parent.
     */
    explicit WebEngineAccessKey(QWebEngineView *webEngine, QObject *parent = nullptr);
    /*!
     * Destroys the WebEngineAccessKey object.
     */
    ~WebEngineAccessKey() override;

    /*!
     * Sets the action collection for the access keys.
     */
    void setActionCollection(KActionCollection *ac);

    /*!
     * Handles a wheel event.
     */
    void wheelEvent(QWheelEvent *e);
    /*!
     * Handles a resize event.
     */
    void resizeEvent(QResizeEvent *);
    /*!
     * Handles a key press event.
     */
    void keyPressEvent(QKeyEvent *e);
    /*!
     * Handles a key release event.
     */
    void keyReleaseEvent(QKeyEvent *e);

    /*!
     * Shows the access keys.
     */
    void showAccessKeys();
    /*!
     * Hides the access keys.
     */
    void hideAccessKeys();

Q_SIGNALS:
    /*!
     * Emitted when a URL should be opened.
     */
    void openUrl(const QUrl &url);

private:
    WEBENGINEVIEWER_NO_EXPORT void handleSearchAccessKey(const QVariant &res);
    std::unique_ptr<WebEngineAccessKeyPrivate> const d;
};
}
