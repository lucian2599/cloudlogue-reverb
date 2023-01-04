/*
 * File: delayline.cpp
 *
 * Test SDRAM memory i/o for delay lines
 *
 * 
 * 
 * 2018 (c) Korg
 *
 */

#include "userrevfx.h"

#include "eurorack/clouds/dsp/frame.h"
#include "eurorack/clouds/dsp/fx/reverb.h"

clouds::Reverb reverb;

static __sdram uint16_t buffer[16384];
static float reverb_amount = 0.f;
static float cutoff = 1.f;

/* Defines put here, until finding a way to make separate make targets 
   for each variant: original (no pan), pan, pan_hard. */
#define CLOUDLOGUE_REVERB_PAN
#define CLOUDLOGUE_REVERB_PAN_HARD

#if defined CLOUDLOGUE_REVERB_PAN_HARD
static float cnt_auto_pan_hard = 0.f;
/* These parameters were manually tuned, after several retries.
   32000 is since when I tried with short int which has max 32767. */
#define CNT_AUTO_PAN_HARD_MAX 1.f
#define CNT_INC (1.f/32000.f)
#define CNT_DEC (20.f/32000.f)
static short auto_pan_hard = 0;
/* 0.01f is roughly for L60 or R60 PAN threshold, at max LEVEL.
   Tested with Korg Volca Sample as audio input. */
static float audio_silent = 0.001f;
#endif

void *__dso_handle;

void REVFX_INIT(uint32_t platform, uint32_t api)
{
  reverb.Init(buffer);
}

#if defined CLOUDLOGUE_REVERB_PAN
static void revfx_process_internal(float *xn, uint32_t frames);

void REVFX_PROCESS(float *xn, uint32_t frames)
{
  float inL[64];
  float inR[64];

  /* Korg : Implementation must support at least up to 64 frames. */
  if(frames > 64)
    frames = 64;

#if !defined CLOUDLOGUE_REVERB_PAN_HARD
  for(uint32_t i = 0; i < frames; i++){
    /* Save left channel as dry bus. */
    inL[i] = xn[2*i];
    /* Set left channel same as right, for wet bus. */
    xn[2*i] = xn[2*i+1];
  }

#else
  for(uint32_t i = 0; i < frames; i++){
    /* Save left channel as dry bus. */
    inL[i] = xn[2*i];
    inR[i] = xn[2*i+1];

    if((inL[i] >  audio_silent || 
        inL[i] < -audio_silent) &&
       (inR[i] >  audio_silent || 
        inR[i] < -audio_silent))
    {
      if(cnt_auto_pan_hard > 0.f)
        cnt_auto_pan_hard -= CNT_DEC;
      else
        auto_pan_hard = 0;
    }
    else
    {
      //At least one channel is close to silence
      if(cnt_auto_pan_hard < CNT_AUTO_PAN_HARD_MAX)
        cnt_auto_pan_hard += CNT_INC;
      else
        auto_pan_hard = 1;
    }
#if 0
    if(!auto_pan_hard)
    {
      /* Set left channel same as right, for wet bus. */
      xn[2*i] = xn[2*i+1];
      //inR[i] = 0.f;
    }
    else{
      xn[2*i]   = 0.f;
      xn[2*i+1] = 0.f;      
    }
#endif

    if(1)//!auto_pan_hard)
    {
      xn[2*i]   = /*inL[i] * cnt_auto_pan_hard  +*/ inR[i] * (1.f - cnt_auto_pan_hard);
      xn[2*i+1] = xn[2*i];//inR[i] * cnt_auto_pan_hard;
    }
    else
    {
      xn[2*i]   = 0.f;
      xn[2*i+1] = 0.f;      
    }
  }
#endif

  revfx_process_internal(xn, frames);

#if !defined CLOUDLOGUE_REVERB_PAN_HARD
  for(uint32_t i = 0; i < frames; i++){
    /* Restore left inpput channel in output. */
    xn[2*i]   += inL[i];
    xn[2*i+1] += inL[i];
  }

#else
  for(uint32_t i = 0; i < frames; i++){
#if 0
    if(!auto_pan_hard)
    {
      /* Restore left inpput channel in output. */
      xn[2*i]   += inL[i];
      xn[2*i+1] += inL[i];
    }else{
      /* Restore as dry the hard-panned audio. */
      xn[2*i]   += inL[i];
      xn[2*i+1] += inR[i];      
    }
#endif
    if(1)//!auto_pan_hard)
    {
      xn[2*i]   += inL[i];
      xn[2*i+1] += inL[i] * (1.f - cnt_auto_pan_hard)  + inR[i] * cnt_auto_pan_hard;
    }
    else
    {
      /* Restore as dry the hard-panned audio. */
      xn[2*i]   += inL[i];
      xn[2*i+1] += inR[i];      
    }
  }
#endif
}

static void revfx_process_internal(float *xn, uint32_t frames)
#else
void REVFX_PROCESS(float *xn, uint32_t frames)
#endif
{
  clouds::FloatFrame *out = reinterpret_cast<clouds::FloatFrame*>(xn);


  reverb.set_amount(reverb_amount * 0.54f);
  reverb.set_diffusion(0.7f);
  reverb.set_time(0.35f + 0.63f * reverb_amount);
  reverb.set_input_gain(0.2f);
  reverb.set_lp(0.6f + 0.37f * cutoff);
  reverb.Process(out, frames);
}

void REVFX_PARAM(uint8_t index, int32_t value)
{
  const float valf = q31_to_f32(value);
  switch (index) {
  case k_user_revfx_param_time:
    cutoff = valf;
    break;
  case k_user_revfx_param_depth:
    reverb_amount = valf;
    break;
  case k_user_revfx_param_shift_depth:
    break;
  default:
    break;
  }
}
