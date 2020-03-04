#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21,manager),
dataport(0x60),
commandport(0x64){
    while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xAE);
    commandport.Write(0x20);
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60);
    dataport.Write(status);

    dataport.Write(0xF4);
}

void print(char*);

KeyboardDriver::~KeyboardDriver(){

}
uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){
    uint8_t key=dataport.Read();

    char* foo = "Keyboard 0x00";
        char* hex = "0123456789ABCDEF";
        foo[11] = hex[(key>>4) & 0x0F]; 
        foo[12] = hex[key & 0x0F];
        print(foo);

    return esp;
}