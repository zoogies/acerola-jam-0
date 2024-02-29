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
