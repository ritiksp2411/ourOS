#include "interrupts.h"
#include "types.h"
#include "port.h"

using namespace std;

void print(char* str);


InterruptHandler::InterruptHandler(uint8_t interruptNumber,InterruptManager* interruptManager)
{
    this->interruptNumber=interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber]=this;
}
InterruptHandler::~InterruptHandler(){
    if(interruptManager->handlers[interruptNumber]==this)
        interruptManager->handlers[interruptNumber]=0;
}
uint32_t InterruptHandler::HandleInterrupt(uint32_t esp){
    return esp;
}

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager* InterruptManager::ActiveInterruptManager = 0;

void InterruptManager::setInterruptDescriptorTableEntry(
            uint8_t interruptNumber,
            uint16_t codeSegmentSelectorOffset,
            void (*handler)(),                   //pointer to interrupt handler
            uint8_t DescriptorPrivilegeLevel,   //access rights
            uint8_t DescriptorType              //flags
        )
        {
            const uint8_t IDT_DESC_PRESENT = 0x80;
            interruptDescriptorTable[interruptNumber].handlerAddressLowBits=((uint32_t)handler) & 0xFFFF;
            interruptDescriptorTable[interruptNumber].handlerAddressHighBits=(((uint32_t)handler)>>16) & 0xFFFF;
            interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
            interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel&3)<<5);
            interruptDescriptorTable[interruptNumber].reserved = 0;
        };

InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
: picMasterCommand(0x20),
  picMasterData(0x21),
  picSlaveCommand(0xA0),
  picSlaveData(0xA1)
{

    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for(uint16_t i=0;i<256;i++){ //all interrupts have to be ignored
        handlers[i]=0;
        setInterruptDescriptorTableEntry(i,CodeSegment,&IgnoreInterruptRequest,0,IDT_INTERRUPT_GATE);
        }//if an interrupt occurs which we haven't considered yet, can lead to the OS crashing

    setInterruptDescriptorTableEntry(0x20,CodeSegment,&HandleInterruptRequest0x00,0,IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x21,CodeSegment,&HandleInterruptRequest0x01,0,IDT_INTERRUPT_GATE);

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);
    picMasterData.Write(0x20);
    picSlaveData.Write(0x20);

    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);

    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);

    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);

    

    interruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));  //tell processor to load idt table
    
}

InterruptManager::~InterruptManager(){

}

void InterruptManager::Activate(){  //to inform the processor that it can start sending interrupts
    if(ActiveInterruptManager!=0)   //not more than 1 interrupt can occur at a time
        ActiveInterruptManager->Deactivate();
    
    ActiveInterruptManager = this;
    asm("sti");
}

void InterruptManager::Deactivate(){  
    if(ActiveInterruptManager==this){   
        
        ActiveInterruptManager = 0;
        asm("cli");}
    }


uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber,uint32_t esp)
{
    if(ActiveInterruptManager!=0){
        return ActiveInterruptManager->DoHandleInterrupt(interruptNumber,esp);
    }
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber,uint32_t esp){
    
    if(handlers[interruptNumber]!=0)
        esp =  handlers[interruptNumber]->HandleInterrupt(esp);
    else if(interruptNumber!=0x20){
        char* foo = "UNHANDLED INTERRUPT 0x00";
        char* hex = "0123456789ABCDEF";
        foo[22] = hex[(interruptNumber>>4) & 0x0F]; 
        foo[23] = hex[interruptNumber & 0x0F];
        print(foo);
    }
        

    if(0x20 <= interruptNumber && interruptNumber <= 0x30){
        picMasterCommand.Write(0x20);
        if(0x28 <= interruptNumber )
            picSlaveCommand.Write(0x20);
    }



    return esp;
}
