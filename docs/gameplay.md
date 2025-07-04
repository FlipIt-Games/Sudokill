# Gameplay

## Player Stats & Resources

### Health
- When at zero, the game is over.
- Base value is 13, increases by 3 when enhanced.

### Stamina
- Decreases by 1 after every [combat turn](#combat). 
- When entering a [combat turn](#combat), if this value is at 0, the player looses 1 health point.
- Base value is 10, increases by 3 when enhanced.

### Damage
- Monster value decreases by this value at the beggining of every [combat turn](#combat)
- Base value is 1, increases by 1 when enhanced.

## Grids

The level is built on top of two orthogonal sudoku grids: one grid for the [enemies](#Monsters) and one grid for the [items or effects](#Items).

That means, for every cell in the level, there is a combination of an enemy and an item or effect, that combination is unique, and every possible combination is gonna occur in the level.

Grid cells can be in 3 different states:

### 1. Undiscovered
The cell is covered by fog and its content is not displayed to the player

Cells can switch to the unconquered state if the player either uses a [Vision Stone](#Items) or moves to the cell.
If the player moves to the cell and one shot the enemy *[see combat system](#Combat)*, the cell switches to the conquered state.

### 2. Unconquered
The cell is not covered by fog, so its content is visible but the player has yet to conquer it. The monster that lives on it is still alive and the item has not been collected.
Unconquered cells should display:
- The initial value of the cell
- The current value of the monster
- The item to be collected

*Ex: A cell has a 7 monster level and a Damage Boost item to it. The player has already attacked it and inflicted 3 damage to it. The cell should display 7 has the initial value, 4 hasthe current value and an icon for the Damage Boost*

An unconquered cell only switches to the conquered state when the player defeat the monster that sits on it. 
They cannot switch back to the undiscovered state.

### 3. Conquered
The monster that sits on the cell has been defeated and the item has been collected. 
Conquered cell should display:
- The initial value of the cell
- A grayed icon indicating what was the former item to collect here

## Turn System
A turn happens every time the player moves from one cell to another, no matter their states.

### Moving to an [undiscovered cell](#_1-undiscovered)
- The fog is removed and the cell switches to the [unconquered](#_2-unconquered) state.
- A [combat](#combat) turn occurs

### Moving to an [unconquered cell](#_2-unconquered)
- A [combat](#combat) turn occurs
- If the monster is defeated at the end of the combat turn the cell switches to the [conquered](#_3-conquered)

### Moving to an [conquered cell](#_3-conquered)
Nothing special happens, the little guy just moves.


## Combat
When a combat turns occurs the monster value is first decreased by the [damage](#damage) value of the player

- If the player [stamina](#stamina) value is equal to 0 when entering the combat turn, he looses 1 [health](#health) point.
- If the player is still alive at the end of the combat turn, his [stamina](stamina) value is decreased by one.


### If the monster is still alive
- the [health](#health) value of the player is decreased by the remaining monster value
- If the [health](#health) value of the player is less or equal to 0 then the game is over
- If the player is still alive, he moves back to the cell he came from

### If the monster is dead
- The cell switches to the [conquered state](#_3-conquered)
- The item on the cell is collected
- The player position remains at the cell the combat took place


## Monsters

Every monster is associated to a value on the enemy grid representing is power level. That number ranges from 1 to 9. 

## Items

### 1. None 

When the player walks on a cell with this effect, nothing happens, there is no mob and no item to collect.

### 2. Health

When the monster is defeated the player regains [health](#health) according to the initial monster value.

*ex: The player defeats a monster on a cell with an initial value of 5, he regains 5 [health](#health) points*

### 3. Stamina
When the monster is defeated the player regains [stamina](#stamina) according to the initial monster value.

*ex: The player defeats a monster on a cell with an initial value of 5, he regains 5 [stamina](#stamina) points*

### 4. Damage Boost
- Provides a temporary damage boost to the player's attack. 
- Damage boost = initial cell value

**Exemple:**

During [combat turn](#combat) bonuses are pooled from the current damage boost value and added to the player base damage until the attack value matches the monster current value or thedamage boost is fully consummed.

1. player has a damage boost of 5 and a base attack value of 2. He attack a cell with a current value of 2. Player inflicts 2 damage, killing the monster and the damage boost stay unchanged.

2. He then attacks a cell with a value of 4. Player inflicts 4 damage, killing the monster, his damage boost is now at 3.
3. He attacks a cell with a value of 6. Player inflics 5 damage. Monster is still alive and the damage boost is now at 0.

### 5. Arrow

- Allows the player to select any non [conquered](#_3-conquered) cell to instantly inflict damage to it
- Damages scales with initial cell value

### 6. Vision Stone

- Allows the player to reveal the content of a cell, turning it from [undiscovered](#_1-undiscovered) to [unconquered](#_2-unconquered)

### 7. Teleport

- Allows the player to teleport to a cell of its choice
- This engage a combat turn

### 8. Gold

- Currency used to buy new upgrades

### 9. Root

- Can be used to catalyse gold and gain upgrade
- Monster are twice as powerful
- Monster with value 9 is the final boss
