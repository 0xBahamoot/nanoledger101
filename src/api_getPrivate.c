#include "api.h"
#include "os.h"
#include "ux.h"
#include "cx.h"
#include "utils.h"
#include "globals.h"
#include "crypto.h"

// static char private[123];
static uint8_t set_result_get_private() {
  uint8_t tx = 0;
  const uint8_t private_size = strlen(processData);
  // G_io_apdu_buffer[tx++] = private_size;
  os_memmove(G_io_apdu_buffer + tx, processData, private_size);
  tx += private_size;
  os_memset(processData, 0, sizeof(processData));
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
    .text = processData,
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

  // unsigned char privateKey[75];
  processData[0] = 0;
  os_memmove(processData + 1, &G_crypto_state_t.key.depth, 1);
  unsigned char child_number[4];
  child_number[0] = (G_crypto_state_t.key.child_number >> 24) & 0xFF;
  child_number[1] = (G_crypto_state_t.key.child_number >> 16) & 0xFF;
  child_number[2] = (G_crypto_state_t.key.child_number >> 8) & 0xFF;
  child_number[3] = G_crypto_state_t.key.child_number & 0xFF;
  os_memmove(processData + 2, child_number, 4);
  os_memmove(processData + 6, G_crypto_state_t.key.chain_code, 32);
  processData[38] = 32;
  os_memmove(processData + 39, G_crypto_state_t.key.key, 32);

  uint8_t buffer[32];
  //doublesha256
  // incognito_doublesha256(processData, 71, buffer);

  //sha3
  incognito_keccak_F(processData, 71, buffer);
  os_memmove(processData + 71, buffer, 4);


  unsigned char base58check[80];
  os_memset(buffer, 0, 32);
  base58check[0] = 0;
  os_memmove(base58check + 1, processData, 75);
  incognito_keccak_F(base58check, 76, buffer);
  os_memmove(base58check + 76, buffer, 4);
  os_memset(processData, 0, sizeof(processData));

  processData[encodeBase58(base58check, 80, (unsigned char*)processData, 120) + 3] = '\0';

  ux_flow_init(0, ux_display_private_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
  }
