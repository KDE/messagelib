/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 * SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>
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
    enum Type {
        TUniversal,
        TReply,
        TReplyAll,
        TForward,
    };

public:
    explicit CustomTemplates(const QList<KActionCollection *> &actionCollection, QWidget *parent = nullptr);
    ~CustomTemplates() override;

    void load();
    void save();

Q_SIGNALS:
    void changed();
    void templatesUpdated();

private Q_SLOTS:
    TEMPLATEPARSER_NO_EXPORT void slotInsertCommand(const QString &cmd, int adjustCursor = 0);
    TEMPLATEPARSER_NO_EXPORT void slotTextChanged();
    TEMPLATEPARSER_NO_EXPORT void slotAddClicked();
    TEMPLATEPARSER_NO_EXPORT void slotRemoveClicked();
    TEMPLATEPARSER_NO_EXPORT void slotListSelectionChanged();
    TEMPLATEPARSER_NO_EXPORT void slotTypeActivated(int index);
    TEMPLATEPARSER_NO_EXPORT void slotShortcutChanged(const QKeySequence &newSeq);
    TEMPLATEPARSER_NO_EXPORT void slotItemChanged(QTreeWidgetItem *item, int column);
    TEMPLATEPARSER_NO_EXPORT void slotHelpLinkClicked(const QString &);
    TEMPLATEPARSER_NO_EXPORT void slotNameChanged(const QString &text);
    TEMPLATEPARSER_NO_EXPORT void slotDuplicateClicked();

private:
    [[nodiscard]] TEMPLATEPARSER_NO_EXPORT bool nameAlreadyExists(const QString &str, QTreeWidgetItem *item = nullptr);
    [[nodiscard]] TEMPLATEPARSER_NO_EXPORT QString indexToType(int index);
    [[nodiscard]] TEMPLATEPARSER_NO_EXPORT QString createUniqueName(const QString &name) const;
    TEMPLATEPARSER_NO_EXPORT void iconFromType(CustomTemplates::Type type, CustomTemplateItem *item);

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
    ~CustomTemplateItem() override;
    void setCustomType(CustomTemplates::Type type);
    [[nodiscard]] CustomTemplates::Type customType() const;

    [[nodiscard]] QString to() const;
    [[nodiscard]] QString cc() const;

    void setTo(const QString &);
    void setCc(const QString &);

    [[nodiscard]] QString content() const;
    void setContent(const QString &);

    [[nodiscard]] QKeySequence shortcut() const;
    void setShortcut(const QKeySequence &);

    [[nodiscard]] QString oldName() const;
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
    [[nodiscard]] QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};
}
