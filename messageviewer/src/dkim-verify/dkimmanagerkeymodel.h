/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once
#include "messageviewer_export.h"
#include <QAbstractListModel>

class MESSAGEVIEWER_EXPORT DKIMManagerKeyModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyModel(QObject *parent = nullptr);
    ~DKIMManagerKeyModel() override;
};
