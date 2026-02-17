/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QDialog>

namespace TextCustomEditor
{
class RichTextEditorWidget;
}

namespace WebEngineViewer
{
/*!
 * \class WebEngineViewer::TrackingDetailsDialog
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/TrackingDetailsDialog
 *
 * \brief The TrackingDetailsDialog class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT TrackingDetailsDialog : public QDialog
{
    Q_OBJECT
public:
    /*! Constructs a TrackingDetailsDialog with the given parent. */
    explicit TrackingDetailsDialog(QWidget *parent = nullptr);
    /*! Destroys the TrackingDetailsDialog. */
    ~TrackingDetailsDialog() override;

    /*! Sets the details text to display. */
    void setDetails(const QString &details);

private:
    WEBENGINEVIEWER_NO_EXPORT void writeConfig();
    WEBENGINEVIEWER_NO_EXPORT void readConfig();

    TextCustomEditor::RichTextEditorWidget *const mDetails;
};
}
