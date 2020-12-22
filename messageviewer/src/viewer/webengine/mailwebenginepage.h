/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef MAILWEBENGINEPAGE_H
#define MAILWEBENGINEPAGE_H

#include "messageviewer_export.h"
#include <WebEngineViewer/WebEnginePage>

namespace MessageViewer {
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
    void slotFeaturePermissionRequested(const QUrl &url, QWebEnginePage::Feature feature);
    void initialize();
};
}
#endif // MAILWEBENGINEPAGE_H
