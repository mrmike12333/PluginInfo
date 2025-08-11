# Plugin Info

A lightweight Juce GUI App for inspecting audio plugin metadata.

Plugin info analyses plugins on a file drop, where it performs a singular scan on your plugin.
There's no caching, so the results are exactly what the plugin reports!

Useful for validating your CMake lists/Projucer config and for debugging platform-specific config issues.

## Supported Plugins Formats

| Plugin Format  | Supported |
|----------------|-----------|
| VST3           | ✅         |
| AudioUnit (AU) | ✅         |
| VST2           | ❌         |
| AAX            | ❌         |
| CLAP           | ❌         |
| LV2            | ❌         |

## Plugin Information

Plugin Info reports the [following information](https://docs.juce.com/master/classPluginDescription.html):
- Plugin Name
- Format
- Category (instrument/fx)
- Manufacturer
- Version
- Unique Identifier
- If the plugin is an instrument
- Channel I/O 
- If the plugin is part of a shell container
- ARA Capability

# Getting Started
After cloning the project, you'll need JUCE, which is loaded in as a submodule:

```bash
    git submodule update --init --recursive
```

Then run CMake using your generator of choice:
```bash
    cmake -B build -G <Generator>
```