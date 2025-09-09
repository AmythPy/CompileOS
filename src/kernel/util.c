void* memset(void* dest,int val,unsigned long len){unsigned char* ptr=dest;while(len--) *ptr++=(unsigned char)val;return dest;}
