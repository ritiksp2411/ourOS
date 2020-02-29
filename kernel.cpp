#include "/home/shidhu/ourOS/gdt.h"
#include "/home/shidhu/ourOS/types.h"

void print(char* str){              //we have to define our own printf function as there are no libraries yet
    static uint16_t* VideoMemory = (uint16_t*)0xb8000; //OS dynamically links the function call to the library

    static uint8_t x=0,y=0;         // x and y are the dimensions of the screen 80*25
                                    
    for(int i=0; str[i]!='\0';++i){

        switch(str[i]){
            case '\n': y++;
                        x=0;break;
            default:
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                    x++;break;
        }
            // if number of print statements exceed the screen size, we clear all and start printing again

        if(x>=80){
            y++;
            x=0;
        }
        if(y>=25){
            for(y=0;y<25;y++)
                for(x=0;x<80;x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x=0;
            y=0;
        }
    }
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){         //to initialise objects 
    for(constructor* i = &start_ctors;i!=&end_ctors;i++)
        (*i)();

}


extern "C" void kernelMain(void* multiboot_structure, uint32_t/*unsigned int magicnumber*/){ //g++ has a different naming convention
                                                                                   //hence while writing into .o file changes name
    print("loaded successfully\n");
    print("waiting....\n");
    print("......");    
    GlobalDescriptorTable gdt;                                            //to prevent that extern is used
    while(1);
}