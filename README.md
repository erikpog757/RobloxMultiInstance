# Roblox Multi Instance by erikpog757

## Unstable Branch

### Features

- Automatically detects new Roblox instances, whether launched by Roblox or external tools  
- Blocks the Roblox cookie file to prevent corrupted game loading, crashes, and unexpected account logouts  
- Optional memory optimization (trimming) to reduce memory usage and improve stability  

### How It Works

This program uses a **mutex** (mutual exclusion) mechanism to manage multiple Roblox instances running simultaneously. This ensures only one instance accesses shared resources at a time, preventing conflicts and crashes.

When a new Roblox instance starts, regardless of how it was launched, the program detects it instantly and blocks the Roblox cookie file. Blocking these cookies helps avoid corrupted game loading, crashes, and forced logouts caused by simultaneous cookie access.

### How to Use

1. Launch the program.  
2. Configure memory optimization (trimming) and set the process priority for Roblox to optimize performance and stability.  
3. After starting the program and selecting all options, launch games via the Roblox alternate account manager on the Roblox website or log into your accounts manually.  
4. Keep the program running throughout your sessions to maintain stable multi-instance functionality.

### Important Notes

- Due to the nature of its operations, Roblox may detect this software, potentially resulting in warnings, restrictions, or account flags.  
- Exercise caution and use this software at your own risk.  
- This version is aimed at advanced users comfortable with potential risks associated with unstable or experimental software.

The unstable release can be downloaded in the **Releases > 2.1-beta** tab.

---

## Main Branch

### Features

- Designed for stable multi-instance usage with Roblox  
- Efficiently manages Roblox processes and shared resources  
- Blocks the Roblox cookie file to prevent incorrect game loading, game crashes, and sound bugs  

### How to Use

1. Run the program.  
2. Open Roblox games with multi-instance support through the Roblox website’s alternate account manager (recommended) or manually log into your accounts.  
3. Keep the program open throughout your gaming sessions to maintain stability and avoid conflicts between multiple instances.  
4. Play Roblox normally once setup.

### Important Notes

- This program’s functionality may be detected by Roblox’s security systems, which could lead to warnings or restrictions on your accounts.  
- Use this software responsibly and be aware of the inherent risks.  
- Continuous operation of the program is essential for consistent multi-instance support.
