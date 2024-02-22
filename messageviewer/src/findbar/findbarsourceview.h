/* SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <WebEngineViewer/FindBarBase>

class QPlainTextEdit;

namespace MessageViewer
{
class FindBarSourceView : public WebEngineViewer::FindBarBase
{
    Q_OBJECT

public:
    explicit FindBarSourceView(QPlainTextEdit *view, QWidget *parent = nullptr);
    ~FindBarSourceView() override;

private:
    FindBarSourceView(QWidget *parent) = delete;

    void clearSelections() override;
    void searchText(bool backward, bool isAutoSearch) override;
    void updateHighLight(bool) override;
    void updateSensitivity(bool) override;

    QPlainTextEdit *const mView;
};
}
