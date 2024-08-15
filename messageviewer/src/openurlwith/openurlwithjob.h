/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "openwithurlinfo.h"
#include <QUrl>

#include <QObject>
namespace MessageViewer
{
class OpenUrlWithJob : public QObject
{
    Q_OBJECT
public:
    explicit OpenUrlWithJob(QObject *parent = nullptr);
    ~OpenUrlWithJob() override;

    [[nodiscard]] bool canStart() const;

    void start();

    [[nodiscard]] const OpenWithUrlInfo &info() const;
    void setInfo(const OpenWithUrlInfo &newInfo);
    void setUrl(const QUrl &url);

private:
    QUrl mUrl;
    OpenWithUrlInfo mInfo;
};
}
