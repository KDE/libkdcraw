#include "enhancedjob.h"

namespace KDcrawIface
{

EnhancedJob::EnhancedJob(): QObject(),Job()
{

}

EnhancedJob::~EnhancedJob()
{

}

void EnhancedJob::defaultEnd(const ThreadWeaver::JobPointer &job,
                                           ThreadWeaver::Thread *thread)
{
    Q_UNUSED(job);
    Q_UNUSED(thread);
    emit done(this);
}


}
