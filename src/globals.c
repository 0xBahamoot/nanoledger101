#include "os.h"
#include "ux.h"
#include "globals.h"

ux_state_t G_ux;
bolos_ux_params_t G_ux_params;

// display stepped screens
unsigned int ux_step;
unsigned int ux_step_count;
unsigned char processData[256];

const internalStorage_t N_storage_real;

crypto_state_t G_crypto_state_t;
// io_state_t G_io_state_t;