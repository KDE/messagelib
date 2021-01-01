/* SPDX-FileCopyrightText: 2011-2021 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef FINDBARSOURCEVIEW_H
#define FINDBARSOURCEVIEW_H

#include <WebEngineViewer/FindBarBase>

class QPlainTextEdit;

namespace MessageViewer {
class FindBarSourceView : public WebEngineViewer::FindBarBase
{
    Q_OBJECT

public:
    explicit FindBarSourceView(QPlainTextEdit *view, QWidget *parent = nullptr);
    ~FindBarSourceView() override;

private:
    explicit FindBarSourceView(QWidget *parent)
    {
        Q_UNUSED(parent)
    }

    void clearSelections() override;
    void searchText(bool backward, bool isAutoSearch) override;
    void updateHighLight(bool) override;
    void updateSensitivity(bool) override;

private:
    QPlainTextEdit *mView = nullptr;
};
}

#endif /* FINDBARSOURCEVIEW_H */
