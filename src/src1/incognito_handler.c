#include <incognito_types.h>
#include "incognito_api.h"
#include "incognito_vars.h"


void update_protocol() {
    G_incognito_vstate.tx_state_ins = G_incognito_vstate.io_ins;
    G_incognito_vstate.tx_state_p1 = G_incognito_vstate.io_p1;
    G_incognito_vstate.tx_state_p2 = G_incognito_vstate.io_p2;
}

void clear_protocol() {
    G_incognito_vstate.tx_state_ins = 0;
    G_incognito_vstate.tx_state_p1 = 0;
    G_incognito_vstate.tx_state_p2 = 0;
}

int check_potocol() {
    /* if locked and pin is veririfed, unlock */
    if ((G_incognito_vstate.protocol_barrier == PROTOCOL_LOCKED_UNLOCKABLE) &&
        (os_global_pin_is_validated() == PIN_VERIFIED)) {
        G_incognito_vstate.protocol_barrier = PROTOCOL_UNLOCKED;
    }

    /* if we are locked, deny all command! */
    if (G_incognito_vstate.protocol_barrier != PROTOCOL_UNLOCKED) {
        return SW_SECURITY_LOCKED;
    }

    /* the first command enforce the protocol version until application quits */
    switch (G_incognito_vstate.io_protocol_version) {
    case 0x00: /* the first one: PCSC epoch */
    case 0x03: /* protocol V3 */
    case 0x04: /* protocol V4 */
        if (G_incognito_vstate.protocol == 0xff) {
            G_incognito_vstate.protocol = G_incognito_vstate.io_protocol_version;
        }
        if (G_incognito_vstate.protocol == G_incognito_vstate.io_protocol_version) {
            break;
        }
        // unknown protocol or hot protocol switch is not allowed
        // FALL THROUGH

    default:
        return SW_PROTOCOL_NOT_SUPPORTED;
    }
    return SW_OK;
}


int check_ins_access() {
    if (G_incognito_vstate.key_set != 1) {
        return SW_KEY_NOT_SET;
    }

    switch (G_incognito_vstate.io_ins) {
    case INS_LOCK_DISPLAY:
    case INS_RESET:
    case INS_GET_KEY:
    case INS_DISPLAY_ADDRESS:
        return SW_OK;
    }

    return SW_INS_NOT_SUPPORTED;
}


int incognito_handler()
{
    int sw;

    // if (((sw = check_potocol()) != SW_OK) || ((sw = check_ins_access() != SW_OK)))
    //     {
    //     incognito_io_discard(0);
    //     return sw;
    //     }

    G_incognito_vstate.options = incognito_io_fetch_u8();

    if (G_incognito_vstate.io_ins == INS_RESET) {
        sw = incognito_apdu_reset();
        return sw;
    }

    if (G_incognito_vstate.io_ins == INS_LOCK_DISPLAY) {
        sw = incognito_apdu_lock();
        return sw;
    }
    sw = 0x6F01;

    switch (G_incognito_vstate.io_ins) {
    case INS_GET_KEY:
        // sw = incognito_apdu_get_key();
        break;
    case INS_DISPLAY_ADDRESS:
        // sw = incognito_apdu_display_address();
        break;
    default:
        THROW(SW_INS_NOT_SUPPORTED);
        break;
    }
    return sw;
}
