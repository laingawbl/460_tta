/**
 * @file lightdrv.h
 * @brief light sensor driver
 *
 * LIGHTDRV translates values from the photocell under the light cone into a HIT or NO HIT value. Note that this only
 * indicates whether the cone is *being* hit, not whether the Roomba on the whole has suffered a 2-second "hit" (that
 * is the task of DEATHMON).
 */

/**
 * @brief start the LIGHTDRV task.
 * TODO: how often should LIGHTDRV run?
 */
void _lightdrvTask();

/**
 * @brief get the latest reading from the LIGHTDRV task.
 * @return true if the cone is being hit; false, if not
 */
bool lightHit();