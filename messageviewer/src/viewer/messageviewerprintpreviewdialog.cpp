/*
  Copyright (c) 2015 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "messageviewerprintpreviewdialog.h"
#include "settings/messageviewersettings.h"
#include <KConfigGroup>
using namespace MessageViewer;

MessageViewerPrintPreviewDialog::MessageViewerPrintPreviewDialog(QWidget *parent)
    : QPrintPreviewDialog(parent)
{
    readConfig();
}

MessageViewerPrintPreviewDialog::~MessageViewerPrintPreviewDialog()
{
    writeConfig();
}

void MessageViewerPrintPreviewDialog::readConfig()
{
    KConfigGroup group(MessageViewer::MessageViewerSettings::self()->config(), "MessageViewerPrintPreviewDialog");
    const QSize size = group.readEntry("Size", QSize(800, 600));
    if (size.isValid()) {
        resize(size);
    }
}

void MessageViewerPrintPreviewDialog::writeConfig()
{
    KConfigGroup group(MessageViewer::MessageViewerSettings::self()->config(), "MessageViewerPrintPreviewDialog");
    group.writeEntry("Size", size());
    group.sync();
}
