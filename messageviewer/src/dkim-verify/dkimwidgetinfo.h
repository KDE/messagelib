/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMWIDGETINFO_H
#define DKIMWIDGETINFO_H

#include <QWidget>
#include "dkimchecksignaturejob.h"
#include "messageviewer_export.h"
#include <AkonadiCore/Item>
class QLabel;
namespace MessageViewer {
/**
 * @brief The DKIMWidgetInfo class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMWidgetInfo : public QWidget
{
    Q_OBJECT
public:
    explicit DKIMWidgetInfo(QWidget *parent = nullptr);
    ~DKIMWidgetInfo();
    void setResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id);
    void clear();

    Q_REQUIRED_RESULT Akonadi::Item::Id currentItemId() const;
    void setCurrentItemId(Akonadi::Item::Id currentItemId);

    Q_REQUIRED_RESULT MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult result() const;

private:
    void updateInfo();
    void updateToolTip();
    void initColors();
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult mResult;
    QLabel *mLabel = nullptr;
    QColor mWarningColor;
    QColor mErrorColor;
    QColor mOkColor;
    QColor mDefaultColor;
    Akonadi::Item::Id mCurrentItemId = -1;
};
}

#endif // DKIMWIDGETINFO_H
