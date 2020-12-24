#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "globals.h"
#include "crypto.h"

static char address[123];
static uint8_t set_result_get_address() {
  uint8_t tx = 0;
  const uint8_t address_size = strlen(address);
  os_memmove(G_io_apdu_buffer + tx, address, address_size);
  tx += address_size;
  return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
  ux_display_public_flow_1_step,
  pnn,
  {
    &C_icon_warning,
    "Retrieving",
    "Address?",
  });
UX_STEP_NOCB(
  ux_display_public_flow_2_step,
  bnnn_paging,
  {
    .title = "PaymentAddress",
    .text = address,
  });
UX_STEP_VALID(
  ux_display_public_flow_3_step,
  pb,
  sendResponse(set_result_get_address(), true),
  {
    &C_icon_validate_14,
    "Approve",
  });
UX_STEP_VALID(
  ux_display_public_flow_4_step,
  pb,
  sendResponse(0, false),
  {
    &C_icon_crossmark,
    "Reject",
  });

UX_FLOW(ux_display_public_flow,
  &ux_display_public_flow_1_step,
  &ux_display_public_flow_2_step,
  &ux_display_public_flow_3_step,
  &ux_display_public_flow_4_step,
  FLOW_LOOP
);

void handleGetAddress(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx) {
  UNUSED(dataLength);
  UNUSED(p2);
  UNUSED(p1);
  address[0] = 1;
  address[1] = 32;
  os_memmove(address + 2, G_crypto_state_t.B, 32);
  address[34] = 32;
  os_memmove(address + 35, G_crypto_state_t.A, 32);


  uint8_t buffer[32];
  //sha3
  incognito_keccak_F(address, 67, buffer);
  os_memmove(address + 67, buffer, 4);

  unsigned char base58check[76];
  // uint8_t buffer2[32];
  os_memset(buffer, 0, 32);
  base58check[0] = 0;
  os_memmove(base58check + 1, address, 71);
  incognito_keccak_F(base58check, 72, buffer);
  os_memmove(base58check + 72, buffer, 4);
  os_memset(address, 0, 32);

  address[encodeBase58(base58check, 76, (unsigned char*)address, 120) + 3] = '\0';

  ux_flow_init(0, ux_display_public_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
}
