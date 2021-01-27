/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "quotehtml.h"

#include "utils/iconnamecache.h"
#include "viewer/csshelperbase.h"

#include <MessageViewer/HtmlWriter>
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
bool looksLikeParaBreak(const QString &s, int newLinePos)
{
    const int WRAP_COL = 78;

    int length = s.length();
    // 1. Is newLinePos at an end of the text?
    if (newLinePos >= length - 1 || newLinePos == 0) {
        return false;
    }

    // 2. Is the previous line really a paragraph -- longer than the wrap size?

    // First char of prev line -- works also for first line
    int prevStart = s.lastIndexOf(QLatin1Char('\n'), newLinePos - 1) + 1;
    int prevLineLength = newLinePos - prevStart;
    if (prevLineLength > WRAP_COL) {
        return true;
    }

    // find next line to delimit search for first word
    int nextStart = newLinePos + 1;
    int nextEnd = s.indexOf(QLatin1Char('\n'), nextStart);
    if (nextEnd == -1) {
        nextEnd = length;
    }
    QString nextLine = s.mid(nextStart, nextEnd - nextStart);
    length = nextLine.length();
    // search for first word in next line
    int wordStart;
    bool found = false;
    for (wordStart = 0; !found && wordStart < length; wordStart++) {
        switch (nextLine[wordStart].toLatin1()) {
        case '>':
        case '|':
        case ' ': // spaces, tabs and quote markers don't count
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
    // Find end of first word.
    // Note: flowText (in kmmessage.cpp) separates words for wrap by
    // spaces only. This should be consistent, which calls for some
    // refactoring.
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

void quotedHTML(const QString &s, MessageViewer::RenderContext *context, MessageViewer::HtmlWriter *htmlWriter)
{
    const auto cssHelper = context->cssHelper();
    Q_ASSERT(cssHelper);

    KTextToHTML::Options convertFlags = KTextToHTML::PreserveSpaces | KTextToHTML::HighlightText | KTextToHTML::ConvertPhoneNumbers;
    if (context->showEmoticons()) {
        convertFlags |= KTextToHTML::ReplaceSmileys;
    }

    const QString normalStartTag = cssHelper->nonQuotedFontTag();
    QString quoteFontTag[3];
    QString deepQuoteFontTag[3];
    for (int i = 0; i < 3; ++i) {
        quoteFontTag[i] = cssHelper->quoteFontTag(i);
        deepQuoteFontTag[i] = cssHelper->quoteFontTag(i + 3);
    }
    const QString normalEndTag = QStringLiteral("</div>");
    const QString quoteEnd = QStringLiteral("</div>");

    const int length = s.length();
    bool paraIsRTL = false;
    bool startNewPara = true;
    int pos, beg;

    // skip leading empty lines
    for (pos = 0; pos < length && s[pos] <= QLatin1Char(' '); ++pos) { }
    while (pos > 0 && (s[pos - 1] == QLatin1Char(' ') || s[pos - 1] == QLatin1Char('\t'))) {
        pos--;
    }
    beg = pos;

    int currQuoteLevel = -2; // -2 == no previous lines
    bool curHidden = false; // no hide any block

    QString collapseIconPath;
    QString expandIconPath;
    if (context->showExpandQuotesMark()) {
        collapseIconPath = MessageViewer::IconNameCache::instance()->iconPathFromLocal(QStringLiteral("quotecollapse.png"));
        expandIconPath = MessageViewer::IconNameCache::instance()->iconPathFromLocal(QStringLiteral("quoteexpand.png"));
    }

    int previousQuoteDepth = -1;
    while (beg < length) {
        /* search next occurrence of '\n' */
        pos = s.indexOf(QLatin1Char('\n'), beg, Qt::CaseInsensitive);
        if (pos == -1) {
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
                if (p == 0 || foundQuote) {
                    actQuoteLevel++;
                    quoteLength = p;
                    foundQuote = true;
                }
                break;
            case ' ': // spaces and tabs are allowed between the quote markers
            case '\t':
            case '\r':
                quoteLength = p;
                break;
            default: // stop quoting depth calculation
                p = numberOfCaracters;
                break;
            }
        } /* for() */
        if (!foundQuote) {
            quoteLength = 0;
        }
        bool actHidden = false;

        // This quoted line needs be hidden
        if (context->showExpandQuotesMark() && context->levelQuote() >= 0 && context->levelQuote() <= actQuoteLevel) {
            actHidden = true;
        }

        if (actQuoteLevel != currQuoteLevel) {
            /* finish last quotelevel */
            if (currQuoteLevel == -1) {
                htmlWriter->write(normalEndTag);
            } else if (currQuoteLevel >= 0 && !curHidden) {
                htmlWriter->write(quoteEnd);
            }
            // Close blockquote
            if (previousQuoteDepth > actQuoteLevel) {
                htmlWriter->write(cssHelper->addEndBlockQuote(previousQuoteDepth - actQuoteLevel));
            }

            /* start new quotelevel */
            if (actQuoteLevel == -1) {
                htmlWriter->write(normalStartTag);
            } else {
                if (context->showExpandQuotesMark()) {
                    // Add blockquote
                    if (previousQuoteDepth < actQuoteLevel) {
                        htmlWriter->write(cssHelper->addStartBlockQuote(actQuoteLevel - previousQuoteDepth));
                    }
                    if (actHidden) {
                        // only show the QuoteMark when is the first line of the level hidden
                        if (!curHidden) {
                            // Expand all quotes
                            htmlWriter->write(QStringLiteral("<div class=\"quotelevelmark\" >"));
                            htmlWriter->write(QStringLiteral("<a href=\"kmail:levelquote?%1 \">"
                                                             "<img src=\"%2\"/></a>")
                                                  .arg(-1)
                                                  .arg(expandIconPath));
                            htmlWriter->write(QStringLiteral("</div><br/>"));
                        }
                    } else {
                        htmlWriter->write(QStringLiteral("<div class=\"quotelevelmark\" >"));
                        htmlWriter->write(QStringLiteral("<a href=\"kmail:levelquote?%1 \">"
                                                         "<img src=\"%2\"/></a>")
                                              .arg(actQuoteLevel)
                                              .arg(collapseIconPath));
                        htmlWriter->write(QStringLiteral("</div>"));
                        if (actQuoteLevel < 3) {
                            htmlWriter->write(quoteFontTag[actQuoteLevel]);
                        } else {
                            htmlWriter->write(deepQuoteFontTag[actQuoteLevel % 3]);
                        }
                    }
                } else {
                    // Add blockquote
                    if (previousQuoteDepth < actQuoteLevel) {
                        htmlWriter->write(cssHelper->addStartBlockQuote(actQuoteLevel - previousQuoteDepth));
                    }

                    if (actQuoteLevel < 3) {
                        htmlWriter->write(quoteFontTag[actQuoteLevel]);
                    } else {
                        htmlWriter->write(deepQuoteFontTag[actQuoteLevel % 3]);
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
                htmlWriter->write(QStringLiteral("<div dir=\"%1\">").arg(paraIsRTL ? QStringLiteral("rtl") : QStringLiteral("ltr")));
                // if quoteLengh == 0 && foundQuote => a simple quote
                if (foundQuote) {
                    quoteLength++;
                    const int rightString = (line.length()) - quoteLength;
                    if (rightString > 0) {
                        htmlWriter->write(QStringLiteral("<span class=\"quotemarks\">%1</span>").arg(line.left(quoteLength)));
                        htmlWriter->write(QStringLiteral("<font color=\"%1\">").arg(cssHelper->quoteColorName(actQuoteLevel)));
                        htmlWriter->write(KTextToHTML::convertToHtml(line.right(rightString), convertFlags, 4096, 512));
                        htmlWriter->write(QStringLiteral("</font>"));
                    } else {
                        htmlWriter->write(QStringLiteral("<span class=\"quotemarksemptyline\">%1</span>").arg(line.left(quoteLength)));
                    }
                } else {
                    htmlWriter->write(KTextToHTML::convertToHtml(line, convertFlags, 4096, 512));
                }

                htmlWriter->write(QStringLiteral("</div>"));
                startNewPara = looksLikeParaBreak(s, pos);
            } else {
                htmlWriter->write(QStringLiteral("<br/>"));
                // after an empty line, always start a new paragraph
                startNewPara = true;
            }
        }
        previousQuoteDepth = actQuoteLevel;
    } /* while() */

    /* really finish the last quotelevel */
    if (currQuoteLevel == -1) {
        htmlWriter->write(normalEndTag);
    } else {
        htmlWriter->write(quoteEnd + cssHelper->addEndBlockQuote(currQuoteLevel + 1));
    }
}
