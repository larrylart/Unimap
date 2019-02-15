#include <stdlib.h>

unsigned char test1[] = { 0x3b, 0x05, 0x10, 0x04, 0xfe, 0x04, 0x03, 0xe2 };
unsigned char test2[] = { 0x3b, 0x03, 0x04, 0x10, 0xfe, 0xeb };
unsigned char test3[] = { 0x3b, 0x03, 0x04, 0x10, 0x01, 0xe8 };
unsigned char test4[] = { 0x3b, 0x06, 0x10, 0x04, 0x01, 0x02, 0x7d, 0xc6, 0xa0 };
unsigned char test5[] = { 0x3b, 0x03, 0x03, 0x04, 0xfe, 0x00 };

unsigned char calc_nexstar_checksum(unsigned char* buf, int len)
{
  unsigned char cksum;
  cksum = 0;
  while (len--)
  {
    cksum+=*buf;
    buf++;
  }
  return -cksum;
}

void main()
{
  printf("0x%02x 0x%02x\n",calc_nexstar_checksum(&test1[1], test1[1]+1), 
                           test1[sizeof(test1)-1]);
  printf("0x%02x 0x%02x\n",calc_nexstar_checksum(&test2[1], test2[1]+1),
                           test2[sizeof(test2)-1]);
  printf("0x%02x 0x%02x\n",calc_nexstar_checksum(&test3[1], test3[1]+1),
                           test3[sizeof(test3)-1]);
  printf("0x%02x 0x%02x\n",calc_nexstar_checksum(&test4[1], test4[1]+1),
                           test4[sizeof(test4)-1]);
  printf("0x%02x 0x%02x\n",calc_nexstar_checksum(&test5[1], test5[1]+1),
                           test5[sizeof(test5)-1]);
}
