#include <iostream>
#include "Header.h"


// Function to unlink a DLL from the process
void Unlink(const wchar_t* dllName)
{
    // Get the base address of the process environment block (PEB)
    PROCESS_BASIC_INFORMATION processBasicInformation;
    ULONG size;
    NtQueryInformationProcess(GetCurrentProcess(), ProcessBasicInformation, &processBasicInformation, sizeof(PROCESS_BASIC_INFORMATION), &size);
    PEB* pPeb = processBasicInformation.PebBaseAddress;

    // Iterate over the loaded modules and find the one with the specified name
    for (auto entry = pPeb->Ldr->InMemoryOrderModuleList.Flink; entry != &pPeb->Ldr->InMemoryOrderModuleList; entry = entry->Flink)
    {
        _LDR_DATA_TABLE_ENTRY* pCurEntry = (_LDR_DATA_TABLE_ENTRY*)entry;

        if (!wcscmp(pCurEntry->FullDllName.Buffer, dllName))
        {
            // If we found the module, unlink it from the process
            entry->Flink->Blink = entry->Blink;
            entry->Blink->Flink = entry->Flink;

            std::wcout << L"Unlinked module: " << pCurEntry->FullDllName.Buffer << std::endl;
            break;
        }
    }
}

int main()
{
    // Unlink the sbiedll.dll module from the current process
    Unlink(L"sbiedll.dll");

    // Print the list of loaded modules to verify that the sbiedll.dll module was unlinked
    PROCESS_BASIC_INFORMATION processBasicInformation;
    ULONG size;
    NtQueryInformationProcess(GetCurrentProcess(), ProcessBasicInformation, &processBasicInformation, sizeof(PROCESS_BASIC_INFORMATION), &size);
    PEB* pPeb = processBasicInformation.PebBaseAddress;
    std::wcout << L"Loaded modules:" << std::endl;
    for (auto entry = pPeb->Ldr->InMemoryOrderModuleList.Flink; entry != &pPeb->Ldr->InMemoryOrderModuleList; entry = entry->Flink)
    {
        _LDR_DATA_TABLE_ENTRY* pEntry = (_LDR_DATA_TABLE_ENTRY*)entry;
        std::wcout << L"    " << pEntry->FullDllName.Buffer << std::endl;
    }

    return 0;
}
