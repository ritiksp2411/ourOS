#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager{
    
    protected:
        struct GateDescriptor{
            uint16_t handlerAddressLowBits;
            uint16_t gdt_codeSegmentSelector;
            uint8_t reserved;
            uint8_t access;
            uint16_t handlerAddressHighBits;
        }__attribute__((packed));

        static GateDescriptor interruptDescriptorTable[256];

        struct interruptDescriptorTablePointer{
            uint16_t size;
            uint16_t base;
        }__attribute__((packed));

        static void setInterruptDescriptorTableEntry(
            uint8_t interruptNumber,
            uint16_t codeSegmentSelectorOffset,
            void (*handler)(),                   //pointer to interrupt handler
            uint8_t DescriptorPrivilegeLevel,   //access rights
            uint8_t DescriptorType              //flags
        );
    
        Port8BitSlow picMasterCommand;
        Port8BitSlow picMasterData;
        Port8BitSlow picSlaveCommand;
        Port8BitSlow picSlaveData;
    
    public:
        InterruptManager(GlobalDescriptorTable* gdt);
        ~InterruptManager();

        void Activate();

        static uint32_t handleInterrupt(uint8_t interruptNumber,uint32_t esp);
        static void IgnoreInterruptRequest();
        static void HandleInterruptRequest0x01();
        static void HandleInterruptRequest0x00();

};

#endif