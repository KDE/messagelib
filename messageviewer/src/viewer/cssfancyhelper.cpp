/*
  Copyright (c) 2018 Montel Laurent <montel@kde.org>

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

#include "cssfancyhelper.h"
#include <QApplication>
#include <QPalette>

using namespace MessageViewer;
namespace {
int pointsToPixel(const QPaintDevice *pd, int pointSize)
{
    return (pointSize * pd->logicalDpiY() + 36) / 72;
}
}
CSSFancyHelper::CSSFancyHelper(const QPaintDevice *pd)
    : CSSHelper(pd)
{

}

CSSFancyHelper::~CSSFancyHelper()
{

}

QString CSSFancyHelper::extraScreenCss() const
{
    const QPalette &pal = QApplication::palette();
    const QString val = QStringLiteral("div.fancy.header > div {\n"
                          "  background-color: %1 ! important;\n"
                          "  color: %2 ! important;\n"
                          "  border: solid %3 1px ! important;\n"
                          "  line-height: normal;\n"
                          "}\n\n"

                          "div.fancy.header > div a[href] { color: %2 ! important; }\n\n"

                          "div.fancy.header > div a[href]:hover { text-decoration: underline ! important; }\n\n"

                          "div.fancy.header > div.spamheader {\n"
                          "  background-color: #cdcdcd ! important;\n"
                          "  border-top: 0px ! important;\n"
                          "  padding: 3px ! important;\n"
                          "  color: black ! important;\n"
                          "  font-weight: bold ! important;\n"
                          "  font-size: smaller ! important;\n"
                          "}\n\n"

                          "div.fancy.header > table.outer {\n"
                          "  all: inherit;\n"
                          "  width: auto ! important;\n"
                          "  border-spacing: 0;\n"
                          "  background-color: %4 ! important;\n"
                          "  color: %3 ! important;\n"
                          "  border-bottom: solid %3 1px ! important;\n"
                          "  border-left: solid %3 1px ! important;\n"
                          "  border-right: solid %3 1px ! important;\n"
                          "}\n\n"
                          ).arg(pal.color(QPalette::Highlight).name(),
                                pal.color(QPalette::HighlightedText).name(),
                                pal.color(QPalette::Foreground).name(),
                                pal.color(QPalette::Background).name());
    return val;
}

QString CSSFancyHelper::extraPrintCss() const
{
    const QPalette &pal = QApplication::palette();
    const QString val = QStringLiteral("div.fancy.header > div {\n"
                          "  background-color: %1 ! important;\n"
                          "  color: %2 ! important;\n"
                          "  padding: 4px ! important;\n"
                          "  border: solid %2 1px ! important;\n"
                          "  line-height: normal;\n"
                          "}\n\n"

                          "div.fancy.header > div a[href] { color: %2 ! important; }\n\n"

                          "div.fancy.header > table.outer{\n"
                          "  all: inherit;\n"
                          "  width: auto ! important;\n"
                          "  border-spacing: 0;\n"
                          "  background-color: %1 ! important;\n"
                          "  color: %2 ! important;\n"
                          "  border-bottom: solid %2 1px ! important;\n"
                          "  border-left: solid %2 1px ! important;\n"
                          "  border-right: solid %2 1px ! important;\n"
                          "}\n\n"
                          ).arg(pal.color(QPalette::Background).name(),
                                pal.color(QPalette::Foreground).name());
    return val;
}

QString CSSFancyHelper::extraCommonCss() const
{
    const QPalette &pal = QApplication::palette();
    const QString headerFont = QStringLiteral("font-family: \"%1\" ! important;\n"
                                              "  font-size: %2px ! important;\n")
                               .arg(mBodyFont.family())
                               .arg(pointsToPixel(this->mPaintDevice, mBodyFont.pointSize()));
    const QString val = QStringLiteral(
                "div.fancy.header table {\n"
                "  width: 100% ! important;\n"
                "  border-width: 0px ! important;\n"
                "  line-height: normal;\n"
                "}\n\n"

                "div.fancy.header > div {\n"
                "  font-weight: bold ! important;\n"
                "  padding: 4px ! important;\n"
                "  line-height: normal;\n"
                "}\n\n"

                "div.fancy.header table {\n"
                "  padding: 2px ! important;\n"
                "  text-align: left ! important;\n"
                "  border-collapse: separate ! important;\n"
                "}\n\n"

                "div.fancy.header table th {\n"
                "  %3\n"
                "  padding: 0px ! important;\n"
                "  white-space: nowrap ! important;\n"
                "  border-spacing: 0px ! important;\n"
                "  text-align: left ! important;\n"
                "  vertical-align: top ! important;\n"
                "  background-color: %1 ! important;\n"
                "  color: %2 ! important;\n"
                "  border: 1px ! important;\n"

                "}\n\n"

                "div.fancy.header table td {\n"
                "  %3\n"
                "  padding: 0px ! important;\n"
                "  border-spacing: 0px ! important;\n"
                "  text-align: left ! important;\n"
                "  vertical-align: top ! important;\n"
                "  width: 100% ! important;\n"
                "  background-color: %1 ! important;\n"
                "  color: %2 ! important;\n"
                "  border: 1px ! important;\n"
                "}\n\n"

                "div.fancy.header table a:hover {\n"
                "  background-color: transparent ! important;\n"
                "}\n\n")
            .arg(pal.color(QPalette::Background).name())
            .arg(pal.color(QPalette::Foreground).name())
            .arg(headerFont);
    return val;
}
