/*
Copyright (C) 2019 Benjamin Larsson <banan@ludd.ltu.se>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <getopt.h>
#include <time.h>


#define CBC 1
#define CTR 1

#include "aes.h"

#define TTYDEV "/dev/ttyACM0"
#define BAUDRATE B38400

unsigned char aes_key[] = {0x0A,0xA1,0xD8,0x28,0xD9,0xF6,0x80,0x37,0x57,0x88,0x1B,0xE9,0x2A,0x64,0x11,0x72};

void print_usage() {

}

int open_device(char* tty_device) {
    struct termios tio;
    struct termios stdio;
    struct termios old_stdio;
    int tty_fd_l;

    memset(&stdio,0,sizeof(stdio));
    stdio.c_iflag=0;
    stdio.c_oflag=0;
    stdio.c_cflag=0;
    stdio.c_lflag=0;
    stdio.c_cc[VMIN]=1;
    stdio.c_cc[VTIME]=0;

    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;
    if((tty_fd_l = open(tty_device , O_RDWR | O_NONBLOCK)) == -1){
        printf("Error while opening %s\n", tty_device); // Just if you want user interface error control
        exit(EXIT_FAILURE);
    }
    cfsetospeed(&tio,BAUDRATE);
    cfsetispeed(&tio,BAUDRATE);            // baudrate is declarated above
    tcsetattr(tty_fd_l,TCSANOW,&tio);

    return tty_fd_l;
}

void print_hex(unsigned char *buf, int buf_size) {
    int i;
    for (i=0 ; i<buf_size ; i++) {
        if (!(i%16)) printf("\n");
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

void parse_wmbus_payload(unsigned char *arr, int cnt){
    int i,j;
    struct AES_ctx ctx;
    unsigned char iv[16];

    printf("L-field:  %02x\n", arr[0]);
    printf("C-field:  %02x\n", arr[1]);
    printf("M-field:  %02x%02x\n", arr[2], arr[3]);
    printf("IdentNum: %02x%02x%02x%02x\n",arr[7],arr[6],arr[5],arr[4]);
    printf("Version:  %02x\n", arr[8]);
    printf("Type:     %02x\n", arr[9]);
    //printf("CRC:      %02x%02x\n", arr[11], arr[10]);
    printf("CI-field: %02x\n", arr[10]);
    printf("AC:       %02x\n", arr[11]);
    printf("ST:       %02x\n", arr[12]);
    printf("CW0:      %01x %01x\n", (arr[13])>>4, arr[13]&0xF);
    printf("CW1:      %01x %01x\n", (arr[14])>>4, arr[14]&0xF);
    printf("DIF:      %02x\n", arr[15]);
    printf("VIF:      %02x\n", arr[16]);
    printf("Instantaneous Value: %02x%02x : %f\n",arr[17],arr[18],(arr[18]<<8|arr[17])*0.01);
    printf("DIF:      %02x\n", arr[19]);
    printf("VIF:      %02x\n", arr[20]);
    printf("Old Value:%02x%02x\n",arr[21],arr[22]);
    printf("DIF:      %02x\n",arr[23]);
    printf("DIFE:     %02x\n",arr[24]);
    printf("VIF:      %02x\n", arr[25]);
    printf("Value 3:  %02x%02x\n",arr[26],arr[27]);
    printf("DIF:      %02x\n", arr[28]);
    printf("VIF:      %02x\n", arr[29]);
    printf("Value 4:  %02x%02x\n",arr[30],arr[31]);
    printf("DIF:      %02x\n", arr[32]);
    printf("VIF:      %02x\n", arr[33]);
    printf("Value 5:  %02x%02x\n",arr[34],arr[35]);
    i=36;
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value 6:  %02x%02x\n",arr[i++],arr[i++]);

    printf("DIF:      %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value 7:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIFE:     %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Humidity: %02x%02x : %f\n",arr[i++],arr[i++], (arr[i+1]<<8|arr[i])*0.1);
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIFE:     %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value 7:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIFE:     %02x\n", arr[i++]);
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIFE:     %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value 8:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIFE:     %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value 9:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIFE:     %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value 10:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIFE:     %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value 11:  %02x%02x\n",arr[i++],arr[i++]);

    memcpy(iv, &arr[2], 8);
    memset(&iv[8], arr[11], 8);
    AES_init_ctx_iv(&ctx, aes_key, iv);
    printf("print_key\n");
    print_hex(aes_key, 16);
    printf("print_iv\n");
    print_hex(iv, 16);

    printf("print_enc\n");
    print_hex(&arr[15], 32);
    AES_CBC_decrypt_buffer(&ctx, &arr[15], 32);
    printf("print_dec\n");
    print_hex(&arr[15], 32);

    i=15;
    printf("ENC:      %02x\n", arr[i++]);
    printf("ENC:      %02x\n", arr[i++]);
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value Dec:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value Dec:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIFE:     %02x\n", arr[i++]);
    printf("DIF:      %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value Dec:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIF:     %02x\n", arr[i++]);
    printf("VIFE:      %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value Dec:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIF:     %02x\n", arr[i++]);
    printf("VIFE:      %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("Value Dec:  %02x%02x\n",arr[i++],arr[i++]);
    printf("DIF:     %02x\n", arr[i++]);
    printf("DIFE:      %02x\n", arr[i++]);
    printf("VIF:      %02x\n", arr[i++]);
    printf("VIFE:      %02x\n", arr[i++]);
    printf("Value Dec:  %02x%02x\n",arr[i++],arr[i++]);
    return;
}

int send_read(int tty_fd_l, int beep, int format, char* output) {
    int i, cnt, idx, csum = 0;
    unsigned char res_arr[256] = {0};
    int last_n;

    

    while(1) {
        usleep(10000);
        memset(res_arr, 0, 256);
        cnt = read(tty_fd_l,&res_arr,256);
        if (cnt > 0) {
            printf("{%04x}\n", cnt);
            for (i=0 ; i<cnt ; i++) {
                printf("%02x ", res_arr[i]);
                if ((i+1)%16==0)
                    printf("\n");
            }
            printf("\n");
            
            parse_wmbus_payload(res_arr, cnt);
        }
        if ((cnt == 0) || ((cnt>0 && cnt!=16)))
            printf("\n");
    }

    return 0;
}



int main(int argc,char** argv)
{
    int option = 0;
    int tty_fd, flags;

    char* tty_device = TTYDEV;
    int test_link = 0;
    int beep = 1;
    int read_device = 0;
    int format = 0;
    int aptus_generate = 0;
    char* write_string = NULL;

    while ((option = getopt(argc, argv,"d:rlbf:w:a")) != -1) {
        switch (option) {
            case 'd' : tty_device = optarg; 
                break;
            case 'l' : test_link = 1;
                break;
            case 'b' : beep = 0;
                break;
            case 'r' : read_device = 1;
                break;
            case 'f' : format = atoi(optarg);
                break;
            case 'w' : write_string = optarg;
                break;
            case 'a' : aptus_generate = 1;
                break;
            default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }
    if (argc < 2) {
        print_usage();
        goto exit;
    }

    tty_fd = open_device(tty_device);


    if (read_device)
        send_read(tty_fd, beep, format, NULL);

exit:
    close(tty_fd);
    return EXIT_SUCCESS;
}

