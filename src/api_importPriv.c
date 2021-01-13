#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "globals.h"
#include "crypto.h"

static char priv[112];

static uint8_t set_result_import_priv()
{
  // uint8_t tx = 0;
  // const uint8_t priv_size = strlen(processData);
  // G_io_apdu_buffer[tx++] = priv_size;
  // os_memmove(G_io_apdu_buffer + tx, processData, priv_size);
  // tx += priv_size;
  return 0;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_priv_flow_1_step,
    pnn,
    {
        &C_icon_warning,
        "Importing",
        "privatekey?",
    });
UX_STEP_NOCB(
    ux_display_priv_flow_2_step,
    bnnn_paging,
    {
        .title = "privatekey",
        .text = priv,
    });
UX_STEP_VALID(
    ux_display_priv_flow_3_step,
    pb,
    sendResponse(set_result_import_priv(), true),
    {
        &C_icon_validate_14,
        "Approve",
    });
UX_STEP_VALID(
    ux_display_priv_flow_4_step,
    pb,
    sendResponse(0, false),
    {
        &C_icon_crossmark,
        "Reject",
    });

UX_FLOW(ux_display_priv_flow,
        &ux_display_priv_flow_1_step,
        &ux_display_priv_flow_2_step,
        &ux_display_priv_flow_3_step,
        &ux_display_priv_flow_4_step,
        FLOW_LOOP);

void handleImportPrivate(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
  // UNUSED(dataLength);
  // UNUSED(p2);

  os_memmove(processData, dataBuffer, dataLength);
  incognito_add_B58checksum(processData, 71, (unsigned char *)priv);
  unsigned char base58check[80];
  os_memset(priv, 0, sizeof(priv));
  base58check[0] = 0;
  os_memmove(base58check + 1, processData, 75);
  incognito_add_B58checksum(base58check, 76, (unsigned char *)priv);
  os_memset(priv, 0, sizeof(priv));
  priv[encodeBase58(base58check, 80, (unsigned char *)priv, 116)] = '\0';

  incognito_reset_crypto_state();
  G_crypto_state_t.key.depth = processData[1];
  G_crypto_state_t.key.child_number = (processData[2] << 24) | (processData[3] << 16) | (processData[4] << 8) | (processData[5]);
  os_memmove(G_crypto_state_t.key.chain_code, processData + 6, 32);
  os_memmove(G_crypto_state_t.key.key, processData + 39, 32);
  incognito_init_crypto_state();

  ux_flow_init(0, ux_display_priv_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
};
