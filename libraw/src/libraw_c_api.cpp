/* -*- C++ -*-
 * File: libraw_c_api.cpp
 * Copyright 2008 Alex Tutubalin <lexa@lexa.ru>
 * Created: Sat Mar  8 , 2008
 *
 * LibRaw C++ interface (implementation)
 */
#include <errno.h>
#include "libraw/libraw.h"

#ifdef __cplusplus
extern "C" 
{
#endif

    libraw_data_t *libraw_init(unsigned int flags)
    {
        LibRaw *ret = new LibRaw(flags);
        return &(ret->imgdata);
    }

    const char*   libraw_version() { return LibRaw::version();}
    int     libraw_versionNumber() { return LibRaw::versionNumber();}
    const char**  libraw_cameraList() { return LibRaw::cameraList();}
    int   libraw_cameraCount() { return LibRaw::cameraCount(); }
    int libraw_open_file(libraw_data_t* lr, const char *file)
    {
        if(!lr) return EINVAL;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        return ip->open_file(file);
    }
    int libraw_unpack(libraw_data_t* lr)
    {
        if(!lr) return EINVAL;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        return ip->unpack();
    }
    int libraw_unpack_thumb(libraw_data_t* lr)
    {
        if(!lr) return EINVAL;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        return ip->unpack_thumb();
    }
    void libraw_recycle(libraw_data_t* lr)
    {
        if(!lr) return;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        ip->recycle();
    }
    void libraw_close(libraw_data_t* lr)
    {
        if(!lr) return;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        delete ip;
    }

    void  libraw_set_memerror_handler(libraw_data_t* lr, memory_callback cb)
    {
        if(!lr) return;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        ip->set_memerror_handler(cb);

    }
    void libraw_set_dataerror_handler(libraw_data_t* lr,data_callback func)
    {
        if(!lr) return;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        ip->set_dataerror_handler(func);

    }

    // DCRAW
    int  libraw_adjust_sizes_info_only(libraw_data_t* lr)
    {
        if(!lr) return EINVAL;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        return ip->adjust_sizes_info_only();
    }

    int  libraw_dcraw_document_mode_processing(libraw_data_t* lr)
    {
        if(!lr) return EINVAL;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        return ip->dcraw_document_mode_processing();

    }
    int  libraw_dcraw_ppm_tiff_writer(libraw_data_t* lr,const char *filename)
    {
        if(!lr) return EINVAL;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        return ip->dcraw_ppm_tiff_writer(filename);
    }
    int  libraw_dcraw_thumb_writer(libraw_data_t* lr,const char *fname)
    {
        if(!lr) return EINVAL;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        return ip->dcraw_thumb_writer(fname);

    }
    int libraw_dcraw_process(libraw_data_t* lr)
    {
        if(!lr) return EINVAL;
        LibRaw *ip = (LibRaw*) lr->parent_class;
        return ip->dcraw_process();
    }

#ifdef __cplusplus
}
#endif
