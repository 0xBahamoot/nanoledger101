#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "key.h"

// static char ota[32];

static uint8_t set_result_get_ota()
{
  uint8_t tx = 0;
  const uint8_t ota_size = 32;
  // G_io_apdu_buffer[tx++] = ota_size;
  os_memmove(G_io_apdu_buffer + tx, processData, ota_size);
  tx += ota_size;
  os_memset(processData, 0, sizeof(processData));
  return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_ota_flow_1_step,
    pnn,
    {
        &C_icon_warning,
        "Export",
        "OTAkey?",
    });
// UX_STEP_NOCB(
//   ux_display_ota_flow_2_step,
//   bnnn_paging,
//   {
//     .title = "Viewkey",
//     .text = processData,
//   });
UX_STEP_VALID(
    ux_display_ota_flow_3_step,
    pb,
    sendResponse(set_result_get_ota(), true),
    {
        &C_icon_validate_14,
        "Approve",
    });
UX_STEP_VALID(
    ux_display_ota_flow_4_step,
    pb,
    sendResponse(0, false),
    {
        &C_icon_crossmark,
        "Reject",
    });

UX_FLOW(ux_display_ota_flow,
        &ux_display_ota_flow_1_step,
        // &ux_display_ota_flow_2_step,
        &ux_display_ota_flow_3_step,
        &ux_display_ota_flow_4_step,
        FLOW_LOOP);

void handleGetOTA(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
  UNUSED(dataLength);
  UNUSED(p2);
  UNUSED(p1);
  unsigned char key[32];
  incognito_gen_private_ota_key(key);
  os_memmove(processData, key, 32);
  // processData[33] = '\0';
  ux_flow_init(0, ux_display_ota_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
}
