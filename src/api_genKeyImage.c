#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"



// static char keyimg[33];

static uint8_t set_result_import_keyimg() {
  uint8_t tx = 0;
  const uint8_t keyimage_size = 33;
  // G_io_apdu_buffer[tx++] = ota_size;
  os_memmove(G_io_apdu_buffer + tx, processData, keyimage_size);
  tx += keyimage_size;
  os_memset(processData, 0, sizeof(processData));
  return tx;
  }

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
  ux_display_keyimg_flow_1_step,
  pnn,
  {
    &C_icon_warning,
    "Gen",
    "KeyImage?",
  });
// UX_STEP_NOCB(
//     ux_display_keyimg_flow_2_step,
//     bnnn_paging,
//     {
//       .title = "KeyImage",
//       .text = keyimg,
//     });
UX_STEP_VALID(
  ux_display_keyimg_flow_3_step,
  pb,
  sendResponse(set_result_import_keyimg(), true),
  {
    &C_icon_validate_14,
    "Approve",
  });
UX_STEP_VALID(
  ux_display_keyimg_flow_4_step,
  pb,
  sendResponse(0, false),
  {
    &C_icon_crossmark,
    "Reject",
  });

UX_FLOW(ux_display_keyimg_flow,
  &ux_display_keyimg_flow_1_step,
  // &ux_display_keyimg_flow_2_step,
  &ux_display_keyimg_flow_3_step,
  &ux_display_keyimg_flow_4_step,
  FLOW_LOOP
);


void handleGenKeyImage(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx) {
  UNUSED(dataLength);
  UNUSED(p2);
  UNUSED(p1);
  unsigned char k[32];
  unsigned char coin_pubkey[32];

  os_memmove(k, dataBuffer, 32);
  unsigned char k1[32];
  incognito_hash_to_scalar(k1, k, 32);
  unsigned char k2[32];
  incognito_hash_to_scalar(k2, G_crypto_state_t.key.key, 32);
  incognito_addm(k, k, G_crypto_state_t.key.key);

  os_memmove(coin_pubkey, dataBuffer + 32, 32);
  incognito_generate_key_image(processData, coin_pubkey, k);

  // os_memmove(processData, k, 32);
  processData[33] = '\0';
  ux_flow_init(0, ux_display_keyimg_flow, NULL);
  *flags |= IO_ASYNCH_REPLY;
  }
