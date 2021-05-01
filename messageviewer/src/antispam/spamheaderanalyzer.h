/*
    spamheaderanalyzer.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Patrick Audley <paudley@blackcat.ca>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KMime/Message>

#include <QString>
#include <QVector>

namespace MessageViewer
{
enum SpamError {
    noError,
    uninitializedStructUsed,
    errorExtractingAgentString,
    couldNotConverScoreToFloat,
    couldNotConvertThresholdToFloatOrThresholdIsNegative,
    couldNotFindTheScoreField,
    couldNotFindTheThresholdField,
    couldNotConvertConfidenceToFloat
};

/**
    @short A simple tuple of error, agent, score, confidence and header.

    The score returned is positive if no error has occurred.
    error values indicate the following errors:
      noError                                               Spam Headers successfully parsed
      uninitializedStructUsed                               Unintialized struct used
      errorExtractingAgentString                            Error extracting agent string
      couldNotConverScoreToFloat                            Couldn't convert score to float
      couldNotConvertThresholdToFloatOrThresholdIsNegative  Couldn't convert threshold to float or threshold is negative
      couldNotFindTheScoreField                             Couldn't find the score field
      couldNotFindTheThresholdField                         Couldn't find the threshold field
      couldNotConvertConfidenceToFloat                      Couldn't convert confidence to float
*/
class SpamScore
{
public:
    SpamScore()
        : mError(noError)
        , mScore(-2.0)
        , mConfidence(-2.0)
    {
    }

    SpamScore(const QString &agent, SpamError error, float score, float confidence, const QString &header, const QString &cheader)
        : mAgent(agent)
        , mError(error)
        , mScore(score)
        , mConfidence(confidence)
        , mHeader(header)
        , mConfidenceHeader(cheader)
    {
    }

    Q_REQUIRED_RESULT QString agent() const
    {
        return mAgent;
    }

    Q_REQUIRED_RESULT float score() const
    {
        return mScore;
    }

    Q_REQUIRED_RESULT float confidence() const
    {
        return mConfidence;
    }

    Q_REQUIRED_RESULT SpamError error() const
    {
        return mError;
    }

    Q_REQUIRED_RESULT QString spamHeader() const
    {
        return mHeader;
    }

    Q_REQUIRED_RESULT QString confidenceHeader() const
    {
        return mConfidenceHeader;
    }

private:
    QString mAgent;
    SpamError mError;
    float mScore;
    float mConfidence;
    QString mHeader;
    QString mConfidenceHeader;
};
using SpamScores = QVector<SpamScore>;

/**
    @short Flyweight for analysing spam headers.
    @author Patrick Audley <paudley@blackcat.ca>
  */
class SpamHeaderAnalyzer
{
public:
    /**
      @short Extract scores from known anti-spam headers
      @param message A KMime::Message to examine
      @return A list of detected scores. See SpamScore
    */
    static SpamScores getSpamScores(KMime::Message *message);
};
}

