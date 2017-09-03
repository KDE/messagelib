/*
  Copyright (c) 2013-2017 Montel Laurent <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/

#ifndef SCAMDETECTIONWARNINGWIDGET_H
#define SCAMDETECTIONWARNINGWIDGET_H

#include "messageviewer_export.h"

#include <KMessageWidget>

namespace MessageViewer {
class ScamDetectionWarningWidgetPrivate;
class MESSAGEVIEWER_EXPORT ScamDetectionWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit ScamDetectionWarningWidget(QWidget *parent = nullptr);
    ~ScamDetectionWarningWidget();

    void setUseInTestApps(bool b);

public Q_SLOTS:
    void slotShowWarning();

Q_SIGNALS:
    void showDetails();
    void moveMessageToTrash();
    void messageIsNotAScam();
    void addToWhiteList();

private:
    void slotShowDetails(const QString &content);
    void slotAddToWhiteList();
    void slotDisableScamDetection();
    void slotMessageIsNotAScam();

private:
    ScamDetectionWarningWidgetPrivate *const d;
};
}

#endif // SCAMDETECTIONWARNINGWIDGET_H
