/*
   SPDX-FileCopyrightText: 2019-2023 Laurent Montel <montel@kde.org>

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

protected:
    Q_REQUIRED_RESULT bool event(QEvent *e) override;

private:
    MESSAGEVIEWER_NO_EXPORT void updateInfo();
    MESSAGEVIEWER_NO_EXPORT void updateToolTip();
    MESSAGEVIEWER_NO_EXPORT void initColors();
    MESSAGEVIEWER_NO_EXPORT void updatePalette();
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult mResult;
    QLabel *const mLabel;
    QColor mWarningColor;
    QColor mErrorColor;
    QColor mOkColor;
    QColor mDefaultColor;
    Akonadi::Item::Id mCurrentItemId = -1;
};
}
