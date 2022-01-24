/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
namespace MessageViewer
{
class MESSAGEVIEWER_EXPORT OpenUrlWithManager : public QObject
{
    Q_OBJECT
public:
    explicit OpenUrlWithManager(QObject *parent = nullptr);
    ~OpenUrlWithManager() override;

    static OpenUrlWithManager *self();
};
}
