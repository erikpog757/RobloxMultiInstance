# Roblox Multi Instance by erikpog757

## Features

- Automatic detection of new Roblox instances, including those launched by Roblox itself or external tools  
- Blocking the Roblox cookie file to prevent incorrect game loading, crashes, and unexpected account logouts  
- Optional memory optimization (trimming) to improve performance  

## How It Works

The program uses a **mutex** (mutual exclusion) mechanism to manage multiple Roblox instances running simultaneously. This ensures that only one instance accesses shared resources at a time, avoiding conflicts and crashes.  

When a new Roblox instance starts, whether launched directly by Roblox or through other means, the program detects it immediately and blocks the Roblox cookie file. This cookie blocking prevents corrupted loading of games and avoids crashes or forced logouts from accounts caused by simultaneous cookie access.  

To get the best experience, it is recommended to use this program alongside an alt manager on the Roblox website. Do not close the program during your gaming sessions to maintain stable multi-instance support.

## How to Use

1. Open the program.  
2. Select memory optimization (trimming) if needed — this reduces the program’s memory footprint and improves stability.  
3. Choose the process priority for Roblox to optimize resource allocation.  
4. Launch Roblox through the program.  

## Important Notes

Roblox may detect the use of this program as unusual behavior, potentially flagging it or triggering anti-cheat measures. Use it at your own risk.

Possible detections by Roblox include:  
- Multiple concurrent instances launched from the same machine  
- Modified or blocked cookie files  
- Unusual mutex patterns or process behaviors 
