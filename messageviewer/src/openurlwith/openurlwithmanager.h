/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include "openwithurlinfo.h"
#include <QList>
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

    void clear();

    [[nodiscard]] const QList<OpenWithUrlInfo> &openWithUrlInfo() const;
    void setOpenWithUrlInfo(const QList<OpenWithUrlInfo> &newOpenWithUrlInfo);
    void saveRules();

    [[nodiscard]] OpenWithUrlInfo openWith(const QUrl &url);

private:
    MESSAGEVIEWER_NO_EXPORT void loadSettings();
    MESSAGEVIEWER_NO_EXPORT void loadUserSettings();
    MESSAGEVIEWER_NO_EXPORT void loadGlobalSettings();

    QList<OpenWithUrlInfo> mOpenWithUrlInfo;
};
}
