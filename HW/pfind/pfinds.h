#ifndef PFIND_H_
#define PFIND_H_

bool verify_permission_string(char *str);
int parse(int argc, char **argv, char **directory, char **permission_string);
bool file_permission(char *permission_string, mode_t file_mode);
int get_matching_permissions(char *directory, char *permission_string);
                   
#endif
