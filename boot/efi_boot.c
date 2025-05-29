#include <efi.h>
#include <efilib.h>
#include <efiprot.h>

// Protocol GUIDs
EFI_GUID SimpleFileSystemProtocol = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID FileInfoGuid = EFI_FILE_INFO_ID;

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    EFI_FILE_IO_INTERFACE *file_io = NULL;
    EFI_FILE_HANDLE root_dir = NULL;
    EFI_FILE_HANDLE kernel_file = NULL;
    UINTN kernel_size = 0;
    EFI_PHYSICAL_ADDRESS kernel_base = 0;
    UINTN pages = 0;

    // Initialize EFI library
    InitializeLib(ImageHandle, SystemTable);

    // Get the loaded image protocol
    status = uefi_call_wrapper(BS->HandleProtocol, 3,
                              ImageHandle,
                              &LoadedImageProtocol,
                              (void **)&loaded_image);
    if (EFI_ERROR(status)) {
        Print(L"Failed to get loaded image protocol\n");
        return status;
    }

    // Get the file system protocol
    status = uefi_call_wrapper(BS->HandleProtocol, 3,
                              loaded_image->DeviceHandle,
                              &SimpleFileSystemProtocol,
                              (void **)&file_io);
    if (EFI_ERROR(status)) {
        Print(L"Failed to get file system protocol\n");
        return status;
    }

    // Open the root directory
    status = uefi_call_wrapper(file_io->OpenVolume, 2,
                              file_io,
                              &root_dir);
    if (EFI_ERROR(status)) {
        Print(L"Failed to open root directory\n");
        return status;
    }

    // Try different possible kernel paths
    CHAR16 *kernel_paths[] = {
        L"kernel.bin",
        L"\\kernel.bin",
        L"/kernel.bin",
        NULL
    };

    for (int i = 0; kernel_paths[i] != NULL; i++) {
        status = uefi_call_wrapper(root_dir->Open, 5,
                                  root_dir,
                                  &kernel_file,
                                  kernel_paths[i],
                                  EFI_FILE_MODE_READ,
                                  0);
        if (!EFI_ERROR(status)) {
            break;
        }
    }

    if (EFI_ERROR(status)) {
        Print(L"Failed to open kernel.bin\n");
        return status;
    }

    // Get kernel file size
    EFI_FILE_INFO *file_info = NULL;
    UINTN info_size = 0;
    status = uefi_call_wrapper(kernel_file->GetInfo, 4,
                              kernel_file,
                              &FileInfoGuid,
                              &info_size,
                              file_info);
    if (status == EFI_BUFFER_TOO_SMALL) {
        file_info = AllocatePool(info_size);
        status = uefi_call_wrapper(kernel_file->GetInfo, 4,
                                  kernel_file,
                                  &FileInfoGuid,
                                  &info_size,
                                  file_info);
    }
    if (EFI_ERROR(status)) {
        Print(L"Failed to get kernel file info\n");
        return status;
    }
    kernel_size = file_info->FileSize;
    FreePool(file_info);

    // Allocate memory for kernel
    pages = (kernel_size + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE;
    status = uefi_call_wrapper(BS->AllocatePages, 4,
                              AllocateAnyPages,
                              EfiLoaderData,
                              pages,
                              &kernel_base);
    if (EFI_ERROR(status)) {
        Print(L"Failed to allocate memory for kernel\n");
        return status;
    }

    // Read kernel into memory
    status = uefi_call_wrapper(kernel_file->Read, 3,
                              kernel_file,
                              &kernel_size,
                              (void *)kernel_base);
    if (EFI_ERROR(status)) {
        Print(L"Failed to read kernel\n");
        return status;
    }

    // Close file handles
    uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
    uefi_call_wrapper(root_dir->Close, 1, root_dir);

    // Exit boot services
    status = uefi_call_wrapper(BS->ExitBootServices, 2,
                              ImageHandle,
                              NULL);
    if (EFI_ERROR(status)) {
        Print(L"Failed to exit boot services\n");
        return status;
    }

    // Jump to kernel
    typedef void (*kernel_entry_t)(void);
    kernel_entry_t kernel_entry = (kernel_entry_t)kernel_base;
    kernel_entry();

    // We should never get here
    return EFI_LOAD_ERROR;
} 