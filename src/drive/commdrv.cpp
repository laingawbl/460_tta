//
// Created by auriga on 04/04/19.
//

#include "commdrv.h"

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