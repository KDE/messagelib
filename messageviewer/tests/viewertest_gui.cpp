/* SPDX-FileCopyrightText: 2010 David Faure <faure@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "messageviewer/headerstylepluginmanager.h"
#include "messageviewer/viewer.h"
#include <KActionCollection>

#include "messageviewer_debug.h"
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QWebEngineUrlScheme>

using namespace MessageViewer;

int main(int argc, char **argv)
{
    // Necessary for "cid" support in kmail.
    QWebEngineUrlScheme cidScheme("cid");
    cidScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored | QWebEngineUrlScheme::LocalScheme
                       | QWebEngineUrlScheme::LocalAccessAllowed);

    cidScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    QWebEngineUrlScheme::registerScheme(cidScheme);

    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("+[file]"), QStringLiteral("File containing an email")));

    QCommandLineOption headerStylePluginOption(QStringList() << QStringLiteral("headerstyleplugin"),
                                               QStringLiteral("Header Style Plugin"),
                                               QStringLiteral("headerstyleplugin"));
    parser.addOption(headerStylePluginOption);

    QCommandLineOption listOption(QStringList() << QStringLiteral("list"), QStringLiteral("Show list of plugins installed."));
    parser.addOption(listOption);

    QCommandLineOption developerToolsOption(QStringList() << QStringLiteral("d") << QStringLiteral("developer-tools"),
                                            QStringLiteral("Activate Developer tools"));
    parser.addOption(developerToolsOption);

    parser.process(app);

    if (parser.isSet(listOption)) {
        qDebug() << "List of Plugin :" << MessageViewer::HeaderStylePluginManager::self()->pluginListName();
        return 0;
    }

    auto msg = new KMime::Message;
    if (parser.positionalArguments().isEmpty()) {
        QByteArray mail =
            "From: dfaure@example.com\n"
            "To: kde@example.com\n"
            "Sender: dfaure@example.com\n"
            "MIME-Version: 1.0\n"
            "Date: 02 Jul 2010 23:58:21 -0000\n"
            "Subject: Akademy\n"
            "Content-Type: text/plain\n"
            "X-Length: 0\n"
            "X-UID: 6161\n"
            "\n"
            "Hello this is a test mail\n";
        msg->setContent(mail);
    } else {
        const QString fileName = parser.positionalArguments().at(0);
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            msg->setContent(file.readAll());
        } else {
            qCWarning(MESSAGEVIEWER_LOG) << "Couldn't read" << fileName;
        }
    }
    msg->parse();

    auto viewer = new Viewer(nullptr, nullptr, new KActionCollection(&app));
    if (parser.isSet(headerStylePluginOption)) {
        viewer->setPluginName(parser.value(headerStylePluginOption));
    }
    viewer->setMessage(KMime::Message::Ptr(msg));

    if (parser.isSet(developerToolsOption)) {
        viewer->showDevelopmentTools();
    }
    viewer->resize(600, 400);
    viewer->show();

    const int ret = app.exec();

    delete viewer;
    return ret;
}
