#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "globals.h"

static char address[95];

static uint8_t set_result_get_address() {
  uint8_t tx = 0;
  const uint8_t address_size = strlen(address);
  G_io_apdu_buffer[tx++] = address_size;
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
  unsigned char publicKey[64];

  os_memmove(publicKey, G_crypto_state_t.A, 32);
  os_memmove(publicKey + 32, G_crypto_state_t.B, 32);

  // snprintf((char*)address, sizeof(address), "lol");

  address[encodeBase58(publicKey, 64, (unsigned char*)address, 95) + 3] = '\0';

  // unsigned int offset;
  // offset = 64;
  // unsigned int full_block_count = (offset) / FULL_BLOCK_SIZE;
  // unsigned int last_block_size = (offset) % FULL_BLOCK_SIZE;

  // for (size_t i = 0; i < full_block_count; ++i) {
  //   encode_block(publicKey + i * FULL_BLOCK_SIZE, FULL_BLOCK_SIZE,
  //     &address[i * FULL_ENCODED_BLOCK_SIZE]);
  // }

  // if (0 < last_block_size) {
  //   encode_block(publicKey + full_block_count * FULL_BLOCK_SIZE, last_block_size,
  //     &address[full_block_count * FULL_ENCODED_BLOCK_SIZE]);
  // }

  // address[95] = '\0';

  ux_flow_init(0, ux_display_public_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
  }
