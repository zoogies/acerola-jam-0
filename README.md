# acerola-jam-0

My entry into the acerola jam 0

## TODO:

- Player controller
- keybind changer
- trigger colliders - used for detecting where player crossed for stuff like bind changes
- brainstorm abilities
- Port cutscene manager from raise a vannie
- intro cutscene
- end cutscene
- Main menu

## BUGS

ENGINE

- the first scene we load doesnt get the scene load callback on load since that occurs before engine fully init, so entry has to do it itself as a dumb hack

## ideas

question mark for bind until you located it first time

- commentary
- heartbeat monitor for health

## transfomration

- ear ringing
- loud heartbeat in background

## todo today

enemy ai

arm attack

health meter

"death" animation

## for me tomorrow

- player "health" which takes damage when hit by enemies (needs cooldown for not spamming it), when health reaches low player screams and enemies that hit them die (maybe keep queue for all hits that lead up to death)
- how to register hits from player arm onto enemies? could put a trigger that covers the whole area around the players but reshape it based on direction facing? idk. trigger should work because can detect that collision with enemies static
- should we have round colliders - how hard?
- enemy hit noises, death noises, enemy groan audio emitter

## notes

- would be cool to have arm alternate its approach each time (just invert renderer and switch angle)

## where im at rn

- enemy groan audiosource
- need registering hits from enemies to player
  - then player "death" condition
- probably need to seperate the vectors for up and down collectively in physics calculations
- calculate distance from center of enemy for calc - hitbox buggy for killing
- COULD NOT REPLICATE: bug where if you walk into two colliders at once you phase
