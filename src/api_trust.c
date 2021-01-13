#include "api.h"
#include "os.h"
#include "ux.h"
#include "globals.h"
#include "utils.h"

static uint8_t set_result_trust_device()
{
  trust_host = 1;
  return 0;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_trust_flow_1_step,
    pnn,
    {
        &C_icon_warning,
        "Trust",
        "this device?",
    });
UX_STEP_VALID(
    ux_display_trust_flow_2_step,
    pb,
    sendResponse(set_result_trust_device(), true),
    {
        &C_icon_validate_14,
        "Approve",
    });
UX_STEP_VALID(
    ux_display_trust_flow_3_step,
    pb,
    sendResponse(0, false),
    {
        &C_icon_crossmark,
        "Reject",
    });

UX_FLOW(ux_display_trust_flow,
        &ux_display_trust_flow_1_step,
        &ux_display_trust_flow_2_step,
        &ux_display_trust_flow_3_step,
        FLOW_LOOP);

void handleTrustDevice(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
  UNUSED(dataLength);
  UNUSED(p2);

  ux_flow_init(0, ux_display_trust_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
};
