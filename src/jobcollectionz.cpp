#include "jobcollectionz.h"

namespace KDcrawIface
{

JobCollectionz::JobCollectionz():Collection(),QObject()
{

}

JobCollectionz::~JobCollectionz()
{

}

void JobCollectionz::defaultEnd(const ThreadWeaver::JobPointer &job,
                                ThreadWeaver::Thread *thread)
{
    emit signalDone(this);
}

}
