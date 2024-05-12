#ifndef __HYDRA__
#define __HYDRA__

#include <stdio.h>
#include <emscripten.h>

#include "d_ticcmd.h"

// TODO: Find a better way to interface with JS

EM_JS(void, hydra_send, (ticcmd_t *cmd), {
  console.log("glue out", "forwardmove", HEAPU8[cmd]);
  hydraSend({
    forwardMove: HEAPU8[cmd]
  });
});

// TODO: calling c from javascript proved itself as hard because the
// EXPORTED_FUNCTIONS option would not work?

EM_JS(void, hydra_recv, (ticcmd_t *cmd), {
  const res = hydraRecv();
  console.log("glue in", res);
  if (res == null) {
     return;
  }
  HEAPU8[cmd] = res.forwardMove;
});

#endif
