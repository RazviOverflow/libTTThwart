/*
	About: License

		Whatever

	Authors:

		<Razvan Raducu: https://twitter.com/Razvieu>

		<Ricardo J. Rodríguez: https://twitter.com/RicardoJRdez>
		
*/
#ifndef FILE_GLOBAL_VARIABLES_H_
#define FILE_GLOBAL_VARIABLES_H_

#include <stdbool.h>

/*	Constant: LIBARRY_FOLDER

	Global folder used to create all log files as well as temporal hard links.
*/
#define LIBRARY_FOLDER "/libTOCTTOU"

/*	
	Variable: file_objects_info

	Global <file_objects_info> array that's used to maintain all the information
	about file metadata.
*/
file_objects_info g_array;

/*	
	Bool: LIBRARY_ON

	Global boolean that indicates whether the libary must be used. Bear in mind
	that one of the premises of TOCTTOU vulnerabilities is the attacker has
	lesser privileges than the vulnerable program. This is, the program is
	a setuid-bit-on program. (EUID = UID); where UID = RUID
*/
bool LIBRARY_ON; 

/*
	Char: g_temp_dir

	Globlar variable that indcates the directory of temporal hard links, in case
	there are any. 
*/
char *g_temp_dir; 

#endif