
#ifndef INCOGNITO_VARS_H
#define INCOGNITO_VARS_H

#include "os.h"
#include "cx.h"
#include "os_io_seproxyhal.h"
#include "incognito_types.h"
#include "incognito_api.h"

extern incognito_v_state_t G_incognito_vstate;

extern incognito_nv_state_t N_state_pic;
#define N_incognito_pstate ((WIDE incognito_nv_state_t *)PIC(&N_state_pic))

extern ux_state_t ux;
#endif
