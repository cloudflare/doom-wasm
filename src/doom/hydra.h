#ifndef __HYDRA__
#define __HYDRA__

#include <stdio.h>
#include <emscripten.h>

#include "d_ticcmd.h"
#include "d_player.h"
#include "doomdef.h"
#include "doomstat.h"

// TODO: Find a better way to interface with JS
// hydra_send(cmd, player_state, killcount, mo->health, mo->floorz, mo->momx, mo->momy, mo->momz, mo->z, mo->angle, gamestate);

EM_ASYNC_JS(void, hydra_send, (ticcmd_t *cmd, playerstate_t playerstate, int kill_count, int secret_count, int item_count, int int health, int floorz, int momx, int momy, int momz, int z, int angle, gamestate_t gamestate), {
  await hydraSend({
    forwardMove: HEAP8[cmd],
    sideMove: HEAP8[cmd+1],
  },
  {
    mapObject: {
      position: {
        floorZ: floorz,
        momentumX: momx,
        momentumY: momy,
        momentumZ: momz,
        z: z,
        angle: angle
      },
      health: health,
    },
    playerState: player_state,
    killCount: kill_count,
    secretCount: secret_count,
    itemCount: item_count,
  },
  gamestate);
});

// TODO: calling c from javascript proved itself as hard because the
// EXPORTED_FUNCTIONS option would not work?

EM_ASYNC_JS(void, hydra_recv, (ticcmd_t *cmd), {
  const res = await hydraRecv();
  console.log("hydra_recv", res);
  if (res == null) {
     return;
  }
  HEAP8[cmd] = res.forwardMove;
  HEAP8[cmd+1] = res.sideMove;
  // FIXME: integrate more ticcmd_t fields
});

#endif
