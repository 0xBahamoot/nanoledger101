#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"

static char priv[FULL_ADDRESS_LENGTH];

static uint8_t set_result_import_priv() {
  uint8_t tx = 0;
  const uint8_t priv_size = strlen(priv);
  G_io_apdu_buffer[tx++] = priv_size;
  os_memmove(G_io_apdu_buffer + tx, priv, priv_size);
  tx += priv_size;
  return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
  ux_display_priv_flow_1_step,
  pnn,
  {
    &C_icon_warning,
    "Importing",
    "privkey?",
  });
UX_STEP_NOCB(
  ux_display_priv_flow_2_step,
  bnnn_paging,
  {
    .title = "privkey",
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
  FLOW_LOOP
);

void  handleImportPrivate(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx) {
  UNUSED(dataLength);
  UNUSED(p2);
  uint8_t privKey[32];
  os_memmove(privKey, os_memmove, 32);
  priv[encodeBase58(privKey, 36, (unsigned char*)priv + 3, 51) + 3] = '\0';
  ux_flow_init(0, ux_display_priv_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
};
