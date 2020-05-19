/* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.

iemlib written by Thomas Musil, Copyright (c) IEM KUG Graz Austria 2000 - 2018 */


#include "m_pd.h"
#include "iemlib.h"

static t_class *iemlib_class;

static void *iemlib_new(void)
{
  t_object *x = (t_object *)pd_new(iemlib_class);
  
  return (x);
}

void biquad_freq_resp_setup(void);
void db2v_setup(void);
void f2note_setup(void);
void filter_tilde_setup(void);
void FIR_tilde_setup(void);
void forpp_setup(void);
void gate_setup(void);
void hml_shelf_tilde_setup(void);
void iem_cot4_tilde_setup(void);
void iem_delay_tilde_setup(void);
void iem_pow4_tilde_setup(void);
void iem_sqrt4_tilde_setup(void);
void lp1_t_tilde_setup(void);
void mov_avrg_kern_tilde_setup(void);
void para_bp2_tilde_setup(void);
void peakenv_tilde_setup(void);
void peakenv_AR_tilde_setup(void);
void peakenv_hold_tilde_setup(void);
void prvu_tilde_setup(void);
void pvu_tilde_setup(void);
void rvu_tilde_setup(void);
void sin_freq_tilde_setup(void);
void sin_phase_tilde_setup(void);
void sparse_FIR_tilde_setup(void);
void soundfile_info_setup(void);
void split_setup(void);
void v2db_setup(void);
void vcf_filter_tilde_setup(void);

void add2_comma_setup(void);
void aspeedlim_setup(void);
void bpe_setup(void);
void dollarg_setup(void);
void exp_inc_setup(void);
void fade_tilde_setup(void);
void float24_setup(void);
void iem_alisttosym_setup(void);
void iem_anything_setup(void);
void iem_append_setup(void);
void iem_blocksize_tilde_setup(void);
void iem_i_route_setup(void);
void iem_pbank_csv_setup(void);
void iem_prepend_setup(void);
void iem_receive_setup(void);
void iem_route_setup(void);
void iem_samplerate_tilde_setup(void);
void iem_sel_any_setup(void);
void iem_send_setup(void);
void iem_symtoalist_setup(void);
void init_setup(void);
void LFO_noise_tilde_setup(void);
void list2send_setup(void);
void m2f_tilde_setup(void);
void mergefilename_setup(void);
void modulo_counter_setup(void);
void parentdollarzero_setup(void);
void post_netreceive_setup(void);
void pre_inlet_setup(void);
void prepend_ascii_setup(void);
void protect_against_open_setup(void);
void receive2list_setup(void);
void round_zero_setup(void);
void speedlim_setup(void);
void splitfilename_setup(void);
void stripfilename_setup(void);
void toggle_mess_setup(void);
void transf_fader_setup(void);
void unsymbol_setup(void);
void wrap_setup(void);

void t3_bpe_setup(void);
void t3_delay_setup(void);
void t3_line_tilde_setup(void);
void t3_metro_setup(void);
void t3_sig_tilde_setup(void);
void t3_timer_setup(void);

/* ------------------------ setup routine ------------------------- */

void iemlib_setup(void)
{
  iemlib_class = class_new(gensym("iemlib"), iemlib_new, 0,
    sizeof(t_object), CLASS_NOINLET, 0);
  
  biquad_freq_resp_setup();
  db2v_setup();
  f2note_setup();
  filter_tilde_setup();
  FIR_tilde_setup();
  forpp_setup();
  gate_setup();
  hml_shelf_tilde_setup();
  iem_cot4_tilde_setup();
  iem_delay_tilde_setup();
  iem_pow4_tilde_setup();
  iem_sqrt4_tilde_setup();
  lp1_t_tilde_setup();
  mov_avrg_kern_tilde_setup();
  para_bp2_tilde_setup();
  peakenv_tilde_setup();
  peakenv_AR_tilde_setup();
  peakenv_hold_tilde_setup();
  prvu_tilde_setup();
  pvu_tilde_setup();
  rvu_tilde_setup();
  sin_freq_tilde_setup();
  sin_phase_tilde_setup();
  sparse_FIR_tilde_setup();
  soundfile_info_setup();
  split_setup();
  v2db_setup();
  vcf_filter_tilde_setup();

  add2_comma_setup();
  aspeedlim_setup();
  bpe_setup();
  dollarg_setup();
  exp_inc_setup();
  fade_tilde_setup();
  float24_setup();
  iem_alisttosym_setup();
  iem_anything_setup();
  iem_append_setup();
  iem_blocksize_tilde_setup();
  iem_i_route_setup();
  iem_pbank_csv_setup();
  iem_prepend_setup();
  iem_receive_setup();
  iem_route_setup();
  iem_samplerate_tilde_setup();
  iem_sel_any_setup();
  iem_send_setup();
  iem_symtoalist_setup();
  init_setup();
  LFO_noise_tilde_setup();
  list2send_setup();
  m2f_tilde_setup();
  mergefilename_setup();
  modulo_counter_setup();
  parentdollarzero_setup();
  post_netreceive_setup();
  pre_inlet_setup();
  prepend_ascii_setup();
  protect_against_open_setup();
  receive2list_setup();
  round_zero_setup();
  speedlim_setup();
  splitfilename_setup();
  stripfilename_setup();
  toggle_mess_setup();
  transf_fader_setup();
  unsymbol_setup();
  wrap_setup();

  t3_bpe_setup();
  t3_delay_setup();
  t3_line_tilde_setup();
  t3_metro_setup();
  t3_sig_tilde_setup();
  t3_timer_setup();

  post("iemlib (1.22-1) library loaded!   (c) Thomas Musil "BUILD_DATE);
	post("   musil%ciem.at iem KUG Graz Austria", '@');
}
