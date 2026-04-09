# Echoes – Tower Defense Roguelike
> ⚠️ Early development — not yet playable

A tower defense roguelike built in Unreal Engine 5 as a personal project to deepen expertise in UE5 and C++ game development.

## Concept
Each run takes place on a hexagonal world map, where every hex contains a procedurally generated level. Completing a level rewards permanent upgrades that carry over between runs. Failing to complete a level costs a life. The goal is to clear all hexes on the map.

## Current State
The project is in early development. What is currently implemented:

- **Grid system** — configurable grid with cell state tracking (walkable, occupied, start, end)
- **Procedural path generation** — DFS with full backtracking and segment-biased direction control for natural-looking paths; path length, number of turns and direction change probability are all configurable
- **Tower placement** — grid-based placement with collision and walkability checks
- **Tower system** — data-asset driven tower definitions, extensible via Blueprint subclassing
- **Ghost tower preview** — transparent tower preview that follows the cursor before placement
- **Tower selection menu** — scalable UI menu for selecting between different tower types
- **Orbital camera** — top-down perspective camera with zoom and rotation support

## Technologies
- Unreal Engine 5
- C++ and Blueprints

## Screenshots

### Procedural Path Generation
Two examples of procedurally generated paths — blue: buildable cells, green: walkable path, yellow: start, red: end

| | |
|---|---|
| <img width="2559" height="1388" alt="Screenshot 2026-04-10 011352" src="https://github.com/user-attachments/assets/2595f040-7efb-4928-951e-88244cfbdfdc" />| <img width="2559" height="1389" alt="Screenshot 2026-04-10 011105" src="https://github.com/user-attachments/assets/5e6a0a8c-7e36-4280-8d7c-c730faaf8a27" />|

### Tower Placement
<img width="2558" height="1392" alt="Screenshot 2026-04-10 011155" src="https://github.com/user-attachments/assets/a10b5fe1-4972-4f41-b8b8-f5a06c07936b" />

*Grid-based tower placement with ghost preview following the cursor*

### Tower Selection Menu
<img width="2558" height="1392" alt="Screenshot 2026-04-10 011218" src="https://github.com/user-attachments/assets/7715702e-deba-47ee-b2a9-b11f3cd6c451" />

*Tower selection menu with multiple tower types*
