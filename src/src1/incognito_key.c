
#include "os.h"
#include "cx.h"
#include "incognito_types.h"
#include "incognito_api.h"
#include "incognito_vars.h"

int incognito_apdu_display_address() {
    unsigned int major;
    unsigned int minor;
    unsigned char index[8];
    unsigned char payment_id[8];
    unsigned char C[32];
    unsigned char D[32];

    // fetch
    incognito_io_fetch(index, 8);
    incognito_io_fetch(payment_id, 8);
    incognito_io_discard(0);

    major = (index[0] << 0) | (index[1] << 8) | (index[2] << 16) | (index[3] << 24);
    minor = (index[4] << 0) | (index[5] << 8) | (index[6] << 16) | (index[7] << 24);
    if ((minor | major) && (G_incognito_vstate.io_p1 == 1)) {
        THROW(SW_WRONG_DATA);
    }

    // retrieve pub keys
    if (minor | major) {
        incognito_get_subaddress(C, D, index);
    }
    else {
        os_memmove(C, G_incognito_vstate.A, 32);
        os_memmove(D, G_incognito_vstate.B, 32);
    }

    // prepare UI
    if (minor | major) {
        G_incognito_vstate.disp_addr_M = major;
        G_incognito_vstate.disp_addr_m = minor;
        G_incognito_vstate.disp_addr_mode = DISP_SUB;
    }
    else {
        G_incognito_vstate.disp_addr_mode = DISP_MAIN;

    }

    ui_menu_any_pubaddr_display(0, C, D, (minor | major) ? 1 : 0,
        (G_incognito_vstate.io_p1 == 1) ? payment_id : NULL);
    return 0;
}

/* ----------------------------------------------------------------------- */
/* ---                                                                 --- */
/* ----------------------------------------------------------------------- */
int incognito_apdu_get_key() {
    incognito_io_discard(1);
    switch (G_incognito_vstate.io_p1) {
        // get pub
    case 1:
        // view key
        incognito_io_insert(G_incognito_vstate.A, 32);
        // spend key
        incognito_io_insert(G_incognito_vstate.B, 32);
        // public base address
        incognito_base58_public_key((char*)G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset,
            G_incognito_vstate.A, G_incognito_vstate.B, 0, NULL);
        incognito_io_inserted(95);
        break;

        // get private
    case 2:
        // view key
        if (G_incognito_vstate.export_view_key == EXPORT_VIEW_KEY) {
            incognito_io_insert(G_incognito_vstate.a, 32);
        }
        else {
            ui_export_viewkey_display(0);
            return 0;
        }
        break;

    default:
        THROW(SW_WRONG_P1P2);
    }
    return SW_OK;
}


/* ----------------------------------------------------------------------- */
/* ---                                                                 --- */
/* ----------------------------------------------------------------------- */
int incognito_apdu_get_subaddress_secret_key(/*const crypto::secret_key& sec, const cryptonote::subaddress_index& index, crypto::secret_key &sub_sec*/) {
    unsigned char sec[32];
    unsigned char index[8];
    unsigned char sub_sec[32];

    incognito_io_fetch_decrypt_key(sec);
    incognito_io_fetch(index, 8);
    incognito_io_discard(0);

    incognito_get_subaddress_secret_key(sub_sec, sec, index);

    os_memmove(G_incognito_vstate.last_get_subaddress_secret_key, sub_sec, 32);
    incognito_io_insert_encrypt(sub_sec, 32, TYPE_SCALAR);
    return SW_OK;
}