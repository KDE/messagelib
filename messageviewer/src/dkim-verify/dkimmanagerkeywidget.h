/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef DKIMMANAGERKEYWIDGET_H
#define DKIMMANAGERKEYWIDGET_H

#include "messageviewer_export.h"
#include <QWidget>
class QTreeWidget;
namespace MessageViewer
{
/**
 * @brief The DKIMManagerKeyWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManagerKeyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyWidget(QWidget *parent = nullptr);
    ~DKIMManagerKeyWidget() override;

    void loadKeys();
    void saveKeys();
    void resetKeys();
    Q_REQUIRED_RESULT QByteArray saveHeaders() const;
    void restoreHeaders(const QByteArray &header);

private:
    void slotCustomContextMenuRequested(const QPoint &);
    QTreeWidget *mTreeWidget = nullptr;
};
}

#endif // DKIMMANAGERKEYWIDGET_H
