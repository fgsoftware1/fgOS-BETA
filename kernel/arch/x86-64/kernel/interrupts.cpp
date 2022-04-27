#include "include/interrupts.hpp"
#include "include/console.hpp"
#include "kernel_utils.hpp"
#include "include/gdt.hpp"
#include "include/isrs.hpp"
#include "include/irqs.hpp"
#include "include/stl/types.hpp"

namespace
{
  struct idt_entry
  {
    uint16_t offset_low;
    uint16_t segment_selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
  } __attribute__((packed));

  static_assert(sizeof(idt_entry) == 16, "The size of an IDT entry should be 16 bits");

  struct idtr
  {
    uint16_t limit;
    uint64_t base;
  } __attribute__((packed));

  idt_entry idt_64[64];
  idtr idtr_64;

  void idt_set_gate(size_t gate, void (*function)(void), uint16_t gdt_selector, uint8_t flags)
  {
    auto &entry = idt_64[gate];

    entry.segment_selector = gdt_selector;
    entry.flags = flags;
    entry.reserved = 0;
    entry.zero = 0;

    auto function_address = reinterpret_cast<uintptr_t>(function);
    entry.offset_low = function_address & 0xFFFF;
    entry.offset_middle = (function_address >> 16) & 0xFFFF;
    entry.offset_high = function_address >> 32;
  }

  void (*irq_handlers[16])();

  uint16_t get_cr2()
  {
    uint16_t value;
    __asm__ __volatile__("mov rax, cr2; mov %0, rax;"
                         : "=m"(value));
    return value;
  }
}

struct regs
{
  uint64_t error_no;
  uint64_t error_code;
  uint64_t rip;
  uint64_t rflags;
  uint64_t cs;
  uint64_t rsp;
  uint64_t ss;
} __attribute__((packed));

extern "C"
{

  void _fault_handler(regs regs)
  {
    k_printf("Exception (%u) occured\n", regs.error_no);
    k_printf("error_code=%u\n", regs.error_code);
    k_printf("rip=%h\n", regs.rip);
    k_printf("rflags=%h\n", regs.rflags);
    k_printf("cs=%h\n", regs.cs);
    k_printf("rsp=%h\n", regs.rsp);
    k_printf("ss=%h\n", regs.ss);
    k_printf("cr2=%h\n", get_cr2());

    // TODO Improve that with kind of blue screen
    __asm__ __volatile__("hlt"
                         :
                         :);
  }

  void _irq_handler(size_t code)
  {
    // If there is an handler, call it
    if (irq_handlers[code])
    {
      irq_handlers[code]();
    }

    // If the IRQ is on the slaved controller, send EOI to it
    if (code >= 8)
    {
      out_byte(0xA0, 0x20);
    }

    // Send EOI to the master controller
    out_byte(0x20, 0x20);
  }
}

void interrupt::install_idt()
{
  // Set the correct values inside IDTR
  idtr_64.limit = (64 * 16) - 1;
  idtr_64.base = reinterpret_cast<size_t>(&idt_64[0]);

  // Clear the IDT
  std::fill_n(reinterpret_cast<size_t *>(idt_64), 64 * sizeof(idt_entry) / sizeof(size_t), 0);

  // Clear the IRQ handlers
  std::fill_n(irq_handlers, 16, nullptr);

  // Give the IDTR address to the CPU
  __asm__ __volatile__("lidt [%0]"
                       :
                       : "m"(idtr_64));
}

void interrupt::install_isrs()
{
  idt_set_gate(0, _isr0, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(1, _isr1, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(2, _isr2, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(3, _isr3, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(4, _isr4, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(5, _isr5, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(6, _isr6, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(7, _isr7, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(8, _isr8, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(9, _isr9, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(10, _isr10, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(11, _isr11, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(12, _isr12, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(13, _isr13, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(14, _isr14, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(15, _isr15, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(16, _isr16, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(17, _isr17, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(18, _isr18, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(19, _isr19, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(20, _isr20, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(21, _isr21, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(22, _isr22, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(23, _isr23, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(24, _isr24, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(25, _isr25, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(26, _isr26, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(27, _isr27, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(28, _isr28, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(29, _isr29, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(30, _isr30, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(31, _isr31, gdt::LONG_SELECTOR, 0x8E);
}

void interrupt::remap_irqs()
{
  // Restart the both PICs
  out_byte(0x20, 0x11);
  out_byte(0xA0, 0x11);

  out_byte(0x21, 0x20); // Make PIC1 start at 32
  out_byte(0xA1, 0x28); // Make PIC2 start at 40

  // Setup cascading for both PICs
  out_byte(0x21, 0x04);
  out_byte(0xA1, 0x02);

  // 8086 mode for both PICs
  out_byte(0x21, 0x01);
  out_byte(0xA1, 0x01);

  // Activate all IRQs in both PICs
  out_byte(0x21, 0x0);
  out_byte(0xA1, 0x0);
}

void interrupt::install_irqs()
{
  idt_set_gate(32, _irq0, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(33, _irq1, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(34, _irq2, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(35, _irq3, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(36, _irq4, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(37, _irq5, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(38, _irq6, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(39, _irq7, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(40, _irq8, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(41, _irq9, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(42, _irq10, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(43, _irq11, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(44, _irq12, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(45, _irq13, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(46, _irq14, gdt::LONG_SELECTOR, 0x8E);
  idt_set_gate(47, _irq15, gdt::LONG_SELECTOR, 0x8E);
}

void interrupt::register_irq_handler(size_t irq, void (*handler)())
{
  irq_handlers[irq] = handler;
}

void interrupt::enable_interrupts()
{
  __asm__ __volatile__("sti"
                       :
                       :);
}
