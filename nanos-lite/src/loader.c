#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr;
  Elf_Phdr ph;
  int fd = fs_open(filename, 0, 0);

  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
  if (*(uint32_t *)ehdr.e_ident != 0x464C457F) {
    panic("Load file is not elf format");
  }

  fs_lseek(fd, ehdr.e_phoff, SEEK_SET);
  fs_read(fd, &ph, sizeof(Elf_Phdr));
  for (int i = 0; i < ehdr.e_phnum; i++) {
    if (ph.p_type != PT_LOAD) continue;
    fs_lseek(fd, ph.p_offset, SEEK_SET);
    fs_read(fd, (void *) ph.p_vaddr, ph.p_filesz);
    uint8_t *bss_start = (uint8_t *) ph.p_vaddr + ph.p_filesz;
    uint8_t *bss_end = (uint8_t *) ph.p_vaddr + ph.p_memsz;
    for (uint8_t *byte = bss_start; byte < bss_end; byte++) {
      *byte = 0;
    }
    fs_lseek(fd, ehdr.e_phoff + (i + 1) * sizeof(Elf_Phdr), SEEK_SET);
    fs_read(fd, &ph, sizeof(Elf_Phdr));
  }
  fs_close(fd);
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

