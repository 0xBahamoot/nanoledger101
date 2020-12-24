#include "api.h"
#include "os.h"
#include "ux.h"
#include "cx.h"
#include "utils.h"
#include "globals.h"
#include "crypto.h"

unsigned char private[123];

static uint8_t set_result_get_private() {
  uint8_t tx = 0;
  const uint8_t private_size = strlen(private);
  G_io_apdu_buffer[tx++] = private_size;
  os_memmove(G_io_apdu_buffer + tx, private, private_size);
  tx += private_size;
  return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
  ux_display_private_flow_1_step,
  pnn,
  {
    &C_icon_warning,
    "Retrieving",
    "Privatekey?",
  });
UX_STEP_NOCB(
  ux_display_private_flow_2_step,
  bnnn_paging,
  {
    .title = "Privatekey",
    .text = private,
  });
UX_STEP_VALID(
  ux_display_private_flow_3_step,
  pb,
  sendResponse(set_result_get_private(), true),
  {
    &C_icon_validate_14,
    "Approve",
  });
UX_STEP_VALID(
  ux_display_private_flow_4_step,
  pb,
  sendResponse(0, false),
  {
    &C_icon_crossmark,
    "Reject",
  });

UX_FLOW(ux_display_private_flow,
  &ux_display_private_flow_1_step,
  &ux_display_private_flow_2_step,
  &ux_display_private_flow_3_step,
  &ux_display_private_flow_4_step,
  FLOW_LOOP
);

void handleGetPrivate(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx) {
  UNUSED(dataLength);
  UNUSED(p2);
  UNUSED(p1);

  unsigned char privateKey[75];
  privateKey[0] = 0;
  os_memmove(privateKey + 1, &G_crypto_state_t.key.depth, 1);
  unsigned char child_number[4];
  child_number[0] = (G_crypto_state_t.key.child_number >> 24) & 0xFF;
  child_number[1] = (G_crypto_state_t.key.child_number >> 16) & 0xFF;
  child_number[2] = (G_crypto_state_t.key.child_number >> 8) & 0xFF;
  child_number[3] = G_crypto_state_t.key.child_number & 0xFF;
  os_memmove(privateKey + 2, child_number, 4);
  os_memmove(privateKey + 6, G_crypto_state_t.key.chain_code, 32);
  privateKey[38] = 32;
  os_memmove(privateKey + 39, G_crypto_state_t.key.key, 32);

  uint8_t buffer[32];
  //doublesha256
  // incognito_doublesha256(privateKey, 71, buffer);

  //sha3
  incognito_keccak_F(privateKey, 71, buffer);

  os_memmove(privateKey + 71, buffer, 4);




  private[encodeBase58(privateKey, 75, (unsigned char*)private, 120) + 3] = '\0';

  ux_flow_init(0, ux_display_private_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
}
