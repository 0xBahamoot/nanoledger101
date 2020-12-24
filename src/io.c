#include "io.h"
#include "globals.h"

void incognito_io_clean() {
    G_io_state_t.io_length = 0;
    G_io_state_t.io_offset = 0;
    os_memset(G_io_state_t.io_buffer, 0, INCOGNITO_IO_BUFFER_LENGTH);
    }

void incognito_io_send(unsigned int io_flags) {
    os_memmove(G_io_apdu_buffer, G_io_state_t.io_buffer + G_io_state_t.io_offset,
        G_io_state_t.io_length);

    if (io_flags & IO_RETURN_AFTER_TX) {
        io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, G_io_state_t.io_length);
        }
    else {
        io_exchange(CHANNEL_APDU, G_io_state_t.io_length);
        }

    incognito_io_clean();
    }
