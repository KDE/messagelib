/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messageviewer_export.h"
#include <WebEngineViewer/WebEnginePage>

namespace MessageViewer
{
/**
 * @brief The MailWebEnginePage class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MailWebEnginePage : public WebEngineViewer::WebEnginePage
{
    Q_OBJECT
public:
    explicit MailWebEnginePage(QObject *parent = nullptr);
    ~MailWebEnginePage() override = default;

    void setPrintElementBackground(bool printElementBackground);

private:
    MESSAGEVIEWER_NO_EXPORT void slotFeaturePermissionRequested(const QUrl &url, QWebEnginePage::Feature feature);
    MESSAGEVIEWER_NO_EXPORT void initialize();
};
}
