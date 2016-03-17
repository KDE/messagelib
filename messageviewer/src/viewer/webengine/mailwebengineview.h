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

public Q_SLOTS:
    void slotZoomChanged(qreal zoom);

    void slotShowDetails();

protected:
    void keyReleaseEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void openUrl(const QUrl &url);
private:
    MailWebEngineViewPrivate *const d;
};
}

#endif // MAILWEBENGINE_H
