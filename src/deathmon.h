/**
 * @file deathmon.h
 * @brief Roomba death-condition monitor
 *
 * DEATHMON watches the output from LIGHTDRV and IRDRV to monitor whether the Roomba has been hit by a laser or crossed
 * the IR river (respectively); if either of these conditions are satisfied, DEATHMON will put the robot into a dead
 * state.
 *
 * TODO: maybe turn the Roomba status LED red if dead?
 */