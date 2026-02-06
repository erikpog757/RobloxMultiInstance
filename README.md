# Roblox Multi Instance by erikpog757

### Features

- Automatically detects new Roblox instances, whether launched by Roblox or external tools  
- Blocks the Roblox cookie file to prevent corrupted game loading, crashes, and unexpected account logouts  
- Memory optimization (trimming) to reduce memory usage and improve stability  

### How It Works

This program uses a **mutex** (mutual exclusion) mechanism to manage multiple Roblox instances running simultaneously. This ensures only one instance accesses shared resources at a time, preventing conflicts and crashes.

When a new Roblox instance starts, regardless of how it was launched, the program detects it instantly and blocks the Roblox cookie file. Blocking these cookies helps avoid corrupted game loading, crashes, and forced logouts caused by simultaneous cookie access.

### How to Use

1. Run the program.  
2. Open Roblox games with multi-instance support through the Roblox website’s alternate account manager (recommended) or manually log into your accounts.  
3. Keep the program open throughout your gaming sessions to maintain stability and avoid conflicts between multiple instances.  
4. Play Roblox normally once setup.

### Important Notes

- This program’s functionality may be detected by Roblox’s security systems, which could lead to warnings or restrictions on your accounts.  
- Use this software responsibly and be aware of the inherent risks.  
- Continuous operation of the program is essential for consistent multi-instance support.
