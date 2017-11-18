/*
   Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
   License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#include "quotehtml.h"

#include "utils/iconnamecache.h"
#include "viewer/csshelperbase.h"

#include <MessageViewer/MessagePartRendererBase>

#include <KTextToHTML>

#include <QSharedPointer>

/** Check if the newline at position @p newLinePos in string @p s
    seems to separate two paragraphs (important for correct BiDi
    behavior, but is heuristic because paragraphs are not
    well-defined) */
// Guesstimate if the newline at newLinePos actually separates paragraphs in the text s
// We use several heuristics:
// 1. If newLinePos points after or before (=at the very beginning of) text, it is not between paragraphs
// 2. If the previous line was longer than the wrap size, we want to consider it a paragraph on its own
//    (some clients, notably Outlook, send each para as a line in the plain-text version).
// 3. Otherwise, we check if the newline could have been inserted for wrapping around; if this
//    was the case, then the previous line will be shorter than the wrap size (which we already
//    know because of item 2 above), but adding the first word from the next line will make it
//    longer than the wrap size.
bool looksLikeParaBreak(const QString &s, unsigned int newLinePos)
{
    const unsigned int WRAP_COL = 78;

    unsigned int length = s.length();
    // 1. Is newLinePos at an end of the text?
    if (newLinePos >= length - 1 || newLinePos == 0) {
        return false;
    }

    // 2. Is the previous line really a paragraph -- longer than the wrap size?

    // First char of prev line -- works also for first line
    unsigned prevStart = s.lastIndexOf(QLatin1Char('\n'), newLinePos - 1) + 1;
    unsigned prevLineLength = newLinePos - prevStart;
    if (prevLineLength > WRAP_COL) {
        return true;
    }

    // find next line to delimit search for first word
    unsigned int nextStart = newLinePos + 1;
    int nextEnd = s.indexOf(QLatin1Char('\n'), nextStart);
    if (nextEnd == -1) {
        nextEnd = length;
    }
    QString nextLine = s.mid(nextStart, nextEnd - nextStart);
    length = nextLine.length();
    // search for first word in next line
    unsigned int wordStart;
    bool found = false;
    for (wordStart = 0; !found && wordStart < length; wordStart++) {
        switch (nextLine[wordStart].toLatin1()) {
        case '>':
        case '|':
        case ' ':  // spaces, tabs and quote markers don't count
        case '\t':
        case '\r':
            break;
        default:
            found = true;
            break;
        }
    } /* for() */

    if (!found) {
        // next line is essentially empty, it seems -- empty lines are
        // para separators
        return true;
    }
    //Find end of first word.
    //Note: flowText (in kmmessage.cpp) separates words for wrap by
    //spaces only. This should be consistent, which calls for some
    //refactoring.
    int wordEnd = nextLine.indexOf(QLatin1Char(' '), wordStart);
    if (wordEnd == (-1)) {
        wordEnd = length;
    }
    int wordLength = wordEnd - wordStart;

    // 3. If adding a space and the first word to the prev line don't
    //    make it reach the wrap column, then the break was probably
    //    meaningful
    return prevLineLength + wordLength + 1 < WRAP_COL;
}

QString quotedHTML(const QString &s, MessageViewer::RenderContext *context, MessageViewer::CSSHelperBase *cssHelper)
{
    Q_ASSERT(cssHelper);

    KTextToHTML::Options convertFlags = KTextToHTML::PreserveSpaces | KTextToHTML::HighlightText;
    if (context->showEmoticons()) {
        convertFlags |= KTextToHTML::ReplaceSmileys;
    }
    QString htmlStr;
    const QString normalStartTag = cssHelper->nonQuotedFontTag();
    QString quoteFontTag[3];
    QString deepQuoteFontTag[3];
    for (int i = 0; i < 3; ++i) {
        quoteFontTag[i] = cssHelper->quoteFontTag(i);
        deepQuoteFontTag[i] = cssHelper->quoteFontTag(i + 3);
    }
    const QString normalEndTag = QStringLiteral("</div>");
    const QString quoteEnd = QStringLiteral("</div>");

    const unsigned int length = s.length();
    bool paraIsRTL = false;
    bool startNewPara = true;
    unsigned int pos, beg;

    // skip leading empty lines
    for (pos = 0; pos < length && s[pos] <= QLatin1Char(' '); ++pos) {
    }
    while (pos > 0 && (s[pos - 1] == QLatin1Char(' ') || s[pos - 1] == QLatin1Char('\t'))) {
        pos--;
    }
    beg = pos;

    int currQuoteLevel = -2; // -2 == no previous lines
    bool curHidden = false; // no hide any block

    QString collapseIconPath;
    QString expandIconPath;
    if (context->showExpandQuotesMark()) {
        collapseIconPath = MessageViewer::IconNameCache::instance()->iconPathFromLocal(QStringLiteral(
                                                                                           "quotecollapse.png"));
        expandIconPath
            = MessageViewer::IconNameCache::instance()->iconPathFromLocal(QStringLiteral(
                                                                              "quoteexpand.png"));
    }

    int previousQuoteDepth = -1;
    while (beg < length) {
        /* search next occurrence of '\n' */
        pos = s.indexOf(QLatin1Char('\n'), beg, Qt::CaseInsensitive);
        if (pos == (unsigned int)(-1)) {
            pos = length;
        }

        QString line(s.mid(beg, pos - beg));
        beg = pos + 1;

        bool foundQuote = false;
        /* calculate line's current quoting depth */
        int actQuoteLevel = -1;
        const int numberOfCaracters(line.length());
        int quoteLength = 0;
        for (int p = 0; p < numberOfCaracters; ++p) {
            switch (line[p].toLatin1()) {
            case '>':
            case '|':
                actQuoteLevel++;
                quoteLength = p;
                foundQuote = true;
                break;
            case ' ':  // spaces and tabs are allowed between the quote markers
            case '\t':
            case '\r':
                quoteLength = p;
                break;
            default:  // stop quoting depth calculation
                p = numberOfCaracters;
                break;
            }
        } /* for() */
        if (!foundQuote) {
            quoteLength = 0;
        }
        bool actHidden = false;

        // This quoted line needs be hidden
        if (context->showExpandQuotesMark() && context->levelQuote() >= 0
            && context->levelQuote() <= actQuoteLevel) {
            actHidden = true;
        }

        if (actQuoteLevel != currQuoteLevel) {
            /* finish last quotelevel */
            if (currQuoteLevel == -1) {
                htmlStr.append(normalEndTag);
            } else if (currQuoteLevel >= 0 && !curHidden) {
                htmlStr.append(quoteEnd);
            }
            //Close blockquote
            if (previousQuoteDepth > actQuoteLevel) {
                htmlStr += cssHelper->addEndBlockQuote(previousQuoteDepth - actQuoteLevel);
            }

            /* start new quotelevel */
            if (actQuoteLevel == -1) {
                htmlStr += normalStartTag;
            } else {
                if (context->showExpandQuotesMark()) {
                    // Add blockquote
                    if (previousQuoteDepth < actQuoteLevel) {
                        htmlStr
                            += cssHelper->addStartBlockQuote(actQuoteLevel - previousQuoteDepth);
                    }
                    if (actHidden) {
                        //only show the QuoteMark when is the first line of the level hidden
                        if (!curHidden) {
                            //Expand all quotes
                            htmlStr += QLatin1String("<div class=\"quotelevelmark\" >");
                            htmlStr += QStringLiteral("<a href=\"kmail:levelquote?%1 \">"
                                                      "<img src=\"%2\"/></a>")
                                       .arg(-1)
                                       .arg(expandIconPath);
                            htmlStr += QLatin1String("</div><br/>");
                        }
                    } else {
                        htmlStr += QLatin1String("<div class=\"quotelevelmark\" >");
                        htmlStr += QStringLiteral("<a href=\"kmail:levelquote?%1 \">"
                                                  "<img src=\"%2\"/></a>")
                                   .arg(actQuoteLevel)
                                   .arg(collapseIconPath);
                        htmlStr += QLatin1String("</div>");
                        if (actQuoteLevel < 3) {
                            htmlStr += quoteFontTag[actQuoteLevel];
                        } else {
                            htmlStr += deepQuoteFontTag[actQuoteLevel % 3];
                        }
                    }
                } else {
                    // Add blockquote
                    if (previousQuoteDepth < actQuoteLevel) {
                        htmlStr
                            += cssHelper->addStartBlockQuote(actQuoteLevel - previousQuoteDepth);
                    }

                    if (actQuoteLevel < 3) {
                        htmlStr += quoteFontTag[actQuoteLevel];
                    } else {
                        htmlStr += deepQuoteFontTag[actQuoteLevel % 3];
                    }
                }
            }
            currQuoteLevel = actQuoteLevel;
        }
        curHidden = actHidden;

        if (!actHidden) {
            // don't write empty <div ...></div> blocks (they have zero height)
            // ignore ^M DOS linebreaks
            if (!line.remove(QLatin1Char('\015')).isEmpty()) {
                if (startNewPara) {
                    paraIsRTL = line.isRightToLeft();
                }
                htmlStr += QStringLiteral("<div dir=\"%1\">").arg(paraIsRTL ? QStringLiteral(
                                                                      "rtl") : QStringLiteral("ltr"));
                // if quoteLengh == 0 && foundQuote => a simple quote
                if (foundQuote) {
                    quoteLength++;
                    const int rightString = (line.length()) - quoteLength;
                    if (rightString > 0) {
                        htmlStr += QStringLiteral("<span class=\"quotemarks\">%1</span>").arg(line.left(
                                                                                                  quoteLength));
                        htmlStr += QStringLiteral("<font color=\"%1\">").arg(cssHelper->quoteColorName(
                                                                                 actQuoteLevel))
                                   + KTextToHTML::convertToHtml(line.right(
                                                                    rightString),
                                                                convertFlags) + QStringLiteral(
                            "</font>");
                    } else {
                        htmlStr
                            += QStringLiteral("<span class=\"quotemarksemptyline\">%1</span>").arg(line.left(
                                                                                                       quoteLength));
                    }
                } else {
                    htmlStr += KTextToHTML::convertToHtml(line, convertFlags);
                }

                htmlStr += QLatin1String("</div>");
                startNewPara = looksLikeParaBreak(s, pos);
            } else {
                htmlStr += QLatin1String("<br/>");
                // after an empty line, always start a new paragraph
                startNewPara = true;
            }
        }
        previousQuoteDepth = actQuoteLevel;
    } /* while() */

    /* really finish the last quotelevel */
    if (currQuoteLevel == -1) {
        htmlStr.append(normalEndTag);
    } else {
        htmlStr += quoteEnd + cssHelper->addEndBlockQuote(currQuoteLevel + 1);
    }

    // qCDebug(MESSAGEVIEWER_LOG) << "========================================\n"
    //                            << htmlStr
    //                            << "\n======================================\n";
    return htmlStr;
}
