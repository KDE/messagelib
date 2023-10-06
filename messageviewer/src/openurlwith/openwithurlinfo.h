/*
   SPDX-FileCopyrightText: 2022-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <QDebug>
namespace MessageViewer
{
/**
 * @brief The OpenWithUrlInfo class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT OpenWithUrlInfo
{
    Q_GADGET
public:
    OpenWithUrlInfo();
    ~OpenWithUrlInfo();

    [[nodiscard]] const QString &command() const;
    void setCommand(const QString &newCommand);

    [[nodiscard]] const QString &url() const;
    void setUrl(const QString &newUrl);

    [[nodiscard]] bool isValid() const;

    [[nodiscard]] const QString &commandLine() const;
    void setCommandLine(const QString &newCommandLine);

    [[nodiscard]] bool operator==(const OpenWithUrlInfo &other) const;

private:
    QString mCommandLine;
    QString mCommand;
    QString mUrl;
};
}
Q_DECLARE_TYPEINFO(MessageViewer::OpenWithUrlInfo, Q_RELOCATABLE_TYPE);
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::OpenWithUrlInfo &t);
