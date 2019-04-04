/*
 * Comms driver for bluetooth interface
 * Parses values and converts them to parameters.
 * Works on a host / slave model
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PKT_START_CHAR "^"
#define PKT_END_CHAR "$"
#define PKT_SEP_CHAR "/"

#define BASE_TO_REMOTE_FMT "%s%i%s%i%s%i%s%i%s%i%s" // params below.
#define BASE_TO_REMOTE_PARAMS PKT_START_CHAR,    \
                              data->laser_power,  \
                              PKT_SEP_CHAR,      \
                              data->turret_x_pos, \
                              PKT_SEP_CHAR,      \
                              data->turret_y_pos, \
                              PKT_SEP_CHAR,      \
                              data->rotate,       \
                              PKT_SEP_CHAR,      \
                              data->direction,    \
                              PKT_END_CHAR
                              
#define REMOTE_TO_BASE_FMT "%s%i%s" // params below.
#define REMOTE_TO_BASE_PARAMS PKT_START_CHAR,           \
                              data->photoresistor_value, \
                              PKT_END_CHAR

#define BASE_TO_REMOTE_POINTER_FMT "^%i/%i/%i/%i/%i$" // params below.
#define BASE_TO_REMOTE_POINTER_PARAMS &data->laser_power, \
                                      &data->turret_x_pos, \
                                      &data->turret_y_pos, \
                                      &data->rotate,       \
                                      &data->direction

#define REMOTE_TO_BASE_POINTER_FMT "^%i$" // params below.
#define REMOTE_TO_BASE_POINTER_PARAMS &data->photoresistor_value

typedef struct 
{
    bool laser_power; // True for ON, false for OFF
    int turret_x_pos; // Up/Down
    int turret_y_pos; // Left/Right
    int rotate;       // Rotation of the roomba - positive indicates CW and negative indicates CCW
    int direction;    // Linear movment of the roomba - Positive for forward, negative for backward
} Base_To_Remote_Pkt_T;

typedef struct 
{
    int photoresistor_value; // Photoresistor Voltage Value
} Remote_To_Base_Pkt_T;

char *base_to_remote_struct_to_string(Base_To_Remote_Pkt_T *data) // This param must be named "data" in order to use macro above
{   
    char *string = (char *)malloc(64);
    
    int size = sprintf(string, BASE_TO_REMOTE_FMT, BASE_TO_REMOTE_PARAMS);
    string = (char *)realloc(string, size + 1); // +1 for null terminator
    return string;
}

char *remote_to_base_struct_to_string(Remote_To_Base_Pkt_T *data) // This param must be named "data" in order to use macro above
{   
    char *string = (char *)malloc(64);
    
    int size = sprintf(string, REMOTE_TO_BASE_FMT, REMOTE_TO_BASE_PARAMS);
    string = (char *)realloc(string, size + 1); // +1 for null terminator
    return string;
}

int base_to_remote_string_to_struct(char *string, Base_To_Remote_Pkt_T *data) // Must stay named "data" to work with macros
{
    char *start_tok = strstr(string, PKT_START_CHAR);
    char *end_tok   = strstr(string, PKT_END_CHAR);
    
    if(!start_tok || !end_tok)
        return -1;
    
    int str_length = end_tok - start_tok + 1; // +1 because pointer math. This length does not include null terminator
    char *trimmed_string = (char *)malloc(str_length + 1); // +1 for null terminator
    
    memcpy(trimmed_string, start_tok, str_length);
    trimmed_string[str_length] = 0x00;
    
    sscanf(trimmed_string, BASE_TO_REMOTE_POINTER_FMT, BASE_TO_REMOTE_POINTER_PARAMS);
}

int remote_to_base_string_to_struct(char *string, Remote_To_Base_Pkt_T *data) // Must stay named "data" to work with macros
{
    char *start_tok = strstr(string, PKT_START_CHAR);
    char *end_tok   = strstr(string, PKT_END_CHAR);
    
    if(!start_tok || !end_tok)
        return -1;
    
    int str_length = end_tok - start_tok + 1; // +1 because pointer math. This length does not include null terminator
    char *trimmed_string = (char *)malloc(str_length + 1); // +1 for null terminator
    
    memcpy(trimmed_string, start_tok, str_length);
    trimmed_string[str_length] = 0x00;
    
    sscanf(trimmed_string, REMOTE_TO_BASE_POINTER_FMT, REMOTE_TO_BASE_POINTER_PARAMS);
}

char *pretty_print_remote_to_base_struct(Remote_To_Base_Pkt_T *data)
{
    char *string = (char *)malloc(256);
    
    int size = sprintf(string, "Photoresistor: %i\n\n", data->photoresistor_value);
    
    string = (char *)realloc(string, size + 1);
    
    return string;
}

char *pretty_print_base_to_remote_struct(Base_To_Remote_Pkt_T *data)
{
    char *string = (char *)malloc(256);
    
    int size = sprintf(string, "Laser Power:  %i\n"
                               "Turret X Pos: %i\n"
                               "Turret Y Pos: %i\n"
                               "Rotation:     %i\n"
                               "Direction:    %i\n\n", data->laser_power, data->turret_x_pos, data->turret_y_pos, data->rotate, data->direction);
    
    string = (char *)realloc(string, size + 1);
    
    return string;
}
