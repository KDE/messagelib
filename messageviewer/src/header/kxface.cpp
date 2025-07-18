/*
  This file is part of libkdepim.

  Original compface:
  Copyright (c) James Ashton - Sydney University - June 1990. //krazy:exclude=copyright

  Additions for KDE:
  SPDX-FileCopyrightText: 2004 Jakob Schröter <js@camaya.net>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kxface.h"
using namespace Qt::Literals::StringLiterals;

#include <QBuffer>
#include <QImage>
#include <QRegularExpression>
#include <QString>

#include <cstdlib>
#include <cstring>

#define GEN(g)                                                                                                                                                 \
    F[h] ^= G.g[k];                                                                                                                                            \
    break

#define BITSPERDIG 4
#define DIGITS (PIXELS / BITSPERDIG)
#define DIGSPERWORD 4
#define WORDSPERLINE (WIDTH / DIGSPERWORD / BITSPERDIG)

/* compressed output uses the full range of printable characters.
 * in ascii these are in a contiguous block so we just need to know
 * the first and last.  The total number of printables is needed too */
#define FIRSTPRINT '!'
#define LASTPRINT '~'
#define NUMPRINTS (LASTPRINT - FIRSTPRINT + 1)

/* output line length for compressed data */
static const int MAXLINELEN = 78;

/* Portable, very large unsigned integer arithmetic is needed.
 * Implementation uses arrays of WORDs.  COMPs must have at least
 * twice as many bits as WORDs to handle intermediate results */
#define COMP unsigned long
#define WORDCARRY (1 << BITSPERWORD)
#define WORDMASK (WORDCARRY - 1)

#define ERR_OK 0 /* successful completion */
#define ERR_EXCESS 1 /* completed OK but some input was ignored */
#define ERR_INSUFF -1 /* insufficient input.  Bad face format? */
#define ERR_INTERNAL -2 /* Arithmetic overflow or buffer overflow */

#define BLACK 0
#define GREY 1
#define WHITE 2

static const int MAX_XFACE_LENGTH = 2048;

using namespace MessageViewer;

KXFace::KXFace()
{
    NumProbs = 0;
}

KXFace::~KXFace() = default;

QString KXFace::fromImage(const QImage &image)
{
    if (image.isNull()) {
        return {};
    }

    QImage scaledImg = image.scaled(48, 48, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QByteArray ba;
    QBuffer buffer(&ba, this);
    buffer.open(QIODevice::WriteOnly);
    scaledImg.save(&buffer, "XBM");
    QString xbm(QString::fromLatin1(ba));
    xbm.remove(0, xbm.indexOf(QLatin1StringView("{")) + 1);
    xbm.truncate(xbm.indexOf(QLatin1StringView("}")));
    xbm.remove(u' ');
    xbm.remove(u',');
    xbm.remove(u"0x"_s);
    xbm.remove(u'\n');
    xbm.truncate(576);
    QString tmp = QLatin1StringView(xbm.toLatin1());
    int len = tmp.length();
    for (int i = 0; i < len; ++i) {
        switch (tmp[i].toLatin1()) {
        case '1':
            tmp[i] = u'8';
            break;
        case '2':
            tmp[i] = u'4';
            break;
        case '3':
            tmp[i] = u'c';
            break;
        case '4':
            tmp[i] = u'2';
            break;
        case '5':
            tmp[i] = u'a';
            break;
        case '7':
            tmp[i] = u'e';
            break;
        case '8':
            tmp[i] = u'1';
            break;
        case 'A':
        case 'a':
            tmp[i] = u'5';
            break;
        case 'B':
        case 'b':
            tmp[i] = u'd';
            break;
        case 'C':
        case 'c':
            tmp[i] = u'3';
            break;
        case 'D':
        case 'd':
            tmp[i] = u'b';
            break;
        case 'E':
        case 'e':
            tmp[i] = u'7';
            break;
        }
        if (i % 2) {
            QChar t = tmp[i];
            tmp[i] = tmp[i - 1];
            tmp[i - 1] = t;
        }
    }
    tmp.replace(QRegularExpression(u"(\\w{12})"_s), u"\\1\n"_s);
    tmp.replace(QRegularExpression(u"(\\w{4})"_s), u"0x\\1,"_s);
    len = tmp.length();
    char *fbuf = (char *)malloc(len + 1);
    strncpy(fbuf, tmp.toLatin1().constData(), len);
    fbuf[len] = '\0';
    if (!(status = setjmp(comp_env))) {
        ReadFace(fbuf);
        GenFace();
        CompAll(fbuf);
    }
    QString ret(QString::fromLatin1(fbuf));
    free(fbuf);

    return ret;
}

QImage KXFace::toImage(const QString &xface)
{
    if (xface.length() > MAX_XFACE_LENGTH) {
        return {};
    }

    char *fbuf = (char *)malloc(MAX_XFACE_LENGTH);
    memset(fbuf, '\0', MAX_XFACE_LENGTH);
    strncpy(fbuf, xface.toLatin1().constData(), xface.length());
    QByteArray img;
    if (!(status = setjmp(comp_env))) {
        UnCompAll(fbuf); /* compress otherwise */
        UnGenFace();
        img = WriteFace();
    }
    free(fbuf);
    QImage p;
    p.loadFromData(img, "XBM");

    return p;
}

//============================================================================
// more or less original compface 1.4 source

void KXFace::RevPush(const Prob *p)
{
    if (NumProbs >= PIXELS * 2 - 1) {
        longjmp(comp_env, ERR_INTERNAL);
    }
    ProbBuf[NumProbs++] = (Prob *)p;
}

void KXFace::BigPush(Prob *p)
{
    static unsigned char tmp;

    BigDiv(p->p_range, &tmp);
    BigMul(0);
    BigAdd(tmp + p->p_offset);
}

int KXFace::BigPop(const Prob *p)
{
    static unsigned char tmp;
    int i;

    BigDiv(0, &tmp);
    i = 0;
    while ((tmp < p->p_offset) || (tmp >= p->p_range + p->p_offset)) {
        p++;
        ++i;
    }
    BigMul(p->p_range);
    BigAdd(tmp - p->p_offset);
    return i;
}

/* Divide B by a storing the result in B and the remainder in the word
 * pointer to by r
 */
void KXFace::BigDiv(unsigned char a, unsigned char *r)
{
    int i;
    unsigned char *w;
    COMP c, d;

    a &= WORDMASK;
    if ((a == 1) || (B.b_words == 0)) {
        *r = 0;
        return;
    }
    if (a == 0) { /* treat this as a == WORDCARRY */
        /* and just shift everything right a WORD (unsigned char)*/
        i = --B.b_words;
        w = B.b_word;
        *r = *w;
        while (i--) {
            *w = *(w + 1);
            w++;
        }
        *w = 0;
        return;
    }
    w = B.b_word + (i = B.b_words);
    c = 0;
    while (i--) {
        c <<= BITSPERWORD;
        c += (COMP) * --w;
        d = c / (COMP)a;
        c = c % (COMP)a;
        *w = (unsigned char)(d & WORDMASK);
    }
    *r = c;
    if (B.b_word[B.b_words - 1] == 0) {
        B.b_words--;
    }
}

/* Multiply a by B storing the result in B
 */
void KXFace::BigMul(unsigned char a)
{
    int i;
    unsigned char *w;
    COMP c;

    a &= WORDMASK;
    if ((a == 1) || (B.b_words == 0)) {
        return;
    }
    if (a == 0) { /* treat this as a == WORDCARRY */
        /* and just shift everything left a WORD (unsigned char) */
        if ((i = B.b_words++) >= MAXWORDS - 1) {
            longjmp(comp_env, ERR_INTERNAL);
        }
        w = B.b_word + i;
        while (i--) {
            *w = *(w - 1);
            w--;
        }
        *w = 0;
        return;
    }
    i = B.b_words;
    w = B.b_word;
    c = 0;
    while (i--) {
        c += (COMP)*w * (COMP)a;
        *(w++) = (unsigned char)(c & WORDMASK);
        c >>= BITSPERWORD;
    }
    if (c) {
        if (B.b_words++ >= MAXWORDS) {
            longjmp(comp_env, ERR_INTERNAL);
        }
        *w = (COMP)(c & WORDMASK);
    }
}

/* Add to a to B storing the result in B
 */
void KXFace::BigAdd(unsigned char a)
{
    int i;
    unsigned char *w;
    COMP c;

    a &= WORDMASK;
    if (a == 0) {
        return;
    }
    i = 0;
    w = B.b_word;
    c = a;
    while ((i < B.b_words) && c) {
        c += (COMP)*w;
        *w++ = (unsigned char)(c & WORDMASK);
        c >>= BITSPERWORD;
        ++i;
    }
    if ((i == B.b_words) && c) {
        if (B.b_words++ >= MAXWORDS) {
            longjmp(comp_env, ERR_INTERNAL);
        }
        *w = (COMP)(c & WORDMASK);
    }
}

void KXFace::BigClear()
{
    B.b_words = 0;
}

QByteArray KXFace::WriteFace()
{
    char *s;
    int i;
    int j;
    int bits;
    int digits;
    int words;
    // int digsperword = DIGSPERWORD;
    // int wordsperline = WORDSPERLINE;
    QByteArray t("#define noname_width 48\n#define noname_height 48\nstatic char noname_bits[] = {\n ");
    j = t.length() - 1;

    s = F;
    bits = digits = words = i = 0;
    t.resize(MAX_XFACE_LENGTH);
    int digsperword = 2;
    int wordsperline = 15;
    while (s < F + PIXELS) {
        if ((bits == 0) && (digits == 0)) {
            t[j++] = '0';
            t[j++] = 'x';
        }
        if (*(s++)) {
            i = (i >> 1) | 0x8;
        } else {
            i >>= 1;
        }
        if (++bits == BITSPERDIG) {
            j++;
            t[j - ((digits & 1) * 2)] = *(i + HexDigits);
            bits = i = 0;
            if (++digits == digsperword) {
                if (s >= F + PIXELS) {
                    break;
                }
                t[j++] = ',';
                digits = 0;
                if (++words == wordsperline) {
                    t[j++] = '\n';
                    t[j++] = ' ';
                    words = 0;
                }
            }
        }
    }
    t.resize(j + 1);
    t += "};\n";
    return t;
}

void KXFace::UnCompAll(char *fbuf)
{
    char *p;

    BigClear();
    BigRead(fbuf);
    p = F;
    while (p < F + PIXELS) {
        *(p++) = 0;
    }
    UnCompress(F, 16, 16, 0);
    UnCompress(F + 16, 16, 16, 0);
    UnCompress(F + 32, 16, 16, 0);
    UnCompress(F + WIDTH * 16, 16, 16, 0);
    UnCompress(F + WIDTH * 16 + 16, 16, 16, 0);
    UnCompress(F + WIDTH * 16 + 32, 16, 16, 0);
    UnCompress(F + WIDTH * 32, 16, 16, 0);
    UnCompress(F + WIDTH * 32 + 16, 16, 16, 0);
    UnCompress(F + WIDTH * 32 + 32, 16, 16, 0);
}

void KXFace::UnCompress(char *f, int wid, int hei, int lev)
{
    switch (BigPop(&levels[lev][0])) {
    case WHITE:
        return;
    case BLACK:
        PopGreys(f, wid, hei);
        return;
    default:
        wid /= 2;
        hei /= 2;
        lev++;
        UnCompress(f, wid, hei, lev);
        UnCompress(f + wid, wid, hei, lev);
        UnCompress(f + hei * WIDTH, wid, hei, lev);
        UnCompress(f + wid + hei * WIDTH, wid, hei, lev);
        return;
    }
}

void KXFace::BigWrite(char *fbuf)
{
    static unsigned char tmp;
    static char buf[DIGITS];
    char *s;
    int i;

    s = buf;
    while (B.b_words > 0) {
        BigDiv(NUMPRINTS, &tmp);
        *(s++) = tmp + FIRSTPRINT;
    }
    i = 7; // leave room for the field name on the first line
    *(fbuf++) = ' ';
    while (s-- > buf) {
        if (i == 0) {
            *(fbuf++) = ' ';
        }
        *(fbuf++) = *s;
        if (++i >= MAXLINELEN) {
            *(fbuf++) = '\n';
            i = 0;
        }
    }
    if (i > 0) {
        *(fbuf++) = '\n';
    }
    *(fbuf++) = '\0';
}

void KXFace::BigRead(char *fbuf)
{
    int c;

    while (*fbuf != '\0') {
        c = *(fbuf++);
        if ((c < FIRSTPRINT) || (c > LASTPRINT)) {
            continue;
        }
        BigMul(NUMPRINTS);
        BigAdd((unsigned char)(c - FIRSTPRINT));
    }
}

void KXFace::ReadFace(char *fbuf)
{
    int c;
    int i;
    char *s;
    char *t;

    t = s = fbuf;
    for (i = strlen(s); i > 0; --i) {
        c = (int)*(s++);
        if ((c >= '0') && (c <= '9')) {
            if (t >= fbuf + DIGITS) {
                status = ERR_EXCESS;
                break;
            }
            *(t++) = c - '0';
        } else if ((c >= 'A') && (c <= 'F')) {
            if (t >= fbuf + DIGITS) {
                status = ERR_EXCESS;
                break;
            }
            *(t++) = c - 'A' + 10;
        } else if ((c >= 'a') && (c <= 'f')) {
            if (t >= fbuf + DIGITS) {
                status = ERR_EXCESS;
                break;
            }
            *(t++) = c - 'a' + 10;
        } else if (((c == 'x') || (c == 'X')) && (t > fbuf) && (*(t - 1) == 0)) {
            t--;
        }
    }
    if (t < fbuf + DIGITS) {
        longjmp(comp_env, ERR_INSUFF);
    }
    s = fbuf;
    t = F;
    c = 1 << (BITSPERDIG - 1);
    while (t < F + PIXELS) {
        *(t++) = (*s & c) ? 1 : 0;
        if ((c >>= 1) == 0) {
            s++;
            c = 1 << (BITSPERDIG - 1);
        }
    }
}

void KXFace::GenFace()
{
    static char newp[PIXELS];
    char *f1;
    char *f2;
    int i;

    f1 = newp;
    f2 = F;
    i = PIXELS;
    while (i-- > 0) {
        *(f1++) = *(f2++);
    }
    Gen(newp);
}

void KXFace::UnGenFace()
{
    Gen(F);
}

// static
void KXFace::Gen(char *f)
{
    int m;
    int l;
    int k;
    int j;
    int i;
    int h;

    for (j = 0; j < HEIGHT; ++j) {
        for (i = 0; i < WIDTH; ++i) {
            h = i + j * WIDTH;
            k = 0;
            for (l = i - 2; l <= i + 2; ++l) {
                for (m = j - 2; m <= j; ++m) {
                    if ((l >= i) && (m == j)) {
                        continue;
                    }
                    if ((l > 0) && (l <= WIDTH) && (m > 0)) {
                        k = *(f + l + m * WIDTH) ? k * 2 + 1 : k * 2;
                    }
                }
            }
            switch (i) {
            case 1:
                switch (j) {
                case 1:
                    GEN(g_22);
                case 2:
                    GEN(g_21);
                default:
                    GEN(g_20);
                }
                break;
            case 2:
                switch (j) {
                case 1:
                    GEN(g_12);
                case 2:
                    GEN(g_11);
                default:
                    GEN(g_10);
                }
                break;
            case WIDTH - 1:
                switch (j) {
                case 1:
                    GEN(g_42);
                case 2:
                    GEN(g_41);
                default:
                    GEN(g_40);
                }
                break;
            /* i runs from 0 to WIDTH-1, so case can never occur. I leave the code in
            because it appears exactly like this in the original compface code.
            case WIDTH :
            switch (j)
            {
            case 1 : GEN(g_32);
            case 2 : GEN(g_31);
            default : GEN(g_30);
            }
            break;
            */
            default:
                switch (j) {
                case 1:
                    GEN(g_02);
                case 2:
                    GEN(g_01);
                default:
                    GEN(g_00);
                }
                break;
            }
        }
    }
}

void KXFace::PopGreys(char *f, int wid, int hei)
{
    if (wid > 3) {
        wid /= 2;
        hei /= 2;
        PopGreys(f, wid, hei);
        PopGreys(f + wid, wid, hei);
        PopGreys(f + WIDTH * hei, wid, hei);
        PopGreys(f + WIDTH * hei + wid, wid, hei);
    } else {
        wid = BigPop(freqs);
        if (wid & 1) {
            *f = 1;
        }
        if (wid & 2) {
            *(f + 1) = 1;
        }
        if (wid & 4) {
            *(f + WIDTH) = 1;
        }
        if (wid & 8) {
            *(f + WIDTH + 1) = 1;
        }
    }
}

void KXFace::CompAll(char *fbuf)
{
    Compress(F, 16, 16, 0);
    Compress(F + 16, 16, 16, 0);
    Compress(F + 32, 16, 16, 0);
    Compress(F + WIDTH * 16, 16, 16, 0);
    Compress(F + WIDTH * 16 + 16, 16, 16, 0);
    Compress(F + WIDTH * 16 + 32, 16, 16, 0);
    Compress(F + WIDTH * 32, 16, 16, 0);
    Compress(F + WIDTH * 32 + 16, 16, 16, 0);
    Compress(F + WIDTH * 32 + 32, 16, 16, 0);
    BigClear();
    while (NumProbs > 0) {
        BigPush(ProbBuf[--NumProbs]);
    }
    BigWrite(fbuf);
}

void KXFace::Compress(char *f, int wid, int hei, int lev)
{
    if (AllWhite(f, wid, hei)) {
        RevPush(&levels[lev][WHITE]);
        return;
    }
    if (AllBlack(f, wid, hei)) {
        RevPush(&levels[lev][BLACK]);
        PushGreys(f, wid, hei);
        return;
    }
    RevPush(&levels[lev][GREY]);
    wid /= 2;
    hei /= 2;
    lev++;
    Compress(f, wid, hei, lev);
    Compress(f + wid, wid, hei, lev);
    Compress(f + hei * WIDTH, wid, hei, lev);
    Compress(f + wid + hei * WIDTH, wid, hei, lev);
}

int KXFace::AllWhite(char *f, int wid, int hei)
{
    return (*f == 0) && Same(f, wid, hei);
}

int KXFace::AllBlack(char *f, int wid, int hei)
{
    if (wid > 3) {
        wid /= 2;
        hei /= 2;
        return AllBlack(f, wid, hei) && AllBlack(f + wid, wid, hei) && AllBlack(f + WIDTH * hei, wid, hei) && AllBlack(f + WIDTH * hei + wid, wid, hei);
    } else {
        return *f || *(f + 1) || *(f + WIDTH) || *(f + WIDTH + 1);
    }
}

int KXFace::Same(char *f, int wid, int hei)
{
    char val;
    char *row;
    int x;

    val = *f;
    while (hei--) {
        row = f;
        x = wid;
        while (x--) {
            if (*(row++) != val) {
                return 0;
            }
        }
        f += WIDTH;
    }
    return 1;
}

void KXFace::PushGreys(char *f, int wid, int hei)
{
    if (wid > 3) {
        wid /= 2;
        hei /= 2;
        PushGreys(f, wid, hei);
        PushGreys(f + wid, wid, hei);
        PushGreys(f + WIDTH * hei, wid, hei);
        PushGreys(f + WIDTH * hei + wid, wid, hei);
    } else {
        RevPush(freqs + *f + 2 * *(f + 1) + 4 * *(f + WIDTH) + 8 * *(f + WIDTH + 1));
    }
}

#include "moc_kxface.cpp"
