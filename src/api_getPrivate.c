#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"

static char private[FULL_ADDRESS_LENGTH];

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
  uint8_t privateKey[32];

  getPublicKey(readUint32BE(dataBuffer), privateKey);
  getAddressStringFromBinary(privateKey, private);

  // if (p1 == P1_NON_CONFIRM) {
  //     *tx = set_result_get_private();
  //     THROW(0x9000);
  // } else {
  //     ux_flow_init(0, ux_display_public_flow, NULL);
  //     *flags |= IO_ASYNCH_REPLY;
  // }
  ux_flow_init(0, ux_display_private_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
  }
