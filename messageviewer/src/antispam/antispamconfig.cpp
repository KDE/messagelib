/*
    antispamconfig.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Patrick Audley <paudley@blackcat.ca>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "antispamconfig.h"

#include <KConfig>
#include <KConfigGroup>

using namespace MessageViewer;

AntiSpamConfig *AntiSpamConfig::instance()
{
    static AntiSpamConfig s_self;
    return &s_self;
}

const SpamAgents AntiSpamConfig::agents() const
{
    return mAgents;
}

AntiSpamConfig::AntiSpamConfig()
{
    readConfig();
}

AntiSpamConfig::~AntiSpamConfig() = default;

void AntiSpamConfig::readConfig()
{
    mAgents.clear();
    KConfig config(QStringLiteral("kmail.antispamrc"));
    KConfigGroup general(&config, QStringLiteral("General"));
    int totalTools = general.readEntry("tools", 0);
    for (int i = 1; i <= totalTools; ++i) {
        KConfigGroup tool(&config, QStringLiteral("Spamtool #%1").arg(i));
        if (tool.hasKey("ScoreHeader")) {
            const QString name = tool.readEntry("ScoreName");
            const QByteArray header = tool.readEntry("ScoreHeader").toLatin1();
            const QByteArray cheader = tool.readEntry("ConfidenceHeader").toLatin1();
            const QByteArray type = tool.readEntry("ScoreType").toLatin1();
            const QString score = tool.readEntryUntranslated("ScoreValueRegexp");
            const QString threshold = tool.readEntryUntranslated("ScoreThresholdRegexp");
            const QString confidence = tool.readEntryUntranslated("ScoreConfidenceRegexp");
            SpamAgentTypes typeE = SpamAgentNone;
            if (qstricmp(type.data(), "bool") == 0) {
                typeE = SpamAgentBool;
            } else if (qstricmp(type.data(), "decimal") == 0) {
                typeE = SpamAgentFloat;
            } else if (qstricmp(type.data(), "percentage") == 0) {
                typeE = SpamAgentFloatLarge;
            } else if (qstricmp(type.data(), "adjusted") == 0) {
                typeE = SpamAgentAdjustedFloat;
            }
            mAgents.append(SpamAgent(name, typeE, header, cheader, QRegularExpression(score), QRegularExpression(threshold), QRegularExpression(confidence)));
        }
    }
}

const SpamAgents AntiSpamConfig::uniqueAgents() const
{
    QStringList seenAgents;
    SpamAgents agents;
    SpamAgents::ConstIterator it(mAgents.begin());
    SpamAgents::ConstIterator end(mAgents.end());
    for (; it != end; ++it) {
        const QString agent((*it).name());
        if (!seenAgents.contains(agent)) {
            agents.append(*it);
            seenAgents.append(agent);
        }
    }
    return agents;
}
