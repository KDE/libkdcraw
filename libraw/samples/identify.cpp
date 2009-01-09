/* -*- C++ -*-
 * File: identify.cpp
 * Copyright 2008-2009 Alex Tutubalin <lexa@lexa.ru>
 * Created: Sat Mar  8 , 2008
 *
 * LibRaw C++ demo (emulates dcraw -i [-v])
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
#include <time.h>

#include "libraw/libraw.h"

#ifdef WIN32
#define snprintf _snprintf
#endif


int main(int ac, char *av[])
{
    int verbose = 0, ret,print_unpack=0,print_frame=0;
    LibRaw MyCoolRawProcessor;

    for (int i=1;i<ac;i++) {
        if(av[i][0]=='-')
            {
                if(av[i][1]=='v' && av[i][2]==0) verbose++;
                if(av[i][1]=='u' && av[i][2]==0) print_unpack++;
                if(av[i][1]=='f' && av[i][2]==0) print_frame++;
                continue;
            }
        if( (ret = MyCoolRawProcessor.open_file(av[i])) != LIBRAW_SUCCESS)
            {
                printf("Cannot decode %s: %s\n",av[i],libraw_strerror(ret));
                continue; // no recycle, open_file will recycle
            }
        if(verbose) {

#define P1 MyCoolRawProcessor.imgdata.idata
#define P2 MyCoolRawProcessor.imgdata.other

#define S MyCoolRawProcessor.imgdata.sizes
#define O MyCoolRawProcessor.imgdata.params
#define C MyCoolRawProcessor.imgdata.color
#define T MyCoolRawProcessor.imgdata.thumbnail


            if( (ret =  MyCoolRawProcessor.adjust_sizes_info_only()))
            {
                printf("Cannot decode %s: %s\n",av[i],libraw_strerror(ret));
                continue; // no recycle, open_file will recycle
            }
            
            printf ("\nFilename: %s\n", av[i]);
            printf ("Timestamp: %s", ctime(&(P2.timestamp)));
            printf ("Camera: %s %s\n", P1.make, P1.model);
            if (P2.artist[0])
                printf ("Owner: %s\n", P2.artist);
            if (P1.dng_version) {
                printf ("DNG Version: ");
                for (int i=24; i >= 0; i -= 8)
                    printf ("%d%c", P1.dng_version >> i & 255, i ? '.':'\n');
            }
            printf ("ISO speed: %d\n", (int) P2.iso_speed);
            printf ("Shutter: ");
            if (P2.shutter > 0 && P2.shutter < 1)
                P2.shutter = (printf ("1/"), 1 / P2.shutter);
            printf ("%0.1f sec\n", P2.shutter);
            printf ("Aperture: f/%0.1f\n", P2.aperture);
            printf ("Focal length: %0.1f mm\n", P2.focal_len);
            if(C.profile)
                printf ("Embedded ICC profile: yes, %d bytes\n", C.profile_length);
            else
                printf ("Embedded ICC profile: no\n", C.profile_length);
                
            printf ("Number of raw images: %d\n", P1.raw_count);
            if (S.pixel_aspect != 1)
                printf ("Pixel Aspect Ratio: %0.6f\n", S.pixel_aspect);
            if (T.tlength)
                printf ("Thumb size:  %4d x %d\n", T.twidth, T.theight);
            printf ("Full size:   %4d x %d\n", S.raw_width, S.raw_height);
            
            printf ("Image size:  %4d x %d\n", S.width, S.height);
            printf ("Output size: %4d x %d\n", S.iwidth, S.iheight);
            printf ("Raw colors: %d", P1.colors);
            if (P1.filters) 
                {
                    printf ("\nFilter pattern: ");
                    if (!P1.cdesc[3]) P1.cdesc[3] = 'G';
                    for (int i=0; i < 16; i++)
                        putchar (P1.cdesc[MyCoolRawProcessor.fc(i >> 1,i & 1)]);
                }
            printf ("\nDaylight multipliers:");
            for(int c=0;c<P1.colors;c++) printf (" %f", C.pre_mul[c]);
            if (C.cam_mul[0] > 0) 
                {
                    printf ("\nCamera multipliers:");
                    for(int c=0;c<4;c++) printf (" %f", C.cam_mul[c]);
                }
            char *csl[] = {"U","I","CO","L","CA"};
            printf("\nColor sources /Legend: (U)nknown, (I)nit, (CO)nstant, (L)oaded, (CA)lculated/:\n");
            printf("\tcurve=%s; rgb_cam=%s; cmatrix=%s, pre_mul=%s, cam_mul=%s",
                   csl[C.color_flags.curve_state],csl[C.color_flags.rgb_cam_state],
                   csl[C.color_flags.cmatrix_state],csl[C.color_flags.pre_mul_state],
                   csl[C.color_flags.cam_mul_state]);
            putchar ('\n');
            printf("Cam->XYZ matrix:\n");
            for(int i=0; i< 4; i++)
                printf("%6.4f\t%6.4f\t%6.4f\n",C.cam_xyz[i][0],C.cam_xyz[i][1],C.cam_xyz[i][2]);
        }
        else 
            {
                if(print_unpack)
                    {
                        char frame[32]="";
                        if(print_frame)
                            snprintf(frame,32,"%dx%dx%dx%d",S.left_margin,S.top_margin,S.right_margin,S.bottom_margin);
                        printf ("%s\t%s\t%s\t%s/%s\n", 
                                av[i],
                                MyCoolRawProcessor.unpack_function_name(),
                                frame,
                                P1.make, P1.model);
                    }
                else
                    printf ("%s is a %s %s image.\n", av[i],P1.make, P1.model);
            }
        MyCoolRawProcessor.recycle();
    }// endfor
    return 0;
}
