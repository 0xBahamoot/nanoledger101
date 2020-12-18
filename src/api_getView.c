#include "api_getView.h"
#include "os.h"
#include "ux.h"
#include "utils.h"

static char view[FULL_ADDRESS_LENGTH];

static uint8_t set_result_get_view() {
  uint8_t tx = 0;
  const uint8_t view_size = strlen(view);
  G_io_apdu_buffer[tx++] = view_size;
  os_memmove(G_io_apdu_buffer + tx, view, view_size);
  tx += view_size;
  return tx;
  }

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
  ux_display_view_flow_1_step,
  pnn,
  {
    &C_icon_warning,
    "Retrieving",
    "Viewkey?",
  });
UX_STEP_NOCB(
  ux_display_view_flow_2_step,
  bnnn_paging,
  {
    .title = "Viewkey",
    .text = view,
  });
UX_STEP_VALID(
  ux_display_view_flow_3_step,
  pb,
  sendResponse(set_result_get_view(), true),
  {
    &C_icon_validate_14,
    "Approve",
  });
UX_STEP_VALID(
  ux_display_view_flow_4_step,
  pb,
  sendResponse(0, false),
  {
    &C_icon_crossmark,
    "Reject",
  });

UX_FLOW(ux_display_view_flow,
  &ux_display_view_flow_1_step,
  &ux_display_view_flow_2_step,
  &ux_display_view_flow_3_step,
  &ux_display_view_flow_4_step,
  FLOW_LOOP
);

void handleGetView(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx) {
  UNUSED(dataLength);
  UNUSED(p2);
  uint8_t viewKey[32];

  getPublicKey(readUint32BE(dataBuffer), viewKey);
  getAddressStringFromBinary(viewKey, view);

  // if (p1 == P1_NON_CONFIRM) {
  //     *tx = set_result_get_view();
  //     THROW(0x9000);
  // } else {
  //     ux_flow_init(0, ux_display_public_flow, NULL);
  //     *flags |= IO_ASYNCH_REPLY;
  // }
  ux_flow_init(0, ux_display_view_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
  }
