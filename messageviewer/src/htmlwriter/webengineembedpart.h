/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QMap>
#include <QObject>
namespace MessageViewer
{
class WebEngineEmbedPart : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineEmbedPart(QObject *parent = nullptr);
    ~WebEngineEmbedPart() override;

    void clear();
    [[nodiscard]] bool isEmpty() const;

    [[nodiscard]] QMap<QString, QString> embeddedPartMap() const;

    void addEmbedPart(const QByteArray &contentId, const QString &contentURL);
    [[nodiscard]] QString contentUrl(const QString &contentId) const;

    static WebEngineEmbedPart *self();

private:
    // Key is Content-Id, value is URL
    QMap<QString, QString> mEmbeddedPartMap;
};
}
