/* WEED is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   Weed is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this source code; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


   Weed is developed by:

   Gabriel "Salsaman" Finch - http://lives.sourceforge.net

   mainly based on LiViDO, which is developed by:


   Niels Elburg - http://veejay.sf.net

   Gabriel "Salsaman" Finch - http://lives.sourceforge.net

   Denis "Jaromil" Rojo - http://freej.dyne.org

   Tom Schouten - http://zwizwa.fartit.com

   Andraz Tori - http://cvs.cinelerra.org

   reviewed with suggestions and contributions from:

   Silvano "Kysucix" Galliani - http://freej.dyne.org

   Kentaro Fukuchi - http://megaui.net/fukuchi

   Jun Iio - http://www.malib.net

   Carlo Prelz - http://www2.fluido.as:8080/

*/

/* (C) Gabriel "Salsaman" Finch, 2005 - 2009 */

#ifndef __WEED_EFFECTS_H__
#define __WEED_EFFECTS_H__

#ifndef __WEED_H__
#include <weed/weed.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  /* API version * 120 */
#define WEED_API_VERSION 120
#define WEED_API_VERSION_100
#define WEED_API_VERSION_110
#define WEED_API_VERSION_120

  /* plant types */
#define WEED_PLANT_PLUGIN_INFO 1
#define WEED_PLANT_FILTER_CLASS 2
#define WEED_PLANT_FILTER_INSTANCE 3
#define WEED_PLANT_CHANNEL_TEMPLATE 4
#define WEED_PLANT_PARAMETER_TEMPLATE 5
#define WEED_PLANT_CHANNEL 6
#define WEED_PLANT_PARAMETER 7
#define WEED_PLANT_GUI 8
#define WEED_PLANT_HOST_INFO 255

/* Palette types */
/* RGB palettes */
#define WEED_PALETTE_END 0
#define WEED_PALETTE_RGB888 1
#define WEED_PALETTE_RGB24 1
#define WEED_PALETTE_BGR888 2
#define WEED_PALETTE_BGR24 2
#define WEED_PALETTE_RGBA8888 3
#define WEED_PALETTE_RGBA32 3
#define WEED_PALETTE_ARGB8888 4
#define WEED_PALETTE_ARGB32 4
#define WEED_PALETTE_RGBFLOAT 5
#define WEED_PALETTE_RGBAFLOAT  6
#define WEED_PALETTE_BGRA8888 7
#define WEED_PALETTE_BGRA32 7

/* YUV palettes */
#define WEED_PALETTE_YUV422P 513
#define WEED_PALETTE_YV16 513
#define WEED_PALETTE_YUV420P 514
#define WEED_PALETTE_YV12 514
#define WEED_PALETTE_YVU420P 515
#define WEED_PALETTE_I420 515
#define WEED_PALETTE_IYUV 515
#define WEED_PALETTE_YUV444P 516
#define WEED_PALETTE_YUVA4444P 517
#define WEED_PALETTE_YUYV8888 518
#define WEED_PALETTE_YUYV 518
#define WEED_PALETTE_YUY2 518
#define WEED_PALETTE_UYVY8888 519
#define WEED_PALETTE_UYVY 519
#define WEED_PALETTE_YUV411 520
#define WEED_PALETTE_IYU1 520
#define WEED_PALETTE_YUV888 521
#define WEED_PALETTE_IYU2 521
#define WEED_PALETTE_YUVA8888 522

/* Alpha palettes */
#define WEED_PALETTE_A1 1025
#define WEED_PALETTE_A8 1026
#define WEED_PALETTE_AFLOAT 1027

/* Parameter hints */
#define WEED_HINT_UNSPECIFIED     0
#define WEED_HINT_INTEGER         1
#define WEED_HINT_FLOAT           2
#define WEED_HINT_TEXT            3
#define WEED_HINT_SWITCH          4
#define WEED_HINT_COLOR           5

/* Colorspaces for Color parameters */
#define WEED_COLORSPACE_RGB   1
#define WEED_COLORSPACE_RGBA  2
#define WEED_COLORSPACE_HSV   3

/* Filter flags */
#define WEED_FILTER_NON_REALTIME    (1<<0)
#define WEED_FILTER_IS_CONVERTOR    (1<<1)
#define WEED_FILTER_HINT_IS_STATELESS (1<<2)
#define WEED_FILTER_HINT_IS_POINT_EFFECT (1<<3)

/* Channel flags */
#define WEED_CHANNEL_REINIT_ON_SIZE_CHANGE    (1<<0)
#define WEED_CHANNEL_REINIT_ON_PALETTE_CHANGE (1<<1)
#define WEED_CHANNEL_CAN_DO_INPLACE           (1<<2)
#define WEED_CHANNEL_SIZE_CAN_VARY            (1<<3)
#define WEED_CHANNEL_PALETTE_CAN_VARY         (1<<4)

/* API version 110 */
#define WEED_CHANNEL_FOLLOWS_OUTPUT           (1<<5)

/* Parameter flags */
#define WEED_PARAMETER_REINIT_ON_VALUE_CHANGE (1<<0)
#define WEED_PARAMETER_VARIABLE_ELEMENTS      (1<<1)

/* API version 110 */
#define WEED_PARAMETER_ELEMENT_PER_CHANNEL    (1<<2)

/* YUV sampling types */
#define WEED_YUV_SAMPLING_DEFAULT   0
#define WEED_YUV_SAMPLING_MPEG  0
#define WEED_YUV_SAMPLING_JPEG   1
#define WEED_YUV_SAMPLING_DVPAL  2
#define WEED_YUV_SAMPLING_DVNTSC  3

/* YUV clamping types */
#define WEED_YUV_CLAMPING_CLAMPED 0
#define WEED_YUV_CLAMPING_UNCLAMPED 1

/* YUV subspace types */
#define WEED_YUV_SUBSPACE_YCBCR 0
#define WEED_YUV_SUBSPACE_YUV 1
#define WEED_YUV_SUBSPACE_BT709 2


/* Plugin errors */
#define WEED_ERROR_TOO_MANY_INSTANCES 6
#define WEED_ERROR_HARDWARE 7
#define WEED_ERROR_INIT_ERROR 8
#define WEED_ERROR_PLUGIN_INVALID 64

  /* host bootstrap function */
typedef weed_plant_t *(*weed_bootstrap_f) (weed_default_getter_f *value, int num_versions, int *plugin_versions);

  /* plugin only functions */
typedef weed_plant_t *(*weed_setup_f)(weed_bootstrap_f weed_boot);
typedef int (*weed_init_f) (weed_plant_t *filter_instance);
typedef int (*weed_process_f) (weed_plant_t *filter_instance, weed_timecode_t timestamp);
typedef int (*weed_deinit_f) (weed_plant_t *filter_instance);

  /* special plugin functions */
typedef void (*weed_display_f)(weed_plant_t *parameter);
typedef int (*weed_interpolate_f)(weed_plant_t **in_values, weed_plant_t *out_value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // #ifndef __WEED_EFFECTS_H__
