#ifndef _DRU_FPGA_DOWNLOAD_H
#define _DRU_FPGA_DOWNLOAD_H
#define FPGA_FILE_LENGTH 3699963
#define BUFFER_SIZE 3699963
#define WAIT_TIME 200
#define TASK_TIME 1
#define DOWNLOAD_FPGA_CS fpga_cs
#define DOWNLOAD_FPGA_PROG fpga_prog
#define DOWNLOAD_FPGA_INIT fpga_init
#define DOWNLOAD_FPGA_DONE fpga_done
#define DOWNLOAD_FPGA_CLK fpga_clk
#define DOWNLOAD_FPGA_DATA fpga_data

#define FPGA_H 1
#define FPGA_L 0
void dru_fpga_download(unsigned int filelength,char *filename);
#endif
