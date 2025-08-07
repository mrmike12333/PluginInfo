# PluginInfo

A lightweight Juce App which displays plugin wrapper information.
All you have to do is drag and drop an audio plugin

## Supported Plugins

| Plugin Format | Supported |
|---------------|-----------|
| VST3          | ✅        |
| AudioUnit (AU)| ✅        |
| VST2          | ❌        |
| AAX           | ❌        |
| CLAP          | ❌        |

# Getting Started
First you'll need JUCE, which is loaded in as a submodule

```bash
    git submodule update --init --recursive
```

Then run CMake using your generator of choice:
```bash
    cmake -B build -G <Generator>
```