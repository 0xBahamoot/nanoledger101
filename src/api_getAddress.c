#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "globals.h"
#include "crypto.h"
#include "menu.h"
#include "key.h"

#define ADDRESS_LENGTH 160

static uint8_t set_result_get_address()
{
  uint8_t tx = 0;
  // const uint8_t address_size = ADDRESS_LENGTH;
  // G_io_apdu_buffer[tx++] = address_size;
  os_memmove(G_io_apdu_buffer + tx, processData, ADDRESS_LENGTH);
  tx += ADDRESS_LENGTH;

  os_memset(processData, 0, sizeof(processData));
  return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_public_flow_1_step,
    pnn,
    {
        &C_icon_warning,
        "Export",
        "Address?",
    });
UX_STEP_NOCB(
    ux_display_public_flow_2_step,
    bnnn_paging,
    {
        .title = "PaymentAddress",
        .text = (const char *)processData,
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
        FLOW_LOOP);

void handleGetAddress(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
  UNUSED(dataLength);
  UNUSED(p2);
  UNUSED(p1);
  processData[0] = 1;
  processData[1] = 32;
  //PaymentAddress.Pk
  unsigned char buffer[32];
  incognito_gen_public_spend_key(buffer);
  os_memmove(processData + 2, buffer, 32);
  //PaymentAddress.TK
  processData[34] = 32;
  os_memset(buffer, 0, 32);
  incognito_gen_public_view_key(buffer);
  os_memmove(processData + 35, buffer, 32);
  //PaymentAddress.OTAPublic
  processData[67] = 32;
  os_memset(buffer, 0, 32);
  incognito_gen_public_ota_key(buffer);
  os_memmove(processData + 68, buffer, 32);

  os_memset(buffer, 0, 32);
  incognito_add_B58checksum(processData, 100, buffer);

  unsigned char base58check[109];
  base58check[0] = 0;
  os_memmove(base58check + 1, processData, 104);

  os_memset(processData, 0, sizeof(processData));
  os_memset(buffer, 0, 32);
  incognito_add_B58checksum(base58check, 105, buffer);

  processData[encodeBase58(base58check, 109, (unsigned char *)processData, 160) + 3] = '\0';

  ux_flow_init(0, ux_display_public_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
}
