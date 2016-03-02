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

#include "mailwebengineaccesskey.h"

#include <KActionCollection>
#include <QKeyEvent>

using namespace MessageViewer;

class MessageViewer::MailWebEngineAccessKeyPrivate
{
public:
    enum AccessKeyState {
        NotActivated,
        PreActivated,
        Activated
    };

    MailWebEngineAccessKeyPrivate()
        : mAccessKeyActivated(NotActivated),
          mActionCollection(Q_NULLPTR)
    {

    }
    AccessKeyState mAccessKeyActivated;
    KActionCollection *mActionCollection;
};

MailWebEngineAccessKey::MailWebEngineAccessKey(QObject *parent)
    : QObject(parent),
      d(new MessageViewer::MailWebEngineAccessKeyPrivate)
{

}

MailWebEngineAccessKey::~MailWebEngineAccessKey()
{
    delete d;
}

void MailWebEngineAccessKey::setActionCollection(KActionCollection *ac)
{
    d->mActionCollection = ac;
}

void MailWebEngineAccessKey::wheelEvent(QWheelEvent *e)
{
    if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::PreActivated && (e->modifiers() & Qt::ControlModifier)) {
        d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::NotActivated;
    }
}

void MailWebEngineAccessKey::resizeEvent(QResizeEvent *)
{
    if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::Activated) {
        //FIXME hideAccessKeys();
    }
}

void MailWebEngineAccessKey::keyPressEvent(QKeyEvent *e)
{
#if 0
    if (e && d->mWebView->hasFocus()) {
        if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::Activated) {
#if 0 //FIXME
            if (checkForAccessKey(e)) {
                hideAccessKeys();
                e->accept();
                return;
            }
            hideAccessKeys();
#endif
        } else if (e->key() == Qt::Key_Control && e->modifiers() == Qt::ControlModifier && !isEditableElement(d->mWebView->page())) {
            d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::PreActivated; // Only preactive here, it will be actually activated in key release.
        }
    }
#endif
}

void MailWebEngineAccessKey::keyReleaseEvent(QKeyEvent *e)
{
    if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::PreActivated) {
        // Activate only when the CTRL key is pressed and released by itself.
        if (e->key() == Qt::Key_Control && e->modifiers() == Qt::NoModifier) {
            //FIXME showAccessKeys();
            d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::Activated;
        } else {
            d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::NotActivated;
        }
    }
}
