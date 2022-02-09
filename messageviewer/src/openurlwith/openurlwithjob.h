/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "openwithurlinfo.h"

#include <QObject>
namespace MessageViewer
{
class OpenUrlWithJob : public QObject
{
    Q_OBJECT
public:
    explicit OpenUrlWithJob(QObject *parent = nullptr);
    ~OpenUrlWithJob() override;

    Q_REQUIRED_RESULT bool canStart() const;

    void start();

    const OpenWithUrlInfo &info() const;
    void setInfo(const OpenWithUrlInfo &newInfo);

private:
    OpenWithUrlInfo mInfo;
};
}
