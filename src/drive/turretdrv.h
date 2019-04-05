/*
 * Note that since we are setting PWM values in hardware, we don't need state or tasks
*/

void init_turret_pwm();

void set_turret_x_val(int val); // Pin 11, Between 0 and 100
void set_turret_y_val(int val); // Pin 12, Between 0 and 100
//int get_turret_x_val(); // Between 0 and 100
//int get_turret_y_val(); // Between 0 and 100
