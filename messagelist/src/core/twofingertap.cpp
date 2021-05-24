/******************************************************************************
 *
 * SPDX-FileCopyrightText: 2021 Steffen Hartleib <steffenhartleib@t-online.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

// Self
#include "twofingertap.h"

// Qt
#include <QApplication>
#include <QGraphicsWidget>
#include <QTouchEvent>

using namespace MessageList::Core;

TwoFingerTapRecognizer::TwoFingerTapRecognizer()
    : QGestureRecognizer()
{
}

TwoFingerTapRecognizer::~TwoFingerTapRecognizer()
{
}

QGesture* TwoFingerTapRecognizer::create(QObject *)
{
    return static_cast<QGesture*>(new TwoFingerTap());
}

QGestureRecognizer::Result TwoFingerTapRecognizer::recognize(QGesture *gesture, QObject *watched, QEvent *event)
{
    if (qobject_cast<QGraphicsWidget *>(watched)) {
        return Ignore;
    }

    TwoFingerTap *twoFingerTap = static_cast<TwoFingerTap *>(gesture);
    const QTouchEvent *touchEvent = static_cast<const QTouchEvent *>(event);

    switch (event->type()) {
    case QEvent::TouchBegin: {
        twoFingerTap->setHotSpot(touchEvent->touchPoints().first().startScreenPos());
        twoFingerTap->setPos(touchEvent->touchPoints().first().startPos());
        twoFingerTap->setScreenPos(touchEvent->touchPoints().first().startScreenPos());
        twoFingerTap->setScenePos(touchEvent->touchPoints().first().startScenePos());
        mGestureTriggered = false;
        return MayBeGesture;
    }

    case QEvent::TouchUpdate: {
        const int touchPointSize = touchEvent->touchPoints().size();
        if (touchPointSize > 2) {
            mGestureTriggered = false;
            return CancelGesture;
        }

        if (touchPointSize == 2) {
            if ((touchEvent->touchPoints().first().startPos() - touchEvent->touchPoints().first().pos()).manhattanLength() >= QApplication::startDragDistance()) {
                mGestureTriggered = false;
                return CancelGesture;
            }
            if ((touchEvent->touchPoints().at(1).startPos() - touchEvent->touchPoints().at(1).pos()).manhattanLength() >= QApplication::startDragDistance()) {
                mGestureTriggered = false;
                return CancelGesture;
            }
            if (touchEvent->touchPointStates() & Qt::TouchPointPressed) {
                mGestureTriggered = true;
            }
            if (touchEvent->touchPointStates() & Qt::TouchPointReleased && mGestureTriggered) {
                mGestureTriggered = false;
                return FinishGesture;
            }
        }
        break;
    }

    default:
        return Ignore;
    }
    return Ignore;
}

TwoFingerTap::TwoFingerTap(QObject *parent)
    : QGesture(parent)
{
}

TwoFingerTap::~TwoFingerTap()
{
}

QPointF TwoFingerTap::pos() const
{
    return mPos;
}

void TwoFingerTap::setPos(QPointF _pos)
{
    mPos = _pos;
}

QPointF TwoFingerTap::screenPos() const
{
    return mScreenPos;
}

void TwoFingerTap::setScreenPos(QPointF _screenPos)
{
    mScreenPos = _screenPos;
}

QPointF TwoFingerTap::scenePos() const
{
    return mScenePos;
}

void TwoFingerTap::setScenePos(QPointF _scenePos)
{
    mScenePos = _scenePos;
}
