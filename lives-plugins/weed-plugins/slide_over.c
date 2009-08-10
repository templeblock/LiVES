// simple_blend.c
// weed plugin
// (c) G. Finch (salsaman) 2005
//
// released under the GNU GPL 3 or later
// see file COPYING or www.gnu.org for details

#include "../../libweed/weed.h"
#include "../../libweed/weed-effects.h"
#include "../../libweed/weed-plugin.h"

///////////////////////////////////////////////////////////////////

static int num_versions=1; // number of different weed api versions supported
static int api_versions[]={100}; // array of weed api versions supported in plugin, in order of preference (most preferred first)

static int package_version=1; // version of this package

//////////////////////////////////////////////////////////////////

#include "../../libweed/weed-utils.h" // optional
#include "../../libweed/weed-plugin-utils.h" // optional

/////////////////////////////////////////////////////////////


static volatile uint32_t fastrand_val;

static inline uint32_t fastrand(void)
{
#define rand_a 1073741789L
#define rand_c 32749L

  return (fastrand_val=((fastrand_val*=rand_a) + rand_c));
}


inline int pick_direction(int dirpref, weed_timecode_t tc) {
  if (dirpref!=0) return dirpref;
  fastrand_val=tc&0xFFFFFFFF;
  return ((fastrand()>>24)&0x03)+1;
}



int sover_init (weed_plant_t *inst) {
  weed_set_int_value(inst,"plugin_direction",-1);
  return WEED_NO_ERROR;
}



int sover_process (weed_plant_t *inst, weed_timecode_t timecode) {
  int error;
  weed_plant_t **in_channels=weed_get_plantptr_array(inst,"in_channels",&error),*out_channel=weed_get_plantptr_value(inst,"out_channels",&error);
  unsigned char *src1=weed_get_voidptr_value(in_channels[0],"pixel_data",&error);
  unsigned char *src2=weed_get_voidptr_value(in_channels[1],"pixel_data",&error);
  unsigned char *dst=weed_get_voidptr_value(out_channel,"pixel_data",&error);
  int width=weed_get_int_value(in_channels[0],"width",&error);
  int height=weed_get_int_value(in_channels[0],"height",&error);
  int irowstride1=weed_get_int_value(in_channels[0],"rowstrides",&error);
  int irowstride2=weed_get_int_value(in_channels[1],"rowstrides",&error);
  int orowstride=weed_get_int_value(out_channel,"rowstrides",&error);
  weed_plant_t **in_params;

  register int j;

  int transval;
  int dirpref,dirn;
  int mvlower;
  int bound;

  in_params=weed_get_plantptr_array(inst,"in_parameters",&error);
  transval=weed_get_int_value(in_params[0],"value",&error);

  if (weed_get_boolean_value(in_params[1],"value",&error)==WEED_TRUE) dirpref=0; // random
  else if (weed_get_boolean_value(in_params[2],"value",&error)==WEED_TRUE) dirpref=1; // left to right
  else if (weed_get_boolean_value(in_params[3],"value",&error)==WEED_TRUE) dirpref=2; // right to left
  else if (weed_get_boolean_value(in_params[4],"value",&error)==WEED_TRUE) dirpref=3; // top to bottom
  else dirpref=4; // bottom to top

  dirn=weed_get_int_value(inst,"plugin_direction",&error);

  if (dirn==-1||(dirpref==0&&(transval==0||transval==255))) {
    dirn=pick_direction(dirpref,timecode);
    weed_set_int_value(inst,"plugin_direction",dirn);
  }

  mvlower=weed_get_boolean_value(in_params[6],"value",&error);

  switch (dirn) {
  case 3:
    // top to bottom
    bound=(float)height*(1.-transval/255.);
    src1+=irowstride1*(height-bound);
    for (j=0;j<bound;j++) {
      weed_memcpy(dst,src1,width*3);
      src1+=irowstride1;
      if (!mvlower) src2+=irowstride2;
      dst+=orowstride;
    }
    for (j=bound;j<height;j++) {
      weed_memcpy(dst,src2,width*3);
      src2+=irowstride2;
      dst+=orowstride;
    }
    break;
  case 4:
    // bottom to top
    bound=(float)height*(transval/255.);
    if (mvlower) src2+=irowstride2*(height-bound);
    for (j=0;j<bound;j++) {
      weed_memcpy(dst,src2,width*3);
      src2+=irowstride2;
      dst+=orowstride;
    }
    for (j=bound;j<height;j++) {
      weed_memcpy(dst,src1,width*3);
      src1+=irowstride1;
      dst+=orowstride;
    }
    break;
  case 1:
    // left to right
    bound=(float)width*(1.-transval/255.);
    for (j=0;j<height;j++) {
      weed_memcpy(dst+bound*3,src2+bound*3*!mvlower,(width-bound)*3);
      weed_memcpy(dst,src1+(width-bound)*3,bound*3);
      src1+=irowstride1;
      src2+=irowstride2;
      dst+=orowstride;
    }
    break;
  case 2:
    // right to left
    bound=(float)width*(transval/255.);
    for (j=0;j<height;j++) {
      weed_memcpy(dst,src2+(width-bound)*3*mvlower,bound*3);
      weed_memcpy(dst+bound*3,src1,(width-bound)*3);
      src1+=irowstride1;
      src2+=irowstride2;
      dst+=orowstride;
    }
    break;
  }

  weed_free(in_params);
  weed_free(in_channels);
  return WEED_NO_ERROR;
}



weed_plant_t *weed_setup (weed_bootstrap_f weed_boot) {
  weed_plant_t *plugin_info=weed_plugin_info_init(weed_boot,num_versions,api_versions);
  if (plugin_info!=NULL) {
    int palette_list[]={WEED_PALETTE_BGR24,WEED_PALETTE_RGB24,WEED_PALETTE_END};
    weed_plant_t *in_chantmpls[]={weed_channel_template_init("in channel 0",0,palette_list),weed_channel_template_init("in channel 1",0,palette_list),NULL};
    weed_plant_t *out_chantmpls[]={weed_channel_template_init("out channel 0",0,palette_list),NULL};
    weed_plant_t *in_params[]={weed_integer_init("amount","Transition _value",128,0,255),weed_radio_init("dir_rand","_Random",1,1),weed_radio_init("dir_l2r","_Left to right",0,1),weed_radio_init("dir_r2l","_Right to left",0,1),weed_radio_init("dir_t2b","_Top to bottom",0,1),weed_radio_init("dir_b2t","_Bottom to top",0,1),weed_switch_init("mlower","_Move lower clip",0),NULL};

    weed_plant_t *filter_class=weed_filter_class_init("slide over","salsaman",1,0,&sover_init,&sover_process,NULL,in_chantmpls,out_chantmpls,in_params,NULL);

    weed_plant_t *gui=weed_filter_class_get_gui(filter_class);
    char *rfx_strings[]={"layout|p0|","layout|hseparator|","layout|fill|\"Slide direction\"|fill|","layout|p1|","layout|p2|p3|","layout|p4|p5|","layout|hseparator|"};

    weed_set_string_value(gui,"layout_scheme","RFX");
    weed_set_string_value(gui,"rfx_delim","|");
    weed_set_string_array(gui,"rfx_strings",7,rfx_strings);

    weed_set_boolean_value(in_params[0],"transition",WEED_TRUE);

    weed_plugin_info_add_filter_class (plugin_info,filter_class);

    weed_set_int_value(plugin_info,"version",package_version);

  }
  return plugin_info;
}
