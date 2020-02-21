void printf(char* str){              //we have to define our own printf function as there are no libraries yet
    unsigned short* VideoMemory = (unsigned short*)0xb8000; //OS dynamically links the function call to the library

    for(int i=0; str[i]!='\0';++i)
        VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){         //to initialise objects 
    for(constructor* i = &start_ctors;i!=&end_ctors;i++)
        (*i)();

}


extern "C" void kernelMain(void* multiboot_structure, unsigned int magicnumber){ //g++ has a different naming convention\
                                                                                   hence while writing into .o file changes name\
    printf("loaded successfully ");                                                to prevent that extern is used
    while(1);
}