#include "include/types.h";
#include "include/gdt.h";

void printf (char* str) {
  static uint16_t* VideoMemory = (uint16_t*)0xb8000;
  for (int i = 0; str[i] != '\0'; ++i) {
    VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
  }
}

extern "C" void fgosMain (const void* multiboot_structure, uint32_t) {
  printf("Hello World");
  GlobalDescriptorTable gdt;
  while(1);
}
