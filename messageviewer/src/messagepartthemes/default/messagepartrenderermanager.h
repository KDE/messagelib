/*
   SPDX-FileCopyrightText: 2016-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <QObject>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <grantlee/template.h>
#else
#include <KTextTemplate/Template>
#endif

#include <QMetaType>

#include <functional>

namespace GrantleeTheme
{
class Engine;
}

namespace MessageViewer
{
class GlobalContext;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using GrantleeCallback = std::function<void(Grantlee::OutputStream *)>;
#else
using GrantleeCallback = std::function<void(KTextTemplate::OutputStream *)>;
#endif
/**
 * @brief The MessagePartRendererManager class
 */
class MESSAGEVIEWER_EXPORT MessagePartRendererManager : public QObject
{
    Q_OBJECT
public:
    explicit MessagePartRendererManager(QObject *parent = nullptr);
    ~MessagePartRendererManager() override;
    static MessagePartRendererManager *self();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_REQUIRED_RESULT Grantlee::Template loadByName(const QString &name);
    Q_REQUIRED_RESULT Grantlee::Context createContext();
#else
    Q_REQUIRED_RESULT KTextTemplate::Template loadByName(const QString &name);
    Q_REQUIRED_RESULT KTextTemplate::Context createContext();
#endif

private:
    void initializeRenderer();
    GrantleeTheme::Engine *m_engine = nullptr;
    GlobalContext *const m_globalContext;
};
}

Q_DECLARE_METATYPE(MessageViewer::GrantleeCallback)
