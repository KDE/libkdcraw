/* -*- C++ -*-
 * File: docmode_withmask.cpp
 * Copyright 2009 Alex Tutubalin <lexa@lexa.ru>
 * Created: Fri Jan 02, 2009
 *
 * LibRaw sample
 * Generates RAW .pgm file with masked pixels included
 * and all processing turned off
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef WIN32
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif

#include "libraw/libraw.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

int main(int ac, char *av[])
{
    int  i, ret;
    int verbose=1,autoscale=0;
    char outfn[1024],thumbfn[1024]; 

    LibRaw RawProcessor;
    if(ac<2) 
        {
            printf(
                "docmode_withmask - LibRaw %s sample. %d cameras supported\n"
                "Usage: %s [-q] [-S] [-g] raw-files....\n"
                "\t-q - be quiet\n"
                "\t-g - use gamma correction with gamma 2.2 (not precise,use for visual inspection only)\n"
                "\t-A - autoscaling (by integer factor)\n"
                ,LibRaw::version(),
                LibRaw::cameraCount(),
                av[0]);
            return 0;
        }
    
#define P1 RawProcessor.imgdata.idata
#define S RawProcessor.imgdata.sizes
#define C RawProcessor.imgdata.color
#define T RawProcessor.imgdata.thumbnail
#define P2 RawProcessor.imgdata.other
#define OUT RawProcessor.imgdata.params

    OUT.document_mode=2;
    OUT.output_bps=16;
    OUT.output_tiff=1;
    OUT.user_flip=0;
    OUT.no_auto_bright = 1;
    OUT.filtering_mode=LIBRAW_FILTERING_NONE;
    for (i=1;i<ac;i++)
        {
            if(av[i][0]=='-')
                {
                    if(av[i][1]=='q' && av[i][2]==0)
                        verbose=0;
                    if(av[i][1]=='A' && av[i][2]==0)
                        autoscale=1;
                    if(av[i][1]=='g' && av[i][2]==0)
                        OUT.gamma_16bit=1;
                    continue;
                }
            int r,c;
            if(verbose) printf("Processing file %s\n",av[i]);
            if( (ret = RawProcessor.open_file(av[i])) != LIBRAW_SUCCESS)
                {
                    fprintf(stderr,"Cannot open %s: %s\n",av[i],libraw_strerror(ret));
                    continue; // no recycle b/c open file will recycle itself
                }
            if(verbose)
                {
                    printf("Image size: %dx%d\nRaw size: %dx%d\n",S.width,S.height,S.raw_width,S.raw_height);
                    printf("Margins: top=%d, left=%d, right=%d, bottom=%d\n",
                           S.top_margin,S.left_margin,S.right_margin,S.bottom_margin);
                }

            if( (ret = RawProcessor.unpack() ) != LIBRAW_SUCCESS)
                {
                    fprintf(stderr,"Cannot unpack %s: %s\n",av[i],libraw_strerror(ret));
                    continue;
                }
            if(verbose)
                printf("Unpacked....\n");

            if( (ret = RawProcessor.add_masked_borders_to_bitmap() ) != LIBRAW_SUCCESS)
                {
                    fprintf(stderr,"Cannot add mask data to bitmap %s\n",av[i]);
                    continue;
                }
            for(int r=0;r<S.raw_height;r++)
                for(c=0;c<S.raw_width;c++)
                    RawProcessor.imgdata.image[r*S.raw_width+c][0] 
                        = RawProcessor.imgdata.image[r*S.raw_width+c][RawProcessor.FC(r,c)];

            P1.colors=1;
            if(autoscale)
                {
                    unsigned max=0,scale;
                    for(int j=0; j<S.raw_height*S.raw_width; j++)
                        if(max < RawProcessor.imgdata.image[j][0])
                            max = RawProcessor.imgdata.image[j][0]; 
                    if (max >0 && max< 1<<15)
                        {
                            scale = (1<<16)/max;
                            if(verbose)
                                printf("Scaling with multiplier=%d (max=%d)\n",scale,max);
                            
                            for(int j=0; j<S.raw_height*S.raw_width; j++)
                                RawProcessor.imgdata.image[j][0] *= scale;
                        }
                }

            snprintf(outfn,sizeof(outfn),"%s.tiff",av[i]);

            if(verbose) printf("Writing file %s\n",outfn);
            if( LIBRAW_SUCCESS != (ret = RawProcessor.dcraw_ppm_tiff_writer(outfn)))
                fprintf(stderr,"Cannot write %s: %s\n",outfn,libraw_strerror(ret));
            RawProcessor.recycle(); // just for show this call
        }
    return 0;
}
