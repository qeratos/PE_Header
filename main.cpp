#include <iostream>
#include <fstream>
#include <iomanip>
#include <Windows.h>

#define PATH "D:\\Vtable\\virtual_table.exe"

#define Is2power(x) (!(x & (x - 1)))
#define ALIGN_DOWN(x, align) (x & ~(align - 1))
#define ALIGN_UP(x, align) ((x & (align - 1)) ? ALIGN_DOWN(x, align) + align : x)

int main()
{


    std::ifstream pefile;
    pefile.open(PATH, std::ios::in | std::ios::binary);
    if (!pefile.is_open())
    {
        std::cout << "Can't open file" << std::endl;
        return 0;
    }


    pefile.seekg(0, std::ios::end);
    std::streamoff filesize = pefile.tellg();
    pefile.seekg(0);

    IMAGE_DOS_HEADER dos_header;
    pefile.read(reinterpret_cast<char*>(&dos_header), sizeof(IMAGE_DOS_HEADER));
    pefile.seekg(dos_header.e_lfanew);

    IMAGE_NT_HEADERS32 nt_headers;
    pefile.read(reinterpret_cast<char*>(&nt_headers), sizeof(IMAGE_NT_HEADERS32) - sizeof(IMAGE_DATA_DIRECTORY) * 16);

    DWORD first_section = dos_header.e_lfanew + nt_headers.FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(DWORD) /* Signature */;
    pefile.seekg(first_section);

    std::cout << std::hex << std::showbase << std::left;

    for (int i = 0; i < nt_headers.FileHeader.NumberOfSections; i++)
    {
        IMAGE_SECTION_HEADER header;
        pefile.read(reinterpret_cast<char*>(&header), sizeof(IMAGE_SECTION_HEADER));

        if (header.SizeOfRawData != 0){
            DWORD virtual_size_aligned;
            if (header.Misc.VirtualSize == 0) {
                virtual_size_aligned = ALIGN_UP(header.SizeOfRawData, nt_headers.OptionalHeader.SectionAlignment);
            }
            else {
                virtual_size_aligned = ALIGN_UP(header.Misc.VirtualSize, nt_headers.OptionalHeader.SectionAlignment);
            }
        }

        char name[9] = { 0 };
        memcpy(name, header.Name, 8);
        std::cout << std::setw(20) << "Section: " << name << std::endl << "=======================" << std::endl;
        std::cout << std::setw(20) << "Virtual size:" << header.Misc.VirtualSize << std::endl;
        std::cout << std::setw(20) << "Raw size:" << header.SizeOfRawData << std::endl;
        std::cout << std::setw(20) << "Virtual address:" << header.VirtualAddress << std::endl;
        std::cout << std::setw(20) << "Raw address:" << header.PointerToRawData << std::endl;

        std::cout << std::endl << std::endl;
    }

    return 0;
}