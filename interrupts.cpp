#include "interrupts.h"
#include "types.h"
#include "port.h"

void print(char* str);

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

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

    for(uint16_t i=0;i<256;i++) //all interrupts have to be ignored
        setInterruptDescriptorTableEntry(i,CodeSegment,&IgnoreInterruptRequest,0,IDT_INTERRUPT_GATE);
        //if an interrupt occurs which we haven't considered yet, can lead to the OS crashing

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
    asm("sti");
}


uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber,uint32_t esp)
{
    print("Interrupt");
    return esp;
}