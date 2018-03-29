unsigned long long strlen_unsigned(const unsigned char * buffer){
   unsigned long long result = 0;
   while(*buffer++) result++;
   return result;
}
unsigned short csum(unsigned short *buf, int nwords)
{       //
        unsigned long sum;
        for(sum=0; nwords>0; nwords--)
                sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}
