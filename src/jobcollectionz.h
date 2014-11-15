#ifndef JOBCOLLECTIONZ_H
#define JOBCOLLECTIONZ_H

#include <QObject>
#include "ThreadWeaver/Collection"

#include "kdcraw_export.h"
namespace KDcrawIface
{
class LIBKDCRAW_EXPORT JobCollectionz : public QObject, public ThreadWeaver::Collection
{
    Q_OBJECT
public:
    JobCollectionz();
    virtual ~JobCollectionz();

protected:
    void defaultEnd(const ThreadWeaver::JobPointer& job,
                    ThreadWeaver::Thread *thread) Q_DECL_OVERRIDE;
signals:
    void signalDone(ThreadWeaver::Job*);
    void signalStarted(ThreadWeaver::Job*);
};

} // namespace ThreadWeaver
#endif
