#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// Configuration
#define PORT_TO_MANAGE "9090"
#define RULE_NAME "Timed_HW_Gate"

void set_firewall(int open) {
    char cmd[256];
    if (open) {
        printf("[+] Opening port %s...\n", PORT_TO_MANAGE);
        sprintf(cmd, "netsh advfirewall firewall add rule name=\"%s\" dir=in action=allow protocol=TCP localport=%s", RULE_NAME, PORT_TO_MANAGE);
    } else {
        printf("[-] Closing port %s...\n", PORT_TO_MANAGE);
        sprintf(cmd, "netsh advfirewall firewall delete rule name=\"%s\"", RULE_NAME);
    }
    system(cmd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <seconds_to_open>\n", argv[0]);
        return 1;
    }

    int duration = atoi(argv[1]);
    
    // Ensure the port is closed initially
    set_firewall(0);

    // Create/Open the same Win32 Event used by your Python scripts
    HANDLE hEvent = CreateEventA(NULL, FALSE, FALSE, "MalwareSignal");
    if (hEvent == NULL) {
        printf("Wait failed (%d)\n", GetLastError());
        return 1;
    }

    printf("Helper active. Waiting for 'MalwareSignal' to open gate for %d seconds...\n", duration);

    while (1) {
        // Block until the Python Server-Sender triggers the event
        WaitForSingleObject(hEvent, INFINITE);
        
        set_firewall(1);  // Open
        Sleep(duration * 1000); 
        set_firewall(0);  // Close
        
        printf("Gate reset. Waiting for next signal...\n");
    }

    CloseHandle(hEvent);
    return 0;
}