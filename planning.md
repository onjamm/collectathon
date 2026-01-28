A place to write your findings and plans

## Understanding
this is the pivotal line that imports the random to be used for generating the "treasure" "#include <bn_random.h>"

## Planning required changes
1. locating the variable where player speed is kept then changing it to be faster 
2. locate variable for backdrop color and change it to purple
3. create and add static constexpr for starting X and Y
4. Create loop to allow for start and restart function
5. Locate where player movement is and add ruleset for screen boundaries 
6. Create a function for when A is pressed and have it speed up the players movement.

## Brainstorming game ideas
- build a snake style game, when the user collects a gold coin they grow in length 
- instead of speed boost allow for slowing down 
- make our sprite look like a snake
- if the users sprite ptr crosses over it's body then the game ends with selection to restart
- a counter in the top right showing a score representing the length of the snake 
- after a certain score do we add obstacles? 

## Plan for implementing game
- set a MAX_SEGMENTS constant and a constant positioning for adding to the body
- Store position of the head so the body can follow using deque?
- set spacing for body segments to follow the head 
- if statement, for the head position to be fixed at the front of que.
- create index for all the body segments
- create an if statement for the user to react to the treasure when intersected 
- add a body segment after each treasure is collected 
- reset body on game end logic 
