/* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.

iemlib written by Thomas Musil, Copyright (c) IEM KUG Graz Austria 2000 - 2018 */


#include "m_pd.h"
#include "iemlib.h"
#include <math.h>

/* ---------------- peakenv_AR~ - peak-envelope-converter with attack and release time. ----------------- */
/* -- now with double precision; for low-frequency filters it is important to calculate the filter in double precision -- */

typedef struct _peakenv_AR_tilde
{
  t_object x_obj;
  double   x_minus_mille_over_sr;
  double   x_old_peak;
  double   x_one_minus_c_a;
  double   x_c_r;
  double   x_attacktime;
  double   x_releasetime;
  t_float  x_float_sig_in;
} t_peakenv_AR_tilde;

static t_class *peakenv_AR_tilde_class;

static void peakenv_AR_tilde_reset(t_peakenv_AR_tilde *x)
{
  x->x_old_peak = 0.0;
}

static void peakenv_AR_tilde_ft1(t_peakenv_AR_tilde *x, t_floatarg f)/* attack-time in ms */
{
  if(f <= 0.0)
  {
    x->x_attacktime = 0.0;
    x->x_one_minus_c_a = 1.0;
  }
  else
  {
    x->x_attacktime = (double)f;
    x->x_one_minus_c_a = 1.0 - exp(x->x_minus_mille_over_sr / x->x_attacktime);
  }
}

static void peakenv_AR_tilde_ft2(t_peakenv_AR_tilde *x, t_floatarg f)/* release-time in ms */
{
  if(f <= 0.0)
  {
    x->x_releasetime = 0.0;
    x->x_c_r = 0.0;
  }
  else
  {
    x->x_releasetime = (double)f;
    x->x_c_r = exp(x->x_minus_mille_over_sr / x->x_releasetime);
  }
}

static t_int *peakenv_AR_tilde_perform(t_int *w)
{
  t_sample *in = (t_sample *)(w[1]);
  t_sample *out = (t_sample *)(w[2]);
  t_peakenv_AR_tilde *x = (t_peakenv_AR_tilde *)(w[3]);
  int n = (int)(w[4]);
  double old_peak = x->x_old_peak;
  double peak = x->x_old_peak;
  double one_minus_c_a = x->x_one_minus_c_a;
  double c_r = x->x_c_r;
  double absolute;
  int i;
  
  for(i=0; i<n; i++)
  {
    absolute = (double)fabs(*in++);
    if(absolute >= old_peak)
      peak = old_peak + (absolute - old_peak) * one_minus_c_a;
    else
      peak = old_peak * c_r;
    *out++ = (t_sample)peak;
    old_peak = peak;
  }
  /* NAN protect */
  if(IEM_DENORMAL_D(old_peak))
    old_peak = 0.0f;
  x->x_old_peak = old_peak;
  return(w+5);
}

static void peakenv_AR_tilde_dsp(t_peakenv_AR_tilde *x, t_signal **sp)
{
  x->x_minus_mille_over_sr = -1000.0 / (double)sp[0]->s_sr;
  peakenv_AR_tilde_ft1(x, x->x_attacktime);
  peakenv_AR_tilde_ft2(x, x->x_releasetime);
  dsp_add(peakenv_AR_tilde_perform, 4, sp[0]->s_vec, sp[1]->s_vec, x, sp[0]->s_n);
}

static void *peakenv_AR_tilde_new(t_floatarg f, t_floatarg g)
{
  t_peakenv_AR_tilde *x = (t_peakenv_AR_tilde *)pd_new(peakenv_AR_tilde_class);
  
  x->x_minus_mille_over_sr = -1000.0 / 44100.0;
  peakenv_AR_tilde_ft1(x, f);
  peakenv_AR_tilde_ft2(x, g);
  x->x_old_peak = 0.0;
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("ft1"));
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("ft2"));
  outlet_new(&x->x_obj, &s_signal);
  x->x_float_sig_in = 0.0;
  return(x);
}

void peakenv_AR_tilde_setup(void)
{
  peakenv_AR_tilde_class = class_new(gensym("peakenv_AR~"), (t_newmethod)peakenv_AR_tilde_new,
    0, sizeof(t_peakenv_AR_tilde), 0, A_DEFFLOAT, A_DEFFLOAT, 0);
  CLASS_MAINSIGNALIN(peakenv_AR_tilde_class, t_peakenv_AR_tilde, x_float_sig_in);
  class_addmethod(peakenv_AR_tilde_class, (t_method)peakenv_AR_tilde_dsp, gensym("dsp"), A_CANT, 0);
  class_addmethod(peakenv_AR_tilde_class, (t_method)peakenv_AR_tilde_ft1, gensym("ft1"), A_FLOAT, 0);
  class_addmethod(peakenv_AR_tilde_class, (t_method)peakenv_AR_tilde_ft2, gensym("ft2"), A_FLOAT, 0);
  class_addmethod(peakenv_AR_tilde_class, (t_method)peakenv_AR_tilde_reset, gensym("reset"), 0);
}
