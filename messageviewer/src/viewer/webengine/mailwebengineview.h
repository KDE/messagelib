/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MAILWEBENGINE_H
#define MAILWEBENGINE_H
#include "messageviewer_export.h"
#include <MessageViewer/WebEngineView>
class KActionCollection;
namespace MessageViewer
{
class MailWebEngineViewPrivate;
class MESSAGEVIEWER_EXPORT MailWebEngineView : public MessageViewer::WebEngineView
{
    Q_OBJECT
public:
    explicit MailWebEngineView(KActionCollection *ac, QWidget *parent = Q_NULLPTR);
    ~MailWebEngineView();

    void selectAll();
    void scamCheck();

    void saveMainFrameScreenshotInFile(const QString &filename);
    void showAccessKeys();

    void setElementByIdVisible(const QString &id, bool visible);
    bool removeAttachmentMarking(const QString &id);
    void markAttachment(const QString &id, const QString &style);
    void scrollToAnchor(const QString &anchor);
public Q_SLOTS:
    void slotZoomChanged(qreal zoom);

    void slotShowDetails();

protected:
    virtual void forwardWheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    virtual void forwardKeyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void forwardKeyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void openUrl(const QUrl &url);

private Q_SLOTS:
    void handleScrollToAnchor(const QVariant &result);

private:
    MailWebEngineViewPrivate *const d;
};
}

#endif // MAILWEBENGINE_H
