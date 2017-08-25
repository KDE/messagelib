/*
   Copyright (C) 2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef TEMPLATEEXTRACTHTMLELEMENTWEBENGINEVIEW_H
#define TEMPLATEEXTRACTHTMLELEMENTWEBENGINEVIEW_H

#include <QWebEngineView>
#include "templateparser_private_export.h"
namespace TemplateParser
{
class TemplateWebEnginePage;
class TEMPLATEPARSER_TESTS_EXPORT TemplateExtractHtmlElementWebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit TemplateExtractHtmlElementWebEngineView(QWidget *parent = nullptr);
    ~TemplateExtractHtmlElementWebEngineView();

    QString bodyElement() const;

    QString headerElement() const;

    QString htmlElement() const;

    void setHtmlContent(const QString &html);

Q_SIGNALS:
    void loadContentDone(bool success);

private:
    void clear();
    void slotLoadFinished(bool success);
    void handleHtmlInfo(const QVariant &result);

    QString mBodyElement;
    QString mHeaderElement;
    QString mHtmlElement;

    TemplateWebEnginePage *mPage = nullptr;
};
}
#endif // TEMPLATEEXTRACTHTMLELEMENTWEBENGINEVIEW_H
