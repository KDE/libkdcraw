#ifndef ENHANCEDJOB_H
#define ENHANCEDJOB_H
#include "ThreadWeaver/Job"
#include "kdcraw_export.h"
namespace KDcrawIface
{

class LIBKDCRAW_EXPORT EnhancedJob : public QObject, public ThreadWeaver::Job
{
    Q_OBJECT
public:
    EnhancedJob();
    virtual ~EnhancedJob();

protected:
    void defaultEnd(const ThreadWeaver::JobPointer &job,
                    ThreadWeaver::Thread *thread);
signals:
    void done(Job*);
};
}

#endif
