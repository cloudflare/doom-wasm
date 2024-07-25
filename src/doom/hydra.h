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

EM_ASYNC_JS(void, hydra_send, (ticcmd_t *cmd, playerstate_t playerstate, int killcount, int health, int floorz, int momx, int momy, int momz, int z, int angle, gamestate_t gamestate), {
  console.log("hydra_send", "forwardmove", HEAPU8[cmd]);
  await hydraSend({
    forwardMove: HEAPU8[cmd]
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
    playerState: playerstate,
    killCount: killcount,
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
  HEAPU8[cmd] = res.forwardMove;
  // FIXME: integrate more ticcmd_t fields
});

#endif
