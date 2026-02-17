/*
   SPDX-FileCopyrightText: 2022-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include "openwithurlinfo.h"
#include <QList>
#include <QObject>
namespace MessageViewer
{
/*!
 * \class MessageViewer::OpenUrlWithManager
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/OpenUrlWithManager
 * \brief The OpenUrlWithManager class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT OpenUrlWithManager : public QObject
{
    Q_OBJECT
public:
    /*! Constructs an OpenUrlWithManager object with the given parent. */
    explicit OpenUrlWithManager(QObject *parent = nullptr);
    /*! Destroys the OpenUrlWithManager object. */
    ~OpenUrlWithManager() override;

    /*! Returns the singleton instance of OpenUrlWithManager. */
    static OpenUrlWithManager *self();

    /*! Clears all the open with URL information. */
    void clear();

    /*! Returns the list of open with URL information. */
    [[nodiscard]] const QList<OpenWithUrlInfo> &openWithUrlInfo() const;
    /*! Sets the open with URL information list. */
    void setOpenWithUrlInfo(const QList<OpenWithUrlInfo> &newOpenWithUrlInfo);
    /*! Saves the rules to persistent storage. */
    void saveRules();

    /*! Returns the open with URL information for the given URL. */
    [[nodiscard]] OpenWithUrlInfo openWith(const QUrl &url);
    /*! Returns whether there is an always rule for the given URL's host. */
    [[nodiscard]] bool alwaysRuleForHost(const QUrl &url);

    /*! Returns the list of configured hosts. */
    [[nodiscard]] QStringList hosts() const;

private:
    MESSAGEVIEWER_NO_EXPORT void loadSettings();
    MESSAGEVIEWER_NO_EXPORT void loadUserSettings();
    MESSAGEVIEWER_NO_EXPORT void loadGlobalSettings();
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QStringList openUrlWithListPath() const;

    QList<OpenWithUrlInfo> mOpenWithUrlInfo;
};
}
