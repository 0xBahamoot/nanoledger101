#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"

// static char view[32];

static uint8_t set_result_get_view()
{
  uint8_t tx = 0;
  const uint8_t view_size = 33;
  // G_io_apdu_buffer[tx++] = view_size;
  os_memmove(G_io_apdu_buffer + tx, processData, view_size);
  tx += view_size;
  os_memset(processData, 0, sizeof(processData));
  return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_view_flow_1_step,
    pnn,
    {
        &C_icon_warning,
        "Export",
        "Viewkey?",
    });
// UX_STEP_NOCB(
//   ux_display_view_flow_2_step,
//   bnnn_paging,
//   {
//     .title = "Viewkey",
//     .text = processData,
//   });
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
        // &ux_display_view_flow_2_step,
        &ux_display_view_flow_3_step,
        &ux_display_view_flow_4_step,
        FLOW_LOOP);

void handleGetView(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
  UNUSED(dataLength);
  UNUSED(p2);
  UNUSED(p1);
  os_memmove(processData, G_crypto_state_t.a, 32);
  processData[33] = '\0';
  ux_flow_init(0, ux_display_view_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
}
