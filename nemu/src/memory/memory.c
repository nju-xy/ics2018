#include "nemu.h"
#include "memory/mmu.h"
extern uint32_t mmio_read(paddr_t addr, int len, int map_NO);
extern void mmio_write(paddr_t addr, int len, uint32_t data, int map_NO);
extern int is_mmio(paddr_t addr);

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

paddr_t page_translate(vaddr_t vaddr);

uint32_t paddr_read(paddr_t addr, int len) {
	static int map_NO;
    map_NO = is_mmio(addr);
	if(map_NO != -1)
		return mmio_read(addr, len, map_NO);
	else 
		return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
	static int map_NO;
    map_NO = is_mmio(addr);
	if(map_NO != -1)
		mmio_write(addr, len, data, map_NO);
	else 
		memcpy(guest_to_host(addr), &data, len);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
	//return paddr_read(addr, len);
	//TODO
	if (addr / PAGE_SIZE != (addr + len - 1) / PAGE_SIZE) {//data cross the page boundary
		/* this is a special case, you can handle it later. */
		//Log("addr = %x, len = %x", addr, len);
		//Log("read cross the page boundary");
		assert((addr + len - 1) / PAGE_SIZE == addr / PAGE_SIZE + 1);
		vaddr_t addr1 = (addr / PAGE_SIZE + 1) * PAGE_SIZE;
		int len1 = addr1 - addr;
		int len2 = len - len1;
		//Log("len1 = %d, len2 = %d, addr1 = %x", len1, len2, addr1);
		//Log("%x %x %x", vaddr_read(addr, len1), vaddr_read(addr1, len2),  vaddr_read(addr1, len2) + (vaddr_read(addr, len1) << (len2 * 8)));
		//return vaddr_read(addr1, len2) + (vaddr_read(addr, len1) << (len2 * 8));
		return vaddr_read(addr, len1) + (vaddr_read(addr1, len2) << (len1 * 8));
	}
	else {
		paddr_t paddr = page_translate(addr);
		return paddr_read(paddr, len);
	}
	return 0;


}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
	//TODO
	if (addr / PAGE_SIZE != (addr + len - 1) / PAGE_SIZE) {//data cross the page boundary
		/* this is a special case, you can handle it later. */
		//untest code !!!
		//Log("addr = %x, len = %x", addr, len);
		//Log("write cross the page boundary");
		assert((addr + len - 1) / PAGE_SIZE == addr / PAGE_SIZE + 1);
		vaddr_t addr1 = (addr / PAGE_SIZE + 1) * PAGE_SIZE;
		int len1 = addr1 - addr;
		int len2 = len - len1;
		uint32_t data1 = (data >> (len1 * 8));
		uint32_t data2 = ((data << (len2 * 8)) >> (len2 * 8));
		vaddr_write(addr, len1, data1);
		vaddr_write(addr1, len2, data2);
	}
	else {
		paddr_t paddr = page_translate(addr);
		paddr_write(paddr, data, len);
	}
}

paddr_t page_translate(vaddr_t vaddr) {
	//TODO
	if(cpu.cr0.paging) {
		//Log("paging on");
		uint32_t dir = vaddr >> 22;
		uint32_t page = (vaddr >> 12) & 0x3ff;
		uint32_t offset = vaddr	& 0xfff;
		//Log("vaddr = %x, dir = %x, page = %x, offset = %x, page_directory_base = %x", vaddr, dir, page, offset, cpu.cr3.page_directory_base);
		//PDE *pde = (PDE*)((uintptr_t)(cpu.cr3.page_directory_base << 12) + (uintptr_t)(dir << 2));
		PDE pde;
	   	pde.val = paddr_read((cpu.cr3.page_directory_base << 12) + (dir << 2), 4);
		//Log("pde.val = %x", pde.val);
		//Log("vaddr = %x, page_directory_base = %x, dir = %x, page = %x, offset = %x", vaddr, cpu.cr3.page_directory_base, dir, page, offset);
		if(pde.present == 0)
			Log("vaddr = %x, dir = %x, page = %x, offset = %x, page_directory_base = %x, pde = %x", vaddr, dir, page, offset, cpu.cr3.page_directory_base, pde.val);
		assert(pde.present == 1);
		//Log("pde.page_frame = %x", pde.page_frame);
		PTE pte;
	    pte.val	= paddr_read((pde.page_frame << 12) + (page << 2), 4);
		//PTE *pte = (PTE*)((uintptr_t)(pde->page_frame << 12) + (uintptr_t)(page << 2));
		if(pte.present == 0)
			Log("vaddr = %x, dir = %x, page = %x, offset = %x, page_directory_base = %x, pte = %x", vaddr, dir, page, offset, cpu.cr3.page_directory_base, pde.val);
		assert(pte.present == 1);
		//Log("page_frame is %x", pte.page_frame);
		return ((pte.page_frame << 12) + offset);
	}
	else {
		//Log("paging off");
		return (paddr_t)vaddr;
	}
}
