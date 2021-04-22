/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 * SPDX-FileCopyrightText: 2011-2021 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "templateparser_export.h"

#include <QStyledItemDelegate>
#include <QTreeWidgetItem>
#include <QWidget>

class KActionCollection;

class Ui_CustomTemplatesBase;

namespace TemplateParser
{
class CustomTemplateItem;
/**
 * @brief The CustomTemplates class
 */
class TEMPLATEPARSER_EXPORT CustomTemplates : public QWidget
{
    Q_OBJECT
public:
    enum Type { TUniversal, TReply, TReplyAll, TForward };

public:
    explicit CustomTemplates(const QList<KActionCollection *> &actionCollection, QWidget *parent = nullptr);
    ~CustomTemplates() override;

    void load();
    void save();

Q_SIGNALS:
    void changed();
    void templatesUpdated();

private Q_SLOTS:
    void slotInsertCommand(const QString &cmd, int adjustCursor = 0);
    void slotTextChanged();
    void slotAddClicked();
    void slotRemoveClicked();
    void slotListSelectionChanged();
    void slotTypeActivated(int index);
    void slotShortcutChanged(const QKeySequence &newSeq);
    void slotItemChanged(QTreeWidgetItem *item, int column);
    void slotHelpLinkClicked(const QString &);
    void slotNameChanged(const QString &text);
    void slotDuplicateClicked();

private:
    Q_REQUIRED_RESULT bool nameAlreadyExists(const QString &str, QTreeWidgetItem *item = nullptr);
    Q_REQUIRED_RESULT QString indexToType(int index);
    Q_REQUIRED_RESULT QString createUniqueName(const QString &name) const;
    void iconFromType(CustomTemplates::Type type, CustomTemplateItem *item);

    /// These templates will be deleted when we're saving.
    QStringList mItemsToDelete;

    QIcon mReplyPix;
    QIcon mReplyAllPix;
    QIcon mForwardPix;

    /// Whether or not to Q_EMIT the changed() signal. This is useful to disable when loading
    /// templates, which changes the UI without user action
    bool mBlockChangeSignal = false;

    Ui_CustomTemplatesBase *const mUi;
};

class CustomTemplateItem : public QTreeWidgetItem
{
public:
    explicit CustomTemplateItem(QTreeWidget *parent,
                                const QString &name,
                                const QString &content,
                                const QKeySequence &shortcut,
                                CustomTemplates::Type type,
                                const QString &to,
                                const QString &cc);
    ~CustomTemplateItem();
    void setCustomType(CustomTemplates::Type type);
    CustomTemplates::Type customType() const;

    Q_REQUIRED_RESULT QString to() const;
    Q_REQUIRED_RESULT QString cc() const;

    void setTo(const QString &);
    void setCc(const QString &);

    Q_REQUIRED_RESULT QString content() const;
    void setContent(const QString &);

    Q_REQUIRED_RESULT QKeySequence shortcut() const;
    void setShortcut(const QKeySequence &);

    Q_REQUIRED_RESULT QString oldName() const;
    void setOldName(const QString &);

private:
    QString mName, mContent;
    QKeySequence mShortcut;
    CustomTemplates::Type mType;
    QString mTo, mCC;
};

class CustomTemplateItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CustomTemplateItemDelegate(QObject *parent = nullptr);
    ~CustomTemplateItemDelegate() override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};
}

