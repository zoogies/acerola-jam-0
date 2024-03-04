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
- should we have round colliders - how hard?

## notes

- would be cool to have arm alternate its approach each time (just invert renderer and switch angle)

## where im at rn

- player "health" which takes damage when hit by enemies (needs cooldown for not spamming it), when health reaches low player screams and enemies that hit them die (maybe keep queue for all hits that lead up to death)
- need registering hits from enemies to player
  - then player "death" condition
- probably need to seperate the vectors for up and down collectively in physics calculations
- calculate distance from center of enemy for calc - hitbox buggy for killing
- COULD NOT REPLICATE: bug where if you walk into two colliders at once you phase

## story relevant ideas

- when you break chair to get out of room you get attacked first time, then when you get to end of hall you get switched

## ideas

- view cone to obstruct behind you
- you berserk to get keybinds switched which is the only time you can attack
  - you have a metetr which brings you back to normal binds later on as your HR decreases
- tutorial to teach original keybinds is first room, you break a chair blocking the way

## bugs / perf issues

- make sure static colliders for walls are touching as little as possible - probably a fat optimization hit
