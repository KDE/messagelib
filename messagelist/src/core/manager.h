/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "core/sortorder.h"
#include <QList>
#include <QMap>
#include <QObject>

#include <collection.h>

namespace KMime
{
class DateFormatter;
}

namespace MessageList
{
namespace Core
{
class Aggregation;
class Theme;
class StorageModel;
class Widget;

/**
 * @brief: The manager for all the existing MessageList::Widget objects.
 *
 * This class is the "central" object of the whole MessageList framework.
 * It's a singleton that can be accessed only by the means of static methods,
 * is created automatically when the first MessageList::Widget object is created
 * and destroyed automatically when the last MessageList::Widget object is destroyed.
 *
 * This class takes care of loading/storing/maintaining the settings for the
 * whole MessageList framework. It also keeps track of all the existing
 * MessageList::Widget objects and takes care of updating them when settings change.
 */
class Manager : public QObject
{
    Q_OBJECT
protected:
    explicit Manager();
    ~Manager() override;

private:
    static Manager *mInstance;
    QList<Widget *> mWidgetList;
    QMap<QString, Aggregation *> mAggregations;
    QMap<QString, Theme *> mThemes;
    KMime::DateFormatter *mDateFormatter = nullptr;
    QString mCachedLocalizedUnknownText;

public:
    // instance management
    static Manager *instance()
    {
        return mInstance;
    }

    // widget registration
    static void registerWidget(Widget *pWidget);
    static void unregisterWidget(Widget *pWidget);

    const KMime::DateFormatter *dateFormatter() const
    {
        return mDateFormatter;
    }

    const QString &cachedLocalizedUnknownText() const
    {
        return mCachedLocalizedUnknownText;
    }

    // aggregation sets management
    const Aggregation *aggregationForStorageModel(const StorageModel *storageModel, bool *storageUsesPrivateAggregation);
    const Aggregation *aggregationForStorageModel(const QString &storageModel, bool *storageUsesPrivateAggregation);
    const Aggregation *aggregationForStorageModel(const Akonadi::Collection &storageModel, bool *storageUsesPrivateAggregation);

    void saveAggregationForStorageModel(const StorageModel *storageModel, const QString &id, bool storageUsesPrivateAggregation);
    void saveAggregationForStorageModel(const QString &index, const QString &id, bool storageUsesPrivateAggregation);
    void saveAggregationForStorageModel(const Akonadi::Collection &col, const QString &id, bool storageUsesPrivateAggregation);

    const Aggregation *defaultAggregation();
    const Aggregation *aggregation(const QString &id);

    void addAggregation(Aggregation *set);
    void removeAllAggregations();

    const QMap<QString, Aggregation *> &aggregations() const
    {
        return mAggregations;
    }

    /**
     * This is called by the aggregation configuration dialog
     * once the sets have been changed.
     */
    void aggregationsConfigurationCompleted();

    // sort order management
    const SortOrder sortOrderForStorageModel(const StorageModel *storageModel, bool *storageUsesPrivateSortOrder);
    void saveSortOrderForStorageModel(const StorageModel *storageModel, SortOrder order, bool storageUsesPrivateSortOrder);

    // theme sets management
    const Theme *themeForStorageModel(const Akonadi::Collection &col, bool *storageUsesPrivateTheme);
    const Theme *themeForStorageModel(const StorageModel *storageModel, bool *storageUsesPrivateTheme);
    const Theme *themeForStorageModel(const QString &id, bool *storageUsesPrivateTheme);

    void saveThemeForStorageModel(const StorageModel *storageModel, const QString &id, bool storageUsesPrivateTheme);
    void saveThemeForStorageModel(int index, const QString &id, bool storageUsesPrivateTheme);
    void saveThemeForStorageModel(const QString &storageModelIndex, const QString &id, bool storageUsesPrivateTheme);

    const Theme *defaultTheme();
    const Theme *theme(const QString &id);

    void addTheme(Theme *set);
    void removeAllThemes();

    const QMap<QString, Theme *> &themes() const
    {
        return mThemes;
    }

    /**
     * This is called by the theme configuration dialog
     * once the sets have been changed.
     */
    void themesConfigurationCompleted();

protected Q_SLOTS:
    /**
     * Reloads the global configuration from the config files (so we assume it has changed)
     * The settings private to MessageList (like Themes or Aggregations) aren't reloaded.
     * If the global configuration has changed then all the views are reloaded.
     */
    void reloadGlobalConfiguration();

    /**
     * Explicitly reloads the contents of all the widgets.
     */
    void reloadAllWidgets();

Q_SIGNALS:
    void aggregationsChanged();
    void themesChanged();

private:
    // internal configuration stuff
    void loadConfiguration();
    void saveConfiguration();
    void loadGlobalConfiguration();
    void saveGlobalConfiguration();

    // internal option set management
    void createDefaultAggregations();
    void createDefaultThemes();
};
} // namespace Core
} // namespace MessageList

