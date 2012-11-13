#include "script.h"
#include "serial.h"
#include <fsaccess.h>

#include "screen.h" //TODO debug only
int load_script(char *script_path) {
	char c_buffer;
	int i,fd,line;
	char delay_buffer[SCRIPT_TRANSFER_DELAY_MAX_LENGTH];

	// Empty script strings TODO nødvendig?
//	for (i = 0; i < DEFAULT_STRING_MAX_LENGTH; ++i) {
//		selected_script.description[i] 			= '\0';
//		selected_script.fpga_bin_path[i] 		= '\0';
//		selected_script.data_type_directory[i] 	= '\0';
//		if (i<SCRIPT_TRANSFER_DELAY_MAX_LENGTH) {
//			delay_buffer[i] = '\0';
//		}
//	}

	// Open script file
	fd = open(script_path, O_RDONLY);
	if (fd < 0) {
		seprintf("Could not open script: %s\n", script_path);
		return fd;
	}

	// Read script file
	for (i=0, line=0; read(fd, &c_buffer, 1) > 0; ++i) {

		if (c_buffer == '\n') {

			// End line
			switch (line) {
			case SCRIPT_LINE_DESCRIPTION:
				selected_script.description[i] = '\0';
				break;
			case SCRIPT_LINE_FPGA_BIN_PATH:
				selected_script.fpga_bin_path[i] = '\0';
				break;
			case SCRIPT_LINE_DATA_TYPE_DIR:
				selected_script.data_type_directory[i] = '\0';
				break;
			case SCRIPT_LINE_TRANSFER_DELAY:
				delay_buffer[i] = '\0';
				break;
			}

			line++;			// Go to next line in script file
			i=0; 			// Reset character counter
			continue;		// Skip to next byte
		}

		// Read lines in script file
		switch (line) {
			case SCRIPT_LINE_DESCRIPTION:
			{
				if (i < DEFAULT_STRING_MAX_LENGTH) {
					selected_script.description[i] = c_buffer;
					//screen_display_error_messagef(selected_script.description);
				}
				break;
			}
			case SCRIPT_LINE_FPGA_BIN_PATH:
			{
				if (i >= DEFAULT_STRING_MAX_LENGTH) {
					//seprintf("Error: FPGA binary file path in script too long (max %d characters). Loading failed.\n", DEFAULT_STRING_MAX_LENGTH);
					return -1;
				}
				selected_script.fpga_bin_path[i] = c_buffer; //TODO klarer å lese alle linjene fra kortet, men klarer kun å lagre den første linja i stringen
				//screen_display_error_messagef(selected_script.fpga_bin_path);
				break;
			}
			case SCRIPT_LINE_DATA_TYPE_DIR:
			{
				if (i >= DEFAULT_STRING_MAX_LENGTH) {
					//seprintf("Error: Data type path in script too long (max %d characters). Loading failed.\n", DEFAULT_STRING_MAX_LENGTH);
					return -1;
				}
				selected_script.data_type_directory[i] = c_buffer;
				break;
			}
			case SCRIPT_LINE_TRANSFER_DELAY:
			{
				if (i >= DEFAULT_STRING_MAX_LENGTH) {
					//seprintf("Error: Transfer delay in script too long (max %d characters). Loading failed.\n", DEFAULT_STRING_MAX_LENGTH);
					return -1;
				}
				delay_buffer[i] = c_buffer;
				break;
			}
//			default: // Unexpected line
//			{
//				//seprintf("Warning: Unexpected line detected in script %s\n", script_path);
//				//goto no_more_lines; // Breaks out of for loop
//			}
		}
	}
	close(fd);
	//no_more_lines:
	selected_script.transfer_delay = atoi(delay_buffer);
	return 0;
}

//void test_load_script(char *path) {
//	load_script(path);
//	seprintf("Loaded contents of %s:", path);
//	seprintf("Description: \t%s\n", selected_script.description);
//	seprintf("FPGA bin path:\t%s\n", selected_script.fpga_bin_path);
//	seprintf("Data type dir:\t%s\n", selected_script.data_type_directory);
//	seprintf("Transfer delay:\t%d\n", selected_script.transfer_delay);
//}
