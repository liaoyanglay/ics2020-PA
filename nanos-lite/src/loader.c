#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern uint8_t ramdisk_start;

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr *ehdr = (Elf_Ehdr *) &ramdisk_start;
  Elf_Phdr *ph = (Elf_Phdr *) ((uint8_t *) ehdr + ehdr->e_phoff);

  for (int i = 0; i < ehdr->e_phnum; i++, ph++) {
    if (ph->p_type != PT_LOAD) continue;
    ramdisk_read((void *) ph->p_vaddr, ph->p_offset, ph->p_filesz);
    uint8_t *bss_start = (uint8_t *) ph->p_vaddr + ph->p_filesz;
    uint8_t *bss_end = (uint8_t *) ph->p_vaddr + ph->p_memsz;
    for (uint8_t *byte = bss_start; byte < bss_end; byte++) {
      *byte = 0;
    }
  }
  return ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

