/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Based on ideas by Stephen Kelly.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "jobbase.h"
#include "messagecomposer_export.h"

namespace KMime
{
class Content;
}

namespace MessageComposer
{
class ContentJobBasePrivate;
/**
 * @brief The ContentJobBase class
 */
class MESSAGECOMPOSER_EXPORT ContentJobBase : public JobBase
{
    Q_OBJECT

public:
    explicit ContentJobBase(QObject *parent = nullptr);
    ~ContentJobBase() override;

    /**
      Starts processing this ContentJobBase asynchronously.
      This processes all children in order first, then calls process().
      Emits finished() after all processing is done, and the
      content is reachable through content().
    */
    void start() override;

    /**
      Get the resulting KMime::Content that the ContentJobBase has generated.
      Jobs never delete their content.
    */
    [[nodiscard]] KMime::Content *content() const;

    /**
      This is meant to be used instead of KCompositeJob::addSubjob(), making
      it possible to add subjobs from the outside.
      Transfers ownership of the @p job to this object.
    */
    bool appendSubjob(ContentJobBase *job);

    /**
      Set some extra content to be saved with the job, and available
        later, for example, in slot handling result of job.
      Job does not take care of deleting extra content.
      */
    void setExtraContent(KMime::Content *extra);

    /**
      Get extra content that was previously added.
     */
    [[nodiscard]] KMime::Content *extraContent() const;

protected:
    ContentJobBase(ContentJobBasePrivate &dd, QObject *parent);

    /** Use appendSubjob() instead. */
    bool addSubjob(KJob *job) override;

protected Q_SLOTS:
    /**
      Reimplement to do additional stuff before processing children, such as
      adding more subjobs.  Remember to call the base implementation.
    */
    virtual void doStart();

    /**
      This is called after all the children have been processed.
      (You must use their resulting contents, or delete them.)
      Reimplement in subclasses to process concrete content.  Call
      emitResult() when finished.
    */
    virtual void process() = 0;

    void slotResult(KJob *job) override;

private:
    Q_DECLARE_PRIVATE(ContentJobBase)
};
} // namespace MessageComposer
