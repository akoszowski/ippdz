This project is so called 'big task' developed for individual programmistic project subject at Univeristy of Warsaw.

It is an implementation of gamma game engine working within two possible console modes:
  - batch mode,
  - interactive mode.
 
RULES:

Gamma game is played on a rectangular board, consisting of uniform squar fields, empty at the begining.
Neighbour fields are defined as those with common side. 
Set of fields, that can be reached while moving between neighbour fields is defined as an area. Single field is also an area.
Gamma game board width and height as well as number of players input parameters of gamma game.

Players consecutively capture single fields, by placing their piece. Player can capture any free field 
provided that the amount of occupied areas is no bigger than maximal number of areas established as the last parameter of game.
Each player once in a game can execute a golden move, which consists in replacing another players piece with the player's own piece.
But still, such a golden move can not break the rule of maximal number of areas.
Player who can not make a move according to those rules is out of game, however his state can be changed by another players golden move.
Game ends when there is no player who can make a legal move.

The winner is a player occupying the biggest number of gamma game fields.
