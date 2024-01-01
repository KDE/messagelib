/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messageviewer_export.h"
#include <QWidget>
class QTreeWidget;
namespace MessageViewer
{
class DKIMManagerKeyTreeView;
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
    [[nodiscard]] QByteArray saveHeaders() const;
    void restoreHeaders(const QByteArray &header);

private:
    MESSAGEVIEWER_NO_EXPORT void slotCustomContextMenuRequested(const QPoint &);
    DKIMManagerKeyTreeView *const mDKIMManagerKeyTreeView;
};
}
