/*
    antispamconfig.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Patrick Audley <paudley@blackcat.ca>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QRegExp>
#include <QVector>

class QString;

namespace MessageViewer
{
/// Valid types of SpamAgent
enum SpamAgentTypes {
    SpamAgentNone, //!< Invalid SpamAgent, skip this agent
    SpamAgentBool, //!< Simple Yes or No (Razor)
    SpamAgentFloat, //!< For straight percentages between 0.0 and 1.0 (BogoFilter)
    SpamAgentFloatLarge, //!< For straight percentages between 0.0 and 100.0
    SpamAgentAdjustedFloat //!< Use this when we need to compare against a threshold (SpamAssasssin)
};

class SpamAgent
{
public:
    SpamAgent()
        : mType(SpamAgentNone)
    {
    }

    SpamAgent(const QString &name,
              SpamAgentTypes type,
              const QByteArray &field,
              const QByteArray &cfield,
              const QRegExp &score,
              const QRegExp &threshold,
              const QRegExp &confidence)
        : mName(name)
        , mType(type)
        , mField(field)
        , mConfidenceField(cfield)
        , mScore(score)
        , mThreshold(threshold)
        , mConfidence(confidence)
    {
    }

    Q_REQUIRED_RESULT QString name() const
    {
        return mName;
    }

    Q_REQUIRED_RESULT SpamAgentTypes scoreType() const
    {
        return mType;
    }

    Q_REQUIRED_RESULT QByteArray header() const
    {
        return mField;
    }

    Q_REQUIRED_RESULT QByteArray confidenceHeader() const
    {
        return mConfidenceField;
    }

    Q_REQUIRED_RESULT QRegExp scorePattern() const
    {
        return mScore;
    }

    Q_REQUIRED_RESULT QRegExp thresholdPattern() const
    {
        return mThreshold;
    }

    Q_REQUIRED_RESULT QRegExp confidencePattern() const
    {
        return mConfidence;
    }

private:
    QString mName;
    SpamAgentTypes mType;
    QByteArray mField;
    QByteArray mConfidenceField;
    QRegExp mScore;
    QRegExp mThreshold;
    QRegExp mConfidence;
};
using SpamAgents = QVector<SpamAgent>;

class AntiSpamConfigSingletonProvider;

/**
    @short Singleton to manage loading the kmail.antispamrc file.
    @author Patrick Audley <paudley@blackcat.ca>

    Use of this config-manager class is straight forward.  Since it
    is a singleton object, all you have to do is obtain an instance
    by calling @p SpamConfig::instance() and use any of the
    public member functions.
  */
class AntiSpamConfig
{
    friend class AntiSpamConfigSingletonProvider;

private:
    AntiSpamConfig();

public:
    ~AntiSpamConfig();

    static AntiSpamConfig *instance();

    /**
     * Returns a list of all agents found on the system. This
     * might list SA twice, if both the C and the Perl version are present.
     */
    const SpamAgents agents() const;

    /**
     * Returns a list of unique agents, found on the system. SpamAssassin will
     * only be listed once, even if both the C and the Perl version are
     * installed.
     */
    const SpamAgents uniqueAgents() const;

private:
    SpamAgents mAgents;

    void readConfig();
};
}

