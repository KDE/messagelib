/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimchecksignaturejob.h"
#include "messageviewer_export.h"
#include <Akonadi/Item>
#include <QWidget>

class QLabel;

namespace MessageViewer
{
class DKIMMailStatus;
/**
 * @brief The DKIMWidgetInfo class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMWidgetInfo : public QWidget
{
    Q_OBJECT
public:
    explicit DKIMWidgetInfo(QWidget *parent = nullptr);
    ~DKIMWidgetInfo() override;
    void setResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id);
    void clear();

    Q_REQUIRED_RESULT Akonadi::Item::Id currentItemId() const;
    void setCurrentItemId(Akonadi::Item::Id currentItemId);

    Q_REQUIRED_RESULT MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult result() const;

    void onStatusChanged(MessageViewer::DKIMCheckSignatureJob::DKIMStatus status);

private:
    void initColors();

    QLabel *const mLabel;
    DKIMMailStatus *const mDKIMMailStatus;
    QColor mWarningColor;
    QColor mErrorColor;
    QColor mOkColor;
    QColor mDefaultColor;
};
}

