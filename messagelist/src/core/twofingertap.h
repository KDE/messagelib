/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2021 Steffen Hartleib <steffenhartleib@t-online.de>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QGesture>
#include <QGestureRecognizer>

namespace MessageList
{
namespace Core
{

class TwoFingerTap : public QGesture
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(QPointF screenPos READ screenPos WRITE setScreenPos)
    Q_PROPERTY(QPointF scenePos READ scenePos WRITE setScenePos)
public:
    explicit TwoFingerTap(QObject *parent = nullptr);
    ~TwoFingerTap() override;
    Q_REQUIRED_RESULT QPointF pos() const;
    void setPos(QPointF pos);
    Q_REQUIRED_RESULT QPointF screenPos() const;
    void setScreenPos(QPointF screenPos);
    Q_REQUIRED_RESULT QPointF scenePos() const;
    void setScenePos(QPointF scenePos);
private:
    QPointF mPos = QPointF(-1, -1);
    QPointF mScreenPos = QPointF(-1, -1);
    QPointF mScenePos = QPointF(-1, -1);
}; // class TwoFingerTap

class TwoFingerTapRecognizer : public QGestureRecognizer
{
public:
    explicit TwoFingerTapRecognizer();
    ~TwoFingerTapRecognizer() override;
    QGesture* create(QObject *) override; 
    Result recognize(QGesture *, QObject *, QEvent *) override;
private:
    Q_DISABLE_COPY(TwoFingerTapRecognizer)
    bool mGestureTriggered = false;
}; // class TwoFingerTapRecognizer
} // namespace Core
} // namespace MessageList
