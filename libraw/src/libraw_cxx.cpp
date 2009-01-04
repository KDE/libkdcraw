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
#else
#include <winsock2.h>
#endif
#define LIBRAW_LIBRARY_BUILD
#include "libraw/libraw.h"

#ifdef __cplusplus
extern "C" 
{
#endif
    void default_memory_callback(void *,const char *file,const char *where)
    {
        fprintf (stderr,"%s: Out of memory in %s\n", file, where);
    }

    void default_data_callback(void*,const char *file, const int offset)
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
            case LIBRAW_CANCELLED_BY_CALLBACK:
                return "Cancelled by user callback";
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
            case LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK: \
                recycle();                              \
                return LIBRAW_CANCELLED_BY_CALLBACK;     \
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

        RUN_CALLBACK(LIBRAW_PROGRESS_LOAD_RAW,0,2);
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
        RUN_CALLBACK(LIBRAW_PROGRESS_LOAD_RAW,1,2);
        
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
        if(O.bad_pixels) 
            {
                bad_pixels(O.bad_pixels);
                SET_PROC_FLAG(LIBRAW_PROGRESS_BAD_PIXELS);
            }
        if (O.dark_frame)
            {
                subtract (O.dark_frame);
                SET_PROC_FLAG(LIBRAW_PROGRESS_DARK_FRAME);
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
#ifndef NO_LCMS
	if(O.camera_profile)
            {
                apply_profile(O.camera_profile,O.output_profile);
                SET_PROC_FLAG(LIBRAW_PROGRESS_APPLY_PROFILE);
            }
#endif
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

#if 1
#define FORC(cnt) for (c=0; c < cnt; c++)
#define FORCC FORC(ret->colors)
#define SWAP(a,b) { a ^= b; a ^= (b ^= a); }

libraw_processed_image_t * LibRaw::dcraw_make_mem_thumb(int *errcode)
{
    if(!T.thumb)
        {
            if ( !ID.toffset) 
                {
                    if(errcode) *errcode= LIBRAW_NO_THUMBNAIL;
                }
            else
                {
                    if(errcode) *errcode= LIBRAW_OUT_OF_ORDER_CALL;
                }
            return NULL;
        }

    if (T.tformat == LIBRAW_THUMBNAIL_BITMAP)
        {
            libraw_processed_image_t * ret = 
                (libraw_processed_image_t *)::malloc(sizeof(libraw_processed_image_t)+T.tlength);

            if(!ret)
                {
                    if(errcode) *errcode= ENOMEM;
                    return NULL;
                }

            bzero(ret,sizeof(libraw_processed_image_t));
            ret->type   = LIBRAW_IMAGE_BITMAP;
            ret->height = T.theight;
            ret->width  = T.twidth;
            ret->colors = 3; 
            ret->bits   = 8;
            ret->gamma_corrected = 1;
            ret->data_size = T.tlength;
            memmove(ret->data,T.thumb,T.tlength);
            if(errcode) *errcode= 0;
            return ret;
        }
    else if (T.tformat == LIBRAW_THUMBNAIL_JPEG)
        {
            ushort exif[5];
            int mk_exif = 0;
            if(strcmp(T.thumb+6,"Exif")) mk_exif = 1;
            
            int dsize = T.tlength + mk_exif * (sizeof(exif)+sizeof(tiff_hdr));

            libraw_processed_image_t * ret = 
                (libraw_processed_image_t *)::malloc(sizeof(libraw_processed_image_t)+dsize);

            if(!ret)
                {
                    if(errcode) *errcode= ENOMEM;
                    return NULL;
                }

            bzero(ret,sizeof(libraw_processed_image_t));

            ret->type = LIBRAW_IMAGE_JPEG;
            ret->data_size = dsize;
            
            ret->data[0] = 0xff;
            ret->data[1] = 0xd8;
            if(mk_exif)
                {
                    struct tiff_hdr th;
                    memcpy (exif, "\xff\xe1  Exif\0\0", 10);
                    exif[1] = htons (8 + sizeof th);
                    memmove(ret->data+2,exif,sizeof(exif));
                    tiff_head (&th, 0);
                    memmove(ret->data+(2+sizeof(exif)),&th,sizeof(th));
                    memmove(ret->data+(2+sizeof(exif)+sizeof(th)),T.thumb+2,T.tlength-2);
                }
            else
                {
                    memmove(ret->data+2,T.thumb+2,T.tlength-2);
                }
            if(errcode) *errcode= 0;
            return ret;
            
        }
    else
        {
            if(errcode) *errcode= LIBRAW_UNSUPPORTED_THUMBNAIL;
            return NULL;

        }
}



libraw_processed_image_t *LibRaw::dcraw_make_mem_image(int *errcode)
{
    if((imgdata.progress_flags & LIBRAW_PROGRESS_THUMB_MASK) < LIBRAW_PROGRESS_PRE_INTERPOLATE)
            {
                if(errcode) *errcode= LIBRAW_OUT_OF_ORDER_CALL;
                return NULL;
            }

    if(!libraw_internal_data.output_data.histogram)
        {
            libraw_internal_data.output_data.histogram = 
                (int (*)[LIBRAW_HISTOGRAM_SIZE]) malloc(sizeof(*libraw_internal_data.output_data.histogram)*4);
            merror(libraw_internal_data.output_data.histogram,"LibRaw::dcraw_ppm_tiff_writer()");
        }

    unsigned ds = S.height * S.width * (O.output_bps/8) * P1.colors;
    libraw_processed_image_t *ret = (libraw_processed_image_t*)::malloc(sizeof(libraw_processed_image_t)+ds);
    if(!ret)
        {
                if(errcode) *errcode= ENOMEM;
                return NULL;
        }
    bzero(ret,sizeof(libraw_processed_image_t));
    // metadata init

    int s_iheight = S.iheight;
    int s_iwidth = S.iwidth;
    int s_width = S.width;
    int s_hwight = S.height;

    S.iheight = S.height;
    S.iwidth  = S.width;


    if (S.flip & 4) SWAP(S.height,S.width);


    ret->type   = LIBRAW_IMAGE_BITMAP;
    ret->height = S.height;
    ret->width  = S.width;
    ret->colors = P1.colors;
    ret->bits   = O.output_bps;
    ret->gamma_corrected = (O.output_bps == 8)?1:O.gamma_16bit;

    ret->data_size = ds;

    // Cut'n'paste from write_tiff_ppm, should be generalized later
    uchar *bufp = ret->data;
    uchar *ppm;
    ushort *ppm2,lut16[0x10000];
    int c, row, col, soff, rstep, cstep;


    if (ret->bits == 8 || ret->gamma_corrected ) gamma_lut (lut16);
    soff  = flip_index (0, 0);
    cstep = flip_index (0, 1) - soff;
    rstep = flip_index (1, 0) - flip_index (0, S.width);


    for (row=0; row < ret->height; row++, soff += rstep) 
        {
            ppm2 = (ushort*) (ppm = bufp);
            for (col=0; col < ret->width; col++, soff += cstep)
                if (ret->bits == 8)
                    FORCC ppm [col*ret->colors+c] = lut16[imgdata.image[soff][c]]/256;
                else if(ret->gamma_corrected) 
                    FORCC ppm2[col*ret->colors+c] =     lut16[imgdata.image[soff][c]];
                else 
                    FORCC ppm2[col*ret->colors+c] =     imgdata.image[soff][c];
            bufp+=ret->colors*(ret->bits/8)*ret->width;
        }
    if(errcode) *errcode= 0;

    S.iheight = s_iheight;
    S.iwidth = s_iwidth;
    S.width = s_width;
    S.height = s_hwight;

    return ret;
}

#undef FORC
#undef FORCC
#undef SWAP
#endif


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
    
    ushort *lut16 = (ushort*)calloc(0x10000,sizeof(ushort));
    merror(lut16,"LibRaw::kodak_thumb_loader()");
    gamma_lut(lut16);
    
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
                        ppm [col*P1.colors+c] = lut16[imgdata.image[soff][c]]/256;
            }
    }
    free(lut16);
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


// ������� thumbnail �� �����, ������ thumb_format � ������������ � ��������
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
	{
		fclose(tfp);
        	return LIBRAW_OUT_OF_ORDER_CALL;
	}

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
    CHECK_ORDER_HIGH(LIBRAW_PROGRESS_FUJI_ROTATE);
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

    try {
        if(O.half_size) 
            O.four_color_rgb = 1;

        if (IO.zero_is_bad) 
            {
                remove_zeroes();
                SET_PROC_FLAG(LIBRAW_PROGRESS_REMOVE_ZEROES);
            }
        if(O.bad_pixels) 
            {
                bad_pixels(O.bad_pixels);
                SET_PROC_FLAG(LIBRAW_PROGRESS_BAD_PIXELS);
            }
        if (O.dark_frame)
            {
                subtract (O.dark_frame);
                SET_PROC_FLAG(LIBRAW_PROGRESS_DARK_FRAME);
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
#ifndef NO_LCMS
	if(O.camera_profile)
            {
                apply_profile(O.camera_profile,O.output_profile);
                SET_PROC_FLAG(LIBRAW_PROGRESS_APPLY_PROFILE);
            }
#endif

        convert_to_rgb();
        SET_PROC_FLAG(LIBRAW_PROGRESS_CONVERT_RGB);

        if (O.use_fuji_rotate) 
            {
                stretch();
                SET_PROC_FLAG(LIBRAW_PROGRESS_STRETCH);
            }
        return 0;
    }
    catch ( LibRaw_exceptions err) {
        EXCEPTION_HANDLER(err);
    }
}

// Supported cameras:
static const char  *static_camera_list[] = 
{
"Adobe Digital Negative (DNG)",
"Apple QuickTake 100",
"Apple QuickTake 150",
"Apple QuickTake 200",
"AVT F-080C",
"AVT F-145C",
"AVT F-201C",
"AVT F-510C",
"AVT F-810C",
"Canon PowerShot 600",
"Canon PowerShot A5",
"Canon PowerShot A5 Zoom",
"Canon PowerShot A50",
"Canon PowerShot A460 (CHDK hack)",
"Canon PowerShot A530 (CHDK hack)",
"Canon PowerShot A610 (CHDK hack)",
"Canon PowerShot A620 (CHDK hack)",
"Canon PowerShot A630 (CHDK hack)",
"Canon PowerShot A640 (CHDK hack)",
"Canon PowerShot A650 (CHDK hack)",
"Canon PowerShot A710 IS (CHDK hack)",
"Canon PowerShot A720 IS (CHDK hack)",
"Canon PowerShot Pro70",
"Canon PowerShot Pro90 IS",
"Canon PowerShot G1",
"Canon PowerShot G2",
"Canon PowerShot G3",
"Canon PowerShot G5",
"Canon PowerShot G6",
"Canon PowerShot G7 (CHDK hack)",
"Canon PowerShot G9",
"Canon PowerShot G10",
"Canon PowerShot S2 IS (CHDK hack)",
"Canon PowerShot S3 IS (CHDK hack)",
"Canon PowerShot S5 IS (CHDK hack)",
"Canon PowerShot SD300 (CHDK hack)",
"Canon PowerShot S30",
"Canon PowerShot S40",
"Canon PowerShot S45",
"Canon PowerShot S50",
"Canon PowerShot S60",
"Canon PowerShot S70",
"Canon PowerShot Pro1",
"Canon EOS D30",
"Canon EOS D60",
"Canon EOS 5D",
"Canon EOS 5D Mark II",
"Canon EOS 10D",
"Canon EOS 20D",
"Canon EOS 30D",
"Canon EOS 40D",
"Canon EOS 50D",
"Canon EOS 300D / Digital Rebel / Kiss Digital",
"Canon EOS 350D / Digital Rebel XT / Kiss Digital N",
"Canon EOS 400D / Digital Rebel XTi / Kiss Digital X",
"Canon EOS 450D / Digital Rebel XSi / Kiss Digital X2",
"Canon EOS 1000D / Digital Rebel XS / Kiss Digital F",
"Canon EOS D2000C",
"Canon EOS-1D",
"Canon EOS-1DS",
"Canon EOS-1D Mark II",
"Canon EOS-1D Mark III",
"Canon EOS-1D Mark II N",
"Canon EOS-1Ds Mark II",
"Canon EOS-1Ds Mark III",
"Casio QV-2000UX",
"Casio QV-3000EX",
"Casio QV-3500EX",
"Casio QV-4000",
"Casio QV-5700",
"Casio QV-R41",
"Casio QV-R51",
"Casio QV-R61",
"Casio EX-S100",
"Casio EX-Z4",
"Casio EX-Z50",
"Casio EX-Z55",
"Casio Exlim Pro 505",
"Casio Exlim Pro 600",
"Casio Exlim Pro 700",
"Contax N Digital",
"Creative PC-CAM 600",
"Epson R-D1",
"Foculus 531C",
"Fuji FinePix E550",
"Fuji FinePix E900",
"Fuji FinePix F700",
"Fuji FinePix F710",
"Fuji FinePix F800",
"Fuji FinePix F810",
"Fuji FinePix S2Pro",
"Fuji FinePix S3Pro",
"Fuji FinePix S5Pro",
"Fuji FinePix S20Pro",
"Fuji FinePix S100FS",
"Fuji FinePix S5000",
"Fuji FinePix S5100/S5500",
"Fuji FinePix S5200/S5600",
"Fuji FinePix S6000fd",
"Fuji FinePix S7000",
"Fuji FinePix S9000/S9500",
"Fuji FinePix S9100/S9600",
"Fuji IS-1",
"Hasselblad CFV",
"Hasselblad H3D",
"Imacon Ixpress 16-megapixel",
"Imacon Ixpress 22-megapixel",
"Imacon Ixpress 39-megapixel",
"ISG 2020x1520",
"Kodak DC20 (see Oliver Hartman's page)",
"Kodak DC25 (see Jun-ichiro Itoh's page)",
"Kodak DC40",
"Kodak DC50",
"Kodak DC120 (also try kdc2tiff)",
"Kodak DCS200",
"Kodak DCS315C",
"Kodak DCS330C",
"Kodak DCS420",
"Kodak DCS460",
"Kodak DCS460A",
"Kodak DCS520C",
"Kodak DCS560C",
"Kodak DCS620C",
"Kodak DCS620X",
"Kodak DCS660C",
"Kodak DCS660M",
"Kodak DCS720X",
"Kodak DCS760C",
"Kodak DCS760M",
"Kodak EOSDCS1",
"Kodak EOSDCS3B",
"Kodak NC2000F",
"Kodak ProBack",
"Kodak PB645C",
"Kodak PB645H",
"Kodak PB645M",
"Kodak DCS Pro 14n",
"Kodak DCS Pro 14nx",
"Kodak DCS Pro SLR/c",
"Kodak DCS Pro SLR/n",
"Kodak C330",
"Kodak C603",
"Kodak P850",
"Kodak P880",
"Kodak KAI-0340",
"Konica KD-400Z",
"Konica KD-510Z",
"Leaf AFi 7",
"Leaf Aptus 17",
"Leaf Aptus 22",
"Leaf Aptus 54S",
"Leaf Aptus 65",
"Leaf Aptus 75",
"Leaf Aptus 75S",
"Leaf Cantare",
"Leaf CatchLight",
"Leaf CMost",
"Leaf DCB2",
"Leaf Valeo 6",
"Leaf Valeo 11",
"Leaf Valeo 17",
"Leaf Valeo 22",
"Leaf Volare",
"Leica Digilux 2",
"Leica Digilux 3",
"Leica D-LUX2",
"Leica D-LUX3",
"Leica D-LUX4",
"Leica V-LUX1",
"Logitech Fotoman Pixtura",
"Mamiya ZD",
"Micron 2010",
"Minolta RD175",
"Minolta DiMAGE 5",
"Minolta DiMAGE 7",
"Minolta DiMAGE 7i",
"Minolta DiMAGE 7Hi",
"Minolta DiMAGE A1",
"Minolta DiMAGE A2",
"Minolta DiMAGE A200",
"Minolta DiMAGE G400",
"Minolta DiMAGE G500",
"Minolta DiMAGE G530",
"Minolta DiMAGE G600",
"Minolta DiMAGE Z2",
"Minolta Alpha/Dynax/Maxxum 5D",
"Minolta Alpha/Dynax/Maxxum 7D",
"Nikon D1",
"Nikon D1H",
"Nikon D1X",
"Nikon D2H",
"Nikon D2Hs",
"Nikon D2X",
"Nikon D2Xs",
"Nikon D3",
"Nikon D40",
"Nikon D40X",
"Nikon D50",
"Nikon D60",
"Nikon D70",
"Nikon D70s",
"Nikon D80",
"Nikon D90",
"Nikon D100",
"Nikon D200",
"Nikon D300",
"Nikon D700",
"Nikon E700 (\"DIAG RAW\" hack)",
"Nikon E800 (\"DIAG RAW\" hack)",
"Nikon E880 (\"DIAG RAW\" hack)",
"Nikon E900 (\"DIAG RAW\" hack)",
"Nikon E950 (\"DIAG RAW\" hack)",
"Nikon E990 (\"DIAG RAW\" hack)",
"Nikon E995 (\"DIAG RAW\" hack)",
"Nikon E2100 (\"DIAG RAW\" hack)",
"Nikon E2500 (\"DIAG RAW\" hack)",
"Nikon E3200 (\"DIAG RAW\" hack)",
"Nikon E3700 (\"DIAG RAW\" hack)",
"Nikon E4300 (\"DIAG RAW\" hack)",
"Nikon E4500 (\"DIAG RAW\" hack)",
"Nikon E5000",
"Nikon E5400",
"Nikon E5700",
"Nikon E8400",
"Nikon E8700",
"Nikon E8800",
"Nikon Coolpix P6000",
"Nikon Coolpix S6 (\"DIAG RAW\" hack)",
"Nokia N95",
"Olympus C3030Z",
"Olympus C5050Z",
"Olympus C5060WZ",
"Olympus C7070WZ",
"Olympus C70Z,C7000Z",
"Olympus C740UZ",
"Olympus C770UZ",
"Olympus C8080WZ",
"Olympus E-1",
"Olympus E-3",
"Olympus E-10",
"Olympus E-20",
"Olympus E-300",
"Olympus E-330",
"Olympus E-400",
"Olympus E-410",
"Olympus E-420",
"Olympus E-500",
"Olympus E-510",
"Olympus E-520",
"Olympus SP310",
"Olympus SP320",
"Olympus SP350",
"Olympus SP500UZ",
"Olympus SP510UZ",
"Olympus SP550UZ",
"Olympus SP560UZ",
"Panasonic DMC-FZ8",
"Panasonic DMC-FZ18",
"Panasonic DMC-FZ28",
"Panasonic DMC-FZ30",
"Panasonic DMC-FZ50",
"Panasonic DMC-FX150",
"Panasonic DMC-G1",
"Panasonic DMC-L1",
"Panasonic DMC-L10",
"Panasonic DMC-LC1",
"Panasonic DMC-LX1",
"Panasonic DMC-LX2",
"Panasonic DMC-LX3",
"Pentax *ist D",
"Pentax *ist DL",
"Pentax *ist DL2",
"Pentax *ist DS",
"Pentax *ist DS2",
"Pentax K10D",
"Pentax K20D",
"Pentax K100D",
"Pentax K100D Super",
"Pentax K200D",
"Pentax Optio S",
"Pentax Optio S4",
"Pentax Optio 33WR",
"Pentax Optio 750Z",
"Phase One LightPhase",
"Phase One H 10",
"Phase One H 20",
"Phase One H 25",
"Phase One P 20",
"Phase One P 25",
"Phase One P 30",
"Phase One P 45",
"Pixelink A782",
"Polaroid x530",
"Rollei d530flex",
"RoverShot 3320af",
"Samsung GX-1S",
"Samsung GX-10",
"Sarnoff 4096x5440",
"Sigma SD9",
"Sigma SD10",
"Sigma SD14",
"Sinar 3072x2048",
"Sinar 4080x4080",
"Sinar 4080x5440",
"Sinar STI format",
"SMaL Ultra-Pocket 3",
"SMaL Ultra-Pocket 4",
"SMaL Ultra-Pocket 5",
"Sony DSC-F828",
"Sony DSC-R1",
"Sony DSC-V3",
"Sony DSLR-A100",
"Sony DSLR-A200",
"Sony DSLR-A300",
"Sony DSLR-A350",
"Sony DSLR-A700",
"Sony DSLR-A900",
"Sony XCD-SX910CR",
"STV680 VGA",
   NULL
};

const char** LibRaw::cameraList() { return static_camera_list;}
int LibRaw::cameraCount() { return (sizeof(static_camera_list)/sizeof(static_camera_list[0]))-1; }


const char * LibRaw::strprogress(enum LibRaw_progress p)
{
    switch(p)
        {
        case LIBRAW_PROGRESS_START:
            return "Starting";
        case LIBRAW_PROGRESS_OPEN :
            return "Opening file";
        case LIBRAW_PROGRESS_IDENTIFY :
            return "Reading metadata";
        case LIBRAW_PROGRESS_SIZE_ADJUST:
            return "Adjusting size";
        case LIBRAW_PROGRESS_LOAD_RAW:
            return "Reading RAW data";
        case LIBRAW_PROGRESS_REMOVE_ZEROES:
            return "Clearing zero values";
        case LIBRAW_PROGRESS_BAD_PIXELS :
            return "Removing dead pixels";
        case LIBRAW_PROGRESS_DARK_FRAME:
            return "Subtracting dark frame data";
        case LIBRAW_PROGRESS_FOVEON_INTERPOLATE:
            return "Interpolating Foveon sensor data";
        case LIBRAW_PROGRESS_SCALE_COLORS:
            return "Scaling colors";
        case LIBRAW_PROGRESS_PRE_INTERPOLATE:
            return "Pre-interpolating";
        case LIBRAW_PROGRESS_INTERPOLATE:
            return "Interpolating";
        case LIBRAW_PROGRESS_MIX_GREEN :
            return "Mixing green channels";
        case LIBRAW_PROGRESS_MEDIAN_FILTER   :
            return "Median filter";
        case LIBRAW_PROGRESS_HIGHLIGHTS:
            return "Highlight recovery";
        case LIBRAW_PROGRESS_FUJI_ROTATE :
            return "Rotating Fuji diagonal data";
        case LIBRAW_PROGRESS_FLIP :
            return "Flipping image";
        case LIBRAW_PROGRESS_APPLY_PROFILE:
            return "ICC conversion";
        case LIBRAW_PROGRESS_CONVERT_RGB:
            return "Converting to RGB";
        case LIBRAW_PROGRESS_STRETCH:
            return "Stretching image";
        case LIBRAW_PROGRESS_THUMB_LOAD:
            return "Loading thumbnail";
        default:
            return "Some strange things";
        }
}
