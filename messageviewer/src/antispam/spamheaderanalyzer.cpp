/*
    spamheaderanalyzer.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Patrick Audley <paudley@blackcat.ca>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "spamheaderanalyzer.h"
#include "antispamconfig.h"
#include "messageviewer_debug.h"

#include <KMime/Headers>
#include <KMime/Message>

#include <QRegularExpression>

using namespace MessageViewer;

// static
SpamScores SpamHeaderAnalyzer::getSpamScores(KMime::Message *message)
{
    SpamScores scores;
    const SpamAgents agents = AntiSpamConfig::instance()->uniqueAgents();
    SpamAgents::const_iterator end(agents.constEnd());
    for (SpamAgents::const_iterator it = agents.constBegin(); it != end; ++it) {
        float score = -2.0;

        SpamError spamError = noError;

        // Skip bogus agents
        if ((*it).scoreType() == SpamAgentNone) {
            continue;
        }

        // Do we have the needed score field for this agent?
        KMime::Headers::Base *header = message->headerByType((*it).header().constData());
        if (!header) {
            continue;
        }

        const QString mField = header->asUnicodeString();

        if (mField.isEmpty()) {
            continue;
        }

        QString scoreString;
        bool scoreValid = false;

        if ((*it).scoreType() != SpamAgentBool) {
            // Can we extract the score?
            QRegularExpression scorePattern = (*it).scorePattern();
            if (scorePattern.match(mField).hasMatch()) {
                scoreString = scorePattern.match(mField).captured(1);
                scoreValid = true;
            }
        } else {
            scoreValid = true;
        }

        if (!scoreValid) {
            spamError = couldNotFindTheScoreField;
            qCDebug(MESSAGEVIEWER_LOG) << "Score could not be extracted from header '" << mField << "'";
        } else {
            bool floatValid = false;
            switch ((*it).scoreType()) {
            case SpamAgentNone:
                spamError = errorExtractingAgentString;
                break;

            case SpamAgentBool:
                if ((*it).scorePattern().match(mField).hasMatch()) {
                    score = 0.0;
                } else {
                    score = 100.0;
                }
                break;

            case SpamAgentFloat:
                score = scoreString.toFloat(&floatValid);
                if (!floatValid) {
                    spamError = couldNotConverScoreToFloat;
                    qCDebug(MESSAGEVIEWER_LOG) << "Score (" << scoreString << ") is no number";
                } else {
                    score *= 100.0;
                }
                break;

            case SpamAgentFloatLarge:
                score = scoreString.toFloat(&floatValid);
                if (!floatValid) {
                    spamError = couldNotConverScoreToFloat;
                    qCDebug(MESSAGEVIEWER_LOG) << "Score (" << scoreString << ") is no number";
                }
                break;

            case SpamAgentAdjustedFloat:
                score = scoreString.toFloat(&floatValid);
                if (!floatValid) {
                    spamError = couldNotConverScoreToFloat;
                    qCDebug(MESSAGEVIEWER_LOG) << "Score (" << scoreString << ") is no number";
                    break;
                }

                // Find the threshold value.
                QString thresholdString;
                const QRegularExpression thresholdPattern = (*it).thresholdPattern();
                if (thresholdPattern.match(mField).hasMatch()) {
                    thresholdString = thresholdPattern.match(mField).captured(1);
                } else {
                    spamError = couldNotFindTheThresholdField;
                    qCDebug(MESSAGEVIEWER_LOG) << "Threshold could not be extracted from header '" << mField << "'";
                    break;
                }
                const float threshold = thresholdString.toFloat(&floatValid);
                if (!floatValid || (threshold <= 0.0)) {
                    spamError = couldNotConvertThresholdToFloatOrThresholdIsNegative;
                    qCDebug(MESSAGEVIEWER_LOG) << "Threshold (" << thresholdString << ") is no"
                                               << "number or is negative";
                    break;
                }

                // Normalize the score. Anything below 0 means 0%, anything above
                // threshold mean 100%. Values between 0 and threshold are mapped
                // linearly to 0% - 100%.
                if (score < 0.0) {
                    score = 0.0;
                } else if (score > threshold) {
                    score = 100.0;
                } else {
                    score = score / threshold * 100.0;
                }
                break;
            }
        }
        // Find the confidence
        float confidence = -2.0;
        QString confidenceString = QStringLiteral("-2.0");
        bool confidenceValid = false;
        // Do we have the needed confidence field for this agent?
        const QByteArray confidenceHeaderName = (*it).confidenceHeader();
        QString mCField;
        if (!confidenceHeaderName.isEmpty()) {
            KMime::Headers::Base *cHeader = message->headerByType(confidenceHeaderName.constData());
            if (cHeader) {
                mCField = cHeader->asUnicodeString();
                if (!mCField.isEmpty()) {
                    // Can we extract the confidence?
                    QRegularExpression cScorePattern = (*it).confidencePattern();
                    if (cScorePattern.match(mCField).hasMatch()) {
                        confidenceString = cScorePattern.match(mCField).captured(1);
                    }
                    confidence = confidenceString.toFloat(&confidenceValid);
                    if (!confidenceValid) {
                        spamError = couldNotConvertConfidenceToFloat;
                        qCDebug(MESSAGEVIEWER_LOG) << "Unable to convert confidence to float:" << confidenceString;
                    }
                }
            }
        }
        scores.append(SpamScore((*it).name(), spamError, score, confidence * 100, mField, mCField));
    }

    return scores;
}
