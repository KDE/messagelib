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

#include "recipientspickerwidget.h"
#include "recipientseditormanager.h"
#include <QHBoxLayout>
#include <QAbstractItemView>
#include <QTreeView>
#include <Akonadi/Contact/EmailAddressSelectionWidget>
#include <Akonadi/Contact/EmailAddressSelectionModel>
#include <Akonadi/Contact/ContactsTreeModel>
using namespace MessageComposer;
RecipientsPickerWidget::RecipientsPickerWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    mView = new Akonadi::EmailAddressSelectionWidget(MessageComposer::RecipientsEditorManager::self()->model()->model(), this);
    layout->addWidget(mView);
    mView->view()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mView->view()->setAlternatingRowColors(true);
    mView->view()->setSortingEnabled(true);
    mView->view()->sortByColumn(0, Qt::AscendingOrder);
}

RecipientsPickerWidget::~RecipientsPickerWidget()
{
}

QTreeView *RecipientsPickerWidget::view() const
{
    return mView->view();
}

Akonadi::EmailAddressSelectionWidget *RecipientsPickerWidget::emailAddressSelectionWidget() const
{
    return mView;
}
