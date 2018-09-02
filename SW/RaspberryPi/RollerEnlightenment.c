#include <stdio.h>
#include <unistd.h>

///from serial example
#include <errno.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <string.h>
#include <termios.h>




void setup_ant();
void send_msg(int fd, unsigned char bd[],unsigned char bd_size);
unsigned char checksum_calc(unsigned char pay_size, unsigned char bd[]);
void send_speed();
void send_cad_pow();
void send_general();
int get_power();
int get_speed();
int get_cadence();
int get_sensor();
int set_interface_attribs(int fd, int speed);
int serial_open();

int main()
{
   int a,b;
   printf("Initializing serial connection\n");
   int fd = serial_open();
   printf("Initializing ANT connection\n");
   setup_ant(fd);
   printf("Beginning Transmission\n");
   while (1) {
     for (b=0;b<2;b++){
       for (a=0;a<2;a++){ 
         send_speed(fd);
         usleep(250000);
       }
       for (a=0;a<2;a++){
         send_cad_pow(fd);
         usleep(250000);
       }
     }
     send_general(fd);
     usleep(250000);
   }
   return 0;
}

///packet formation
void send_speed(int fd){
    printf("Send_speed:\t\t");
    int speed_val = get_speed();
    unsigned char elapse_time = 0x00;
    unsigned char dist_tr = 0x00;
    unsigned char speed_MSB = (unsigned char) (speed_val/256);
    unsigned char speed_LSB = (unsigned char) (speed_val - 256.0*(float)speed_MSB);;
    unsigned char HR = 0x00;
    unsigned char data_ar[] = {0x4E,0x00,0x10,elapse_time,dist_tr,speed_LSB,speed_MSB,HR,0x03};
    send_msg(fd, data_ar,9);
    
}

void send_cad_pow(int fd){
    printf("Send_cad_pow:\t\t");
    int power_val = get_power();
    int cadence_val = get_cadence();
    unsigned char RPM = (unsigned char) cadence_val;
    unsigned char power_MSB = (unsigned char) (power_val/256);
    unsigned char power_LSB = (unsigned char) (power_val - 256.0*(float)power_MSB);;
    unsigned char data_ar[] = {0x4E,0x00,0x15,0xFF,0xFF,0xFF,RPM,power_LSB,power_MSB,0x03};
    send_msg(fd, data_ar,10);
    
}

void send_general(int fd){
    printf("Send_general:\t\t");
    unsigned char data_ar[] = {0x4E,0x00,0x11,0xFF,0xFF,0x23,0x00,0x44,0x58,0x03};
    send_msg(fd, data_ar,10);
    
    
}

///ANT device setup
void setup_ant(int fd) {
  unsigned char data_ar1[] = {0x4A,0x00};
  send_msg(fd, data_ar1,2);
  usleep(250000);
  unsigned char data_ar2[] = {0x4D,0x00,0x54};
  send_msg(fd, data_ar2,3);
  usleep(250000);
  unsigned char data_ar3[] = {0x46,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  send_msg(fd, data_ar3,10);
  usleep(250000);
  unsigned char data_ar4[] = {0x46,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  send_msg(fd, data_ar4,10);
  usleep(250000);
  unsigned char data_ar5[] = {0x46,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  send_msg(fd, data_ar5,10);
  usleep(250000);
  unsigned char data_ar6[] = {0x46,0x00,0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45};
  send_msg(fd, data_ar6,10);
  usleep(250000);
  unsigned char data_ar7[] = {0x42,0x00,0x10,0x00};
  send_msg(fd, data_ar7,4);
  usleep(250000);
  unsigned char data_ar8[] = {0x51,0x00,0x01,0x02,0x11,0x10};
  send_msg(fd, data_ar8,6);
  usleep(250000);
  unsigned char data_ar9[] = {0x44,0x00,0xFF};
  send_msg(fd, data_ar9,3);
  usleep(250000);
  unsigned char data_ar10[] = {0x43,0x00,0x86,0x1F};
  send_msg(fd, data_ar10,4);
  usleep(250000);
  unsigned char data_ar11[] = {0x45,0x00,0x39};
  send_msg(fd, data_ar11,3);
  usleep(250000);
  unsigned char data_ar12[] = {0x4B,0x00};
  send_msg(fd, data_ar12,2);
}

///packet transmission
void send_msg(int fd, unsigned char bd[],unsigned char bd_size){
  int wlen;
  unsigned char buf[80];
  int buf_size, a;
  unsigned char pay_size = bd_size-1;
  unsigned char check_val = checksum_calc(pay_size,bd);
  
  printf("%02x",0xA4); //send the sync
  printf("%02x",pay_size); //send the payload size
  printf("%02x%02x",bd,bd_size);//send payload
  printf("%02x\n",check_val); //send checksum


  buf[0] = 0xA4;
  buf[1] = pay_size;
  for (a=0;a<bd_size;a++){
    buf[a+2] = bd[a];
  }
  buf[a+bd_size] = check_val;
  buf_size = bd_size + 3;

  wlen = write(fd, buf, buf_size);
  if (wlen != buf_size) {
    printf("Error from write: %d, %d\n", wlen, errno);
  }
  

}

///checksum calculation
unsigned char checksum_calc(unsigned char pay_size, unsigned char bd[]){
  unsigned char cv = 0xA4;
  int a;
  cv = (cv ^ pay_size) % 0xFF;
  for (a = 0; a<pay_size+1; a++){
    cv = (cv ^ bd[a]) % 0xFF;
  }

  return  cv;
}




int get_power(){
  int sensor = get_sensor();
  return sensor;
}

int get_speed(){
  int sensor = get_sensor();
  return sensor;
}

int get_cadence(){
  int sensor = get_sensor();
  return sensor;
}


int get_sensor(){
  return 0;
}




///from serial example
int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}


int serial_open()
{
  char *portname = "/dev/ttyUSB0";
  int fd;
  fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    printf("Error opening %s: %s\n", portname, strerror(errno));
    return -1;
  }
  set_interface_attribs(fd, B9600);
  return fd;
}





