/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include <QObject>

#include <KTextTemplate/Node>
#include <KTextTemplate/TagLibraryInterface>

#include <functional>

namespace MessageViewer
{
using KTextTemplateCallback = std::function<void(KTextTemplate::OutputStream *)>;

class CallbackTag : public KTextTemplate::Node
{
    Q_OBJECT
public:
    explicit CallbackTag(const QString &name, QObject *parent = nullptr);
    void render(KTextTemplate::OutputStream *stream, KTextTemplate::Context *context) const override;

private:
    const QString m_name;
};

class CallbackTagFactory : public KTextTemplate::AbstractNodeFactory
{
    Q_OBJECT
public:
    KTextTemplate::Node *getNode(const QString &tagContent, KTextTemplate::Parser *parser) const override;
};
class KTextTemplateTagLibrary : public QObject, public KTextTemplate::TagLibraryInterface
{
    Q_OBJECT
    Q_INTERFACES(KTextTemplate::TagLibraryInterface)
    Q_PLUGIN_METADATA(IID "org.grantlee.TagLibraryInterface")
public:
    explicit KTextTemplateTagLibrary(QObject *parent = nullptr);
    [[nodiscard]] QHash<QString, KTextTemplate::AbstractNodeFactory *> nodeFactories(const QString &name) override;
};
}

Q_DECLARE_METATYPE(MessageViewer::KTextTemplateCallback)
