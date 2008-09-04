/* -*- C++ -*-
 * File: libraw_cxx.cpp
 * Copyright 2008 Alex Tutubalin <lexa@lexa.ru>
 * Created: Sat Mar  8 , 2008
 *
 * LibRaw C++ interface (implementation)
 */

#include <errno.h>
#include <float.h>
#include <math.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
#define LIBRAW_LIBRARY_BUILD
#include "libraw/libraw.h"

#ifdef __cplusplus
extern "C" 
{
#endif
    void default_memory_callback(const char *file,const char *where)
    {
        fprintf (stderr,"%s: Out of memory in %s\n", file, where);
    }

    void default_data_callback(const char *file, const int offset)
    {
        if(offset < 0)
            fprintf (stderr,"%s: Unexpected end of file\n", file);
        else
            fprintf (stderr,"%s: data corrupted at %d\n",file,offset); 
    }
    const char *libraw_strerror(int e)
    {
        enum LibRaw_errors errorcode = (LibRaw_errors)e;
        switch(errorcode)
            {
            case        LIBRAW_SUCCESS:
                return "No error";
            case        LIBRAW_UNSPECIFIED_ERROR:
                return "Unspecified error";
            case        LIBRAW_FILE_UNSUPPORTED:
                return "Unsupported file format or not RAW file";
            case        LIBRAW_REQUEST_FOR_NONEXISTENT_IMAGE:
                return "Request for nonexisting image number";
            case        LIBRAW_OUT_OF_ORDER_CALL:
                return "Out of order call of libraw function";
            case    LIBRAW_NO_THUMBNAIL:
                return "No thumbnail in file";
            case    LIBRAW_UNSUPPORTED_THUMBNAIL:
                return "Unsupported thumbnail format";
            case    LIBRAW_UNSUFFICIENT_MEMORY:
                return "Unsufficient memory";
            case    LIBRAW_DATA_ERROR:
                return "Corrupted data or unexpected EOF";
            case    LIBRAW_IO_ERROR:
                return "Input/output error";
            default:
                return "Unknown error code";
        }
    }

#ifdef __cplusplus
}
#endif


const double LibRaw_constants::xyz_rgb[3][3] = 
{
    { 0.412453, 0.357580, 0.180423 },
    { 0.212671, 0.715160, 0.072169 },
    { 0.019334, 0.119193, 0.950227 } 
};

const float LibRaw_constants::d65_white[3] =  { 0.950456, 1, 1.088754 };

#define P1 imgdata.idata
#define S imgdata.sizes
#define O imgdata.params
#define C imgdata.color
#define T imgdata.thumbnail
#define IO libraw_internal_data.internal_output_params
#define ID libraw_internal_data.internal_data

#define EXCEPTION_HANDLER(e) do{                        \
        fprintf(stderr,"Exception %d caught\n",e);      \
        switch(e)                                       \
            {                                           \
            case LIBRAW_EXCEPTION_ALLOC:                \
                recycle();                              \
                return LIBRAW_UNSUFFICIENT_MEMORY;      \
            case LIBRAW_EXCEPTION_DECODE_RAW:           \
            case LIBRAW_EXCEPTION_DECODE_JPEG:          \
                recycle();                              \
                return LIBRAW_DATA_ERROR;               \
            case LIBRAW_EXCEPTION_IO_EOF:               \
            case LIBRAW_EXCEPTION_IO_CORRUPT:           \
                recycle();                              \
                return LIBRAW_IO_ERROR;                 \
            default:                                    \
                return LIBRAW_UNSPECIFIED_ERROR;        \
            } \
    }while(0)


int LibRaw::open_file(const char *fname)
{

    if(!fname)
        return ENOENT;
        
    recycle();
    libraw_internal_data.internal_data.ifname = (char*)fname;
    try {
        if(!(ID.input = fopen(ID.ifname,"rb")))
            return errno;

        SET_PROC_FLAG(LIBRAW_PROGRESS_OPEN);

        if (O.use_camera_matrix < 0)
            O.use_camera_matrix = O.use_camera_wb;

        identify();
        if(C.profile_length)
            {
                if(C.profile) free(C.profile);
                C.profile = malloc(C.profile_length);
                merror(C.profile,"LibRaw::open_file()");
                fseek(ID.input,ID.profile_offset,SEEK_SET);
                fread(C.profile,C.profile_length,1,ID.input);
            }
        
        SET_PROC_FLAG(LIBRAW_PROGRESS_IDENTIFY);
    }
    catch ( LibRaw_exceptions err) {
        EXCEPTION_HANDLER(err);
    }

    if(P1.raw_count < 1) 
        return LIBRAW_FILE_UNSUPPORTED;

    if (O.user_flip >= 0)
        S.flip = O.user_flip;
    
    switch ((S.flip+3600) % 360) 
        {
        case 270:  S.flip = 5;  break;
        case 180:  S.flip = 3;  break;
        case  90:  S.flip = 6;  break;
        }
    
    write_fun = &LibRaw::write_ppm_tiff;
    
    if (load_raw == &LibRaw::kodak_ycbcr_load_raw) 
        {
            S.height += S.height & 1;
            S.width  += S.width  & 1;
        }

    IO.shrink = P1.filters && (O.half_size || O.threshold || O.aber[0] != 1 || O.aber[2] != 1);
    S.iheight = (S.height + IO.shrink) >> IO.shrink;
    S.iwidth  = (S.width  + IO.shrink) >> IO.shrink;
    
    SET_PROC_FLAG(LIBRAW_PROGRESS_SIZE_ADJUST);

    return LIBRAW_SUCCESS;
}

int LibRaw::unpack(void)
{
    CHECK_ORDER_HIGH(LIBRAW_PROGRESS_LOAD_RAW);
    CHECK_ORDER_LOW(LIBRAW_PROGRESS_IDENTIFY);
    try {

        if (O.shot_select >= P1.raw_count)
            return LIBRAW_REQUEST_FOR_NONEXISTENT_IMAGE;
        
        if(!load_raw)
            return LIBRAW_UNSPECIFIED_ERROR;
        
        if (O.use_camera_matrix && C.cmatrix[0][0] > 0.25) 
            {
                memcpy (C.rgb_cam, C.cmatrix, sizeof (C.cmatrix));
                IO.raw_color = 0;
            }
        // already allocated ?
        if(imgdata.image) free(imgdata.image);
        
        imgdata.image = (ushort (*)[4]) calloc (S.iheight*S.iwidth, sizeof (*imgdata.image));
        merror (imgdata.image, "unpack()");
        if (libraw_internal_data.unpacker_data.meta_length) 
            {
                libraw_internal_data.internal_data.meta_data = 
                    (char *) malloc (libraw_internal_data.unpacker_data.meta_length);
                merror (libraw_internal_data.internal_data.meta_data, "LibRaw::unpack()");
            }
        fseek (ID.input, libraw_internal_data.unpacker_data.data_offset, SEEK_SET);
        // foveon_load_raw produces different data for document_mode, we'll
        // deal with it in dcraw_document_mode_processing
        int save_document_mode = O.document_mode;
        O.document_mode = 0;
        
        (this->*load_raw)();
        
        O.document_mode = save_document_mode;
        
        SET_PROC_FLAG(LIBRAW_PROGRESS_LOAD_RAW);
        
        return 0;
    }
    catch ( LibRaw_exceptions err) {
        EXCEPTION_HANDLER(err);
    }
}

int LibRaw::dcraw_document_mode_processing(void)
{
    CHECK_ORDER_HIGH(LIBRAW_PROGRESS_PRE_INTERPOLATE);
    CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

    try {
        O.document_mode = 2;
        if(P1.is_foveon)
            {
                // filter image data for foveon document mode
                short *iptr = (short *)imgdata.image;
                for (int i=0; i < S.height*S.width*4; i++)
                    {
                        if ((short) iptr[i] < 0) 
                            iptr[i] = 0;
                    }
                SET_PROC_FLAG(LIBRAW_PROGRESS_FOVEON_INTERPOLATE);
            }

        O.use_fuji_rotate = 0;
        if (IO.zero_is_bad)
            {
                remove_zeroes();
                SET_PROC_FLAG(LIBRAW_PROGRESS_REMOVE_ZEROES);
            }

        if (O.user_black >= 0) 
            C.black = O.user_black;

        if (O.user_sat > 0) 
            C.maximum = O.user_sat;

        pre_interpolate();
        SET_PROC_FLAG(LIBRAW_PROGRESS_PRE_INTERPOLATE);

        if (libraw_internal_data.internal_output_params.mix_green)
            {
                int i;
                for (P1.colors=3, i=0; i < S.height*S.width; i++)
                    imgdata.image[i][1] = (imgdata.image[i][1] + imgdata.image[i][3]) >> 1;
            }
        SET_PROC_FLAG(LIBRAW_PROGRESS_MIX_GREEN);

        if (!P1.is_foveon && P1.colors == 3) 
            median_filter();
        SET_PROC_FLAG(LIBRAW_PROGRESS_MEDIAN_FILTER);

        if (!P1.is_foveon && O.highlight == 2) 
            blend_highlights();

        if (!P1.is_foveon && O.highlight > 2) 
            recover_highlights();
        SET_PROC_FLAG(LIBRAW_PROGRESS_HIGHLIGHTS);

        if (O.use_fuji_rotate) 
            fuji_rotate();
        SET_PROC_FLAG(LIBRAW_PROGRESS_FUJI_ROTATE);

        if(!libraw_internal_data.output_data.histogram)
            {
                libraw_internal_data.output_data.histogram = (int (*)[LIBRAW_HISTOGRAM_SIZE]) malloc(sizeof(*libraw_internal_data.output_data.histogram)*4);
                merror(libraw_internal_data.output_data.histogram,"LibRaw::dcraw_document_mode_processing()");
            }
        convert_to_rgb();
        SET_PROC_FLAG(LIBRAW_PROGRESS_CONVERT_RGB);

        if (O.use_fuji_rotate)
            stretch();
        SET_PROC_FLAG(LIBRAW_PROGRESS_STRETCH);
        return 0;
    }
    catch ( LibRaw_exceptions err) {
        EXCEPTION_HANDLER(err);
    }

}

int LibRaw::dcraw_ppm_tiff_writer(const char *filename)
{
    CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

    if(!imgdata.image) 
        return LIBRAW_OUT_OF_ORDER_CALL;

    if(!filename) 
        return ENOENT;
    FILE *f = fopen(filename,"wb");

    if(!f) 
        return errno;
        
    try {
        if(!libraw_internal_data.output_data.histogram)
            {
                libraw_internal_data.output_data.histogram = 
                    (int (*)[LIBRAW_HISTOGRAM_SIZE]) malloc(sizeof(*libraw_internal_data.output_data.histogram)*4);
                merror(libraw_internal_data.output_data.histogram,"LibRaw::dcraw_ppm_tiff_writer()");
            }
        write_ppm_tiff(f);
        SET_PROC_FLAG(LIBRAW_PROGRESS_FLIP);
        fclose(f);
        return 0;
    }
    catch ( LibRaw_exceptions err) {
        fclose(f);
        EXCEPTION_HANDLER(err);
    }
}

void LibRaw::kodak_thumb_loader()
{
    // some kodak cameras
    ushort s_height = S.height, s_width = S.width,s_iwidth = S.iwidth,s_iheight=S.iheight;
    int s_colors = P1.colors;
    unsigned s_filters = P1.filters;
    ushort (*s_image)[4] = imgdata.image;

    
    S.height = T.theight;
    S.width  = T.twidth;
    P1.filters = 0;

    if (thumb_load_raw == &CLASS kodak_ycbcr_load_raw) 
        {
            S.height += S.height & 1;
            S.width  += S.width  & 1;
        }
    
    imgdata.image = (ushort (*)[4]) calloc (S.iheight*S.iwidth, sizeof (*imgdata.image));
    merror (imgdata.image, "LibRaw::kodak_thumb_loader()");

    fseek (ID.input,ID.toffset, SEEK_SET);
    // read kodak thumbnail into T.image[]
    (this->*thumb_load_raw)();

    // copy-n-paste from image pipe
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define LIM(x,min,max) MAX(min,MIN(x,max))
#define CLIP(x) LIM(x,0,65535)
#define SWAP(a,b) { a ^= b; a ^= (b ^= a); }

    // from scale_colors
    {
        double   dmax;
        float scale_mul[4];
        int c,val;
        for (dmax=DBL_MAX, c=0; c < 3; c++) 
                if (dmax > C.pre_mul[c])
                    dmax = C.pre_mul[c];

        for( c=0; c< 3; c++)
                scale_mul[c] = (C.pre_mul[c] / dmax) * 65535.0 / C.maximum;
        scale_mul[3] = scale_mul[1];

        size_t size = S.height * S.width;
        for (int i=0; i < size*4 ; i++) 
            {
                val = imgdata.image[0][i];
                if(!val) continue;
                val *= scale_mul[i & 3];
                imgdata.image[0][i] = CLIP(val);
            }
    }

    // from convert_to_rgb
    ushort *img;
    int row,col;
    
    int  (*t_hist)[LIBRAW_HISTOGRAM_SIZE] =  (int (*)[LIBRAW_HISTOGRAM_SIZE]) calloc(sizeof(*t_hist),4);
    merror (t_hist, "LibRaw::kodak_thumb_loader()");
    
    float out[3], 
        out_cam[3][4] = 
        {
            {2.81761312, -1.98369181, 0.166078627, 0}, 
            {-0.111855984, 1.73688626, -0.625030339, 0}, 
            {-0.0379119813, -0.891268849, 1.92918086, 0}
        };

    for (img=imgdata.image[0], row=0; row < S.height; row++)
        for (col=0; col < S.width; col++, img+=4)
            {
                out[0] = out[1] = out[2] = 0;
                for(int c=0;c<3;c++) 
                    {
                        out[0] += out_cam[0][c] * img[c];
                        out[1] += out_cam[1][c] * img[c];
                        out[2] += out_cam[2][c] * img[c];
                    }
                for(int c=0; c<3; c++)
                    img[c] = CLIP((int) out[c]);
                for(int c=0; c<P1.colors;c++)
                    t_hist[c][img[c] >> 3]++;
                    
            }

    // from gamma_lut
    int  (*save_hist)[LIBRAW_HISTOGRAM_SIZE] = libraw_internal_data.output_data.histogram;
    libraw_internal_data.output_data.histogram = t_hist;
    
    uchar *lut = (uchar*)calloc(0x10000,1);
    merror(lut,"LibRaw::kodak_thumb_loader()");
    gamma_lut(lut);
    
    libraw_internal_data.output_data.histogram = save_hist;

    free(t_hist);
    
    // from write_ppm_tiff - copy pixels into bitmap
    
    S.iheight = S.height;
    S.iwidth  = S.width;
    if (S.flip & 4) SWAP(S.height,S.width);

    if(T.thumb) free(T.thumb);
    T.thumb = (char*) calloc (S.width * S.height, P1.colors);
    merror (T.thumb, "LibRaw::kodak_thumb_loader()");
    T.tlength = S.width * S.height * P1.colors;

    // from write_tiff_ppm
    {
        int soff  = flip_index (0, 0);
        int cstep = flip_index (0, 1) - soff;
        int rstep = flip_index (1, 0) - flip_index (0, S.width);
        
        for (int row=0; row < S.height; row++, soff += rstep) 
            {
                char *ppm = T.thumb + row*S.width*P1.colors;
                for (int col=0; col < S.width; col++, soff += cstep)
                    for(int c = 0; c < P1.colors; c++)
                        ppm [col*P1.colors+c] = lut[imgdata.image[soff][c]];
            }
    }
    free(lut);
    // restore variables
    free(imgdata.image);
    imgdata.image  = s_image;
    
    T.twidth = S.width;
    S.width = s_width;

    S.iwidth = s_iwidth;
    S.iheight = s_iheight;

    T.theight = S.height;
    S.height = s_height;

    T.tcolors = P1.colors;
    P1.colors = s_colors;

    P1.filters = s_filters;
}
#undef MIN
#undef MAX
#undef LIM
#undef CLIP
#undef SWAP


void LibRaw::foveon_thumb_loader (void)
{
    unsigned bwide, row, col, bitbuf=0, bit=1, c, i;
    struct decode *dindex;
    short pred[3];
    
    if(T.thumb) free(T.thumb);
    T.thumb = NULL;
    
    bwide = get4();
    if (bwide > 0) 
        {
            if (bwide < T.twidth*3) return;
            T.thumb = (char*)malloc(3*T.twidth * T.theight);
            merror (T.thumb, "foveon_thumb()");
            char *buf = (char*)malloc(bwide); 
            merror (buf, "foveon_thumb()");
            for (row=0; row < T.theight; row++) 
                {
                    fread(buf, 1, bwide, ID.input);
                    memmove(T.thumb+(row*T.twidth*3),buf,T.twidth*3);
                }
            free(buf);
            T.tlength = 3*T.twidth * T.theight;
            T.tformat = LIBRAW_THUMBNAIL_BITMAP;
            return;
        }
    else 
        {
            foveon_decoder (256, 0);
            T.thumb = (char*)malloc(3*T.twidth * T.theight);
            char *bufp = T.thumb;
            merror (T.thumb, "foveon_thumb()");
            for (row=0; row < T.theight; row++) 
                {
                    memset (pred, 0, sizeof pred);
                    if (!bit) get4();
                    for (bit=col=0; col < T.twidth; col++)
                        for(c=0;c<3;c++) 
                            {
                                for (dindex=first_decode; dindex->branch[0]; ) 
                                    {
                                        if ((bit = (bit-1) & 31) == 31)
                                            for (i=0; i < 4; i++)
                                                bitbuf = (bitbuf << 8) + fgetc(ID.input);
                                        dindex = dindex->branch[bitbuf >> bit & 1];
                                    }
                                pred[c] += dindex->leaf;
                                (*bufp++)=pred[c];
                            }
                }
            T.tformat = LIBRAW_THUMBNAIL_BITMAP;
            T.tlength = 3*T.twidth * T.theight;
        }
    return;
}


// Достает thumbnail из файла, ставит thumb_format в соответствии с форматом
int LibRaw::unpack_thumb(void)
{
    CHECK_ORDER_LOW(LIBRAW_PROGRESS_IDENTIFY);
    CHECK_ORDER_BIT(LIBRAW_PROGRESS_THUMB_LOAD);

    try {
        if ( !ID.toffset) 
            {
                return LIBRAW_NO_THUMBNAIL;
            } 
        else if (thumb_load_raw) 
            {
                kodak_thumb_loader();
                T.tformat = LIBRAW_THUMBNAIL_BITMAP;
                SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
                return 0;
            } 
        else 
            {
                fseek (ID.input,ID.toffset, SEEK_SET);
                if ( write_thumb == &LibRaw::jpeg_thumb)
                    {
                        if(T.thumb) free(T.thumb);
                        T.thumb = (char *) malloc (T.tlength);
                        merror (T.thumb, "jpeg_thumb()");
                        fread (T.thumb, 1, T.tlength, ID.input);
                        T.tcolors = 3;
                        T.tformat = LIBRAW_THUMBNAIL_JPEG;
                        SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
                        return 0;
                    }
                else if (write_thumb == &LibRaw::ppm_thumb)
                    {
                        T.tlength = T.twidth * T.theight*3;
                        if(T.thumb) free(T.thumb);

                        T.thumb = (char *) malloc (T.tlength);
                        merror (T.thumb, "ppm_thumb()");

                        fread (T.thumb, 1, T.tlength, ID.input);

                        T.tformat = LIBRAW_THUMBNAIL_BITMAP;
                        SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
                        return 0;

                    }
                else if (write_thumb == &LibRaw::foveon_thumb)
                    {
                        foveon_thumb_loader();
                        // may return with error, so format is set in
                        // foveon thumb loader itself
                        SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
                        return 0;
                    }
                // else if -- all other write_thumb cases!
                else
                    {
                        return LIBRAW_UNSUPPORTED_THUMBNAIL;
                    }
            }
        // last resort
        return LIBRAW_UNSUPPORTED_THUMBNAIL;
    }
    catch ( LibRaw_exceptions err) {
        EXCEPTION_HANDLER(err);
    }

}

int LibRaw::dcraw_thumb_writer(const char *fname)
{
//    CHECK_ORDER_LOW(LIBRAW_PROGRESS_THUMB_LOAD);

    if(!fname) 
        return ENOENT;
        
    FILE *tfp = fopen(fname,"wb");
    
    if(!tfp) 
        return errno;

    if(!T.thumb)
        return LIBRAW_OUT_OF_ORDER_CALL;

    try {
        switch (T.tformat)
            {
            case LIBRAW_THUMBNAIL_JPEG:
                jpeg_thumb_writer (tfp,T.thumb,T.tlength);
                break;
            case LIBRAW_THUMBNAIL_BITMAP:
                fprintf (tfp, "P6\n%d %d\n255\n", T.twidth, T.theight);
                fwrite (T.thumb, 1, T.tlength, tfp);
                break;
            default:
                fclose(tfp);
                return LIBRAW_UNSUPPORTED_THUMBNAIL;
           }
        fclose(tfp);
        return 0;
    }
    catch ( LibRaw_exceptions err) {
        fclose(tfp);
        EXCEPTION_HANDLER(err);
    }
}

int LibRaw::adjust_sizes_info_only(void)
{
    CHECK_ORDER_LOW(LIBRAW_PROGRESS_IDENTIFY);
    CHECK_ORDER_LOW(LIBRAW_PROGRESS_FUJI_ROTATE);
    if (O.use_fuji_rotate)
        {
            if (IO.fuji_width) 
                {
                    IO.fuji_width = (IO.fuji_width - 1 + IO.shrink) >> IO.shrink;
                    S.iwidth = (ushort)(IO.fuji_width / sqrt(0.5));
                    S.iheight = (ushort)( (S.iheight - IO.fuji_width) / sqrt(0.5));
                } 
            else 
                {
                    if (S.pixel_aspect < 1) S.iheight = (ushort)( S.iheight / S.pixel_aspect + 0.5);
                    if (S.pixel_aspect > 1) S.iwidth  = (ushort) (S.iwidth  * S.pixel_aspect + 0.5);
                }
        }
    SET_PROC_FLAG(LIBRAW_PROGRESS_FUJI_ROTATE);
    if (S.flip & 4)
        {
            unsigned short t = S.iheight;
            S.iheight=S.iwidth;
            S.iwidth = t;
            SET_PROC_FLAG(LIBRAW_PROGRESS_FLIP);
        }
    return 0;
}

int LibRaw::dcraw_process(void)
{
    int quality,i;


    CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);
    CHECK_ORDER_HIGH(LIBRAW_PROGRESS_PRE_INTERPOLATE);

    if(O.half_size) 
        O.four_color_rgb = 1;

    if (IO.zero_is_bad) 
        {
            remove_zeroes();
            SET_PROC_FLAG(LIBRAW_PROGRESS_REMOVE_ZEROES);
        }

    quality = 2 + !IO.fuji_width;
    if (O.user_qual >= 0) quality = O.user_qual;
    if (O.user_black >= 0) C.black = O.user_black;
    if (O.user_sat > 0) C.maximum = O.user_sat;

    if (P1.is_foveon && !O.document_mode) 
        {
            foveon_interpolate();
            SET_PROC_FLAG(LIBRAW_PROGRESS_FOVEON_INTERPOLATE);
        }

    if (!P1.is_foveon && O.document_mode < 2)
        {
            scale_colors();
            SET_PROC_FLAG(LIBRAW_PROGRESS_SCALE_COLORS);
        }

    pre_interpolate();
    SET_PROC_FLAG(LIBRAW_PROGRESS_PRE_INTERPOLATE);

    if (P1.filters && !O.document_mode) 
        {
            if (quality == 0)
                lin_interpolate();
            else if (quality == 1 || P1.colors > 3)
                vng_interpolate();
            else if (quality == 2)
                ppg_interpolate();
            else 
                ahd_interpolate();
            SET_PROC_FLAG(LIBRAW_PROGRESS_INTERPOLATE);
        }
    if (IO.mix_green)
        {
            for (P1.colors=3, i=0; i < S.height * S.width; i++)
                imgdata.image[i][1] = (imgdata.image[i][1] + imgdata.image[i][3]) >> 1;
            SET_PROC_FLAG(LIBRAW_PROGRESS_MIX_GREEN);
        }

    if(!P1.is_foveon)
        {
            if (P1.colors == 3) 
                {
                    median_filter();
                    SET_PROC_FLAG(LIBRAW_PROGRESS_MEDIAN_FILTER);
                }
            
            if (O.highlight == 2) 
                {
                    blend_highlights();
                    SET_PROC_FLAG(LIBRAW_PROGRESS_HIGHLIGHTS);
                }
            
            if (O.highlight > 2) 
                {
                    recover_highlights();
                    SET_PROC_FLAG(LIBRAW_PROGRESS_HIGHLIGHTS);
                }
        }
    if (O.use_fuji_rotate) 
        {
            fuji_rotate();
            SET_PROC_FLAG(LIBRAW_PROGRESS_FUJI_ROTATE);
        }
    
    if(!libraw_internal_data.output_data.histogram)
        {
            libraw_internal_data.output_data.histogram = (int (*)[LIBRAW_HISTOGRAM_SIZE]) malloc(sizeof(*libraw_internal_data.output_data.histogram)*4);
            merror(libraw_internal_data.output_data.histogram,"LibRaw::dcraw_process()");
        }

    convert_to_rgb();
    SET_PROC_FLAG(LIBRAW_PROGRESS_CONVERT_RGB);

    if (O.use_fuji_rotate) 
        {
            stretch();
            SET_PROC_FLAG(LIBRAW_PROGRESS_STRETCH);
        }
    return 0;
}

