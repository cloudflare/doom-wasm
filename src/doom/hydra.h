#ifndef __HYDRA__
#define __HYDRA__

#include <stdio.h>
#include <emscripten.h>

#include "d_ticcmd.h"
#include "d_player.h"
#include "doomdef.h"
#include "doomstat.h"

// TODO: Find a better way to interface with JS

EM_ASYNC_JS(void, hydra_send, (ticcmd_t *cmd, playerstate_t player_state, int kill_count, int secret_count, int item_count, int health, int x, int y, int z, gamestate_t gamestate, int leveltics), {
    await hydraSend({
    forwardMove: HEAP8[cmd],
    sideMove: HEAP8[cmd+1],
  },
  {
    mapObject: {
      position: {
        x: x,
        y: y,
        z: z,
      },
      health: health,
    },
    playerState: player_state,
    totalStats: {
        killCount: -1,
        secretCount: -1,
        itemCount: -1,
    },
    levelStats: {
    killCount: kill_count,
    secretCount: secret_count,
    itemCount: item_count,
    }
  },
  gamestate,
  leveltics);
});

// TODO: calling c from javascript proved itself as hard because the
// EXPORTED_FUNCTIONS option would not work?

EM_ASYNC_JS(void, hydra_recv, (ticcmd_t *cmd), {
  const res = await hydraRecv();
  if (res == null) {
     return;
  }
  HEAP8[cmd] = res.forwardMove;
  HEAP8[cmd+1] = res.sideMove;
  // FIXME: integrate more ticcmd_t fields
});

#endif
