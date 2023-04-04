/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QByteArray>
#include <QHash>
#include <QString>

#include <vector>

namespace MessageViewer
{
class MessagePartRendererBase;

struct RendererInfo {
    QSharedPointer<MessagePartRendererBase> renderer;
    QString mimeType;
    int priority;
};

class MessagePartRendererFactoryPrivate
{
public:
    ~MessagePartRendererFactoryPrivate();
    void setup();
    void loadPlugins();
    void initialize_builtin_renderers();
    void insert(const QByteArray &type, MessagePartRendererBase *formatter, const QString &mimeType = QString(), int priority = 0);

    QHash<QByteArray, std::vector<RendererInfo>> m_renderers;
    QString m_pluginSubdir = QStringLiteral("pim6/messageviewer/bodypartformatter");
};
}
