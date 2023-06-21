/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "utils/optionseteditor.h"
#include "core/optionset.h"

#include <QGridLayout>
#include <QLabel>

#include <KLocalizedString>
#include <KTextEdit>
#include <QLineEdit>

using namespace MessageList::Utils;
using namespace MessageList::Core;

OptionSetEditor::OptionSetEditor(QWidget *parent)
    : QTabWidget(parent)
{
    // General tab
    auto tab = new QWidget(this);
    addTab(tab, i18nc("@title:tab General options of a view mode", "General"));

    auto tabg = new QGridLayout(tab);

    auto l = new QLabel(i18nc("@label:textbox Name of the option", "Name:"), tab);
    tabg->addWidget(l, 0, 0);

    mNameEdit = new QLineEdit(tab);

    tabg->addWidget(mNameEdit, 0, 1);

    connect(mNameEdit, &QLineEdit::textEdited, this, &OptionSetEditor::slotNameEditTextEdited);

    l = new QLabel(i18nc("@label:textbox Description of the option", "Description:"), tab);
    tabg->addWidget(l, 1, 0);

    mDescriptionEdit = new KTextEdit(tab);
    mDescriptionEdit->setAcceptRichText(false);
    tabg->addWidget(mDescriptionEdit, 1, 1, 2, 1);

    tabg->setColumnStretch(1, 1);
    tabg->setRowStretch(2, 1);
}

OptionSetEditor::~OptionSetEditor() = default;

void OptionSetEditor::setReadOnly(bool readOnly)
{
    mDescriptionEdit->setReadOnly(readOnly);
    mNameEdit->setReadOnly(readOnly);
}

KTextEdit *OptionSetEditor::descriptionEdit() const
{
    return mDescriptionEdit;
}

QLineEdit *OptionSetEditor::nameEdit() const
{
    return mNameEdit;
}

#include "moc_optionseteditor.cpp"
