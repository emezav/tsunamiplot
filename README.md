# Tsunami data plot utilities

Tsunami plot utilities used by the tsunami simulation models (tsunamin2cuda,
tsunamif1cuda, etc.) to generate GMT-based maps and wave diagrams.

## Requirements

- GNU/Linux: build tools (compiler, linker, make)
- Windows: Visual Studio 2022+
- CMake 3.24+ available in PATH
- Generic Mapping Tools (GMT) available in PATH
- GhostScript

## Setup

Open VS Code and wait for the automatic configuration to complete.
If the VSCode Action Buttons extension is installed, there will be several buttons
at the bottom bar: Clean, Clean & Build, etc.

Generate the library in Release mode before building any model that depends on it.

## Manual setup

Linux:

```sh
mkdir build
cd build
cmake -G "Unix Makefiles" ..
```

Windows (Visual Studio 2022+):

```bat
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
```

## Building

Debug configuration:

```sh
cd build
cmake --build . --config Debug
```

Release configuration:

```sh
cd build
cmake --build . --config Release
```

## Cleaning

Clean debug build:

```sh
cd build
cmake --build . --config Debug --target clean
```

Clean release build:

```sh
cd build
cmake --build . --config Release --target clean
```

## Elevation animation (animate / animate-globe)

Shared by tsunamif1cuda's `animate`/`animate-globe` and tsunamin2cuda's `animate`
commands: `TsunamiPlot::plotElevationAnimation2D` (titled/timestamped flat-map
video) and `TsunamiPlot::plotElevationAnimationGlobe` (rotating orthographic
globe, far-field only). Both render a scenario's per-step elevation snapshots
(`elevNNNNN.*` for far-field, `elr<level>NNNN.*` for near-field) into an MP4.

Requires `save_elevation`/`write_timestep` enabled in the scenario during
`simulate`, so the elevation snapshot grids actually exist to animate.

### Options common to both

| Option | Default | Meaning |
| --- | --- | --- |
| `palette_max_z` | `1.0` | Upper bound (m) of the wave-height color scale. Tune to the scenario's actual max elevation -- too high compresses most of the palette into a single flat color, too low clips peaks to solid dark red. |
| `animate_mask_thresh` | `0.03` | \|elevation\| below this (m) is masked to transparent, letting the bathymetry background show through instead of floating-point noise. Near-field scenarios with a nonzero baseline offset may need this raised (e.g. `0.05`). |
| `plot_threads` | `4` | Number of frames rendered concurrently. Each frame is an independent gmt/ffmpeg subprocess; safe to raise on multi-core machines. |
| `animate_fps` | `30` | Output video frame rate. |
| `animate_speed_factor` | `4.0` | Slows playback by this factor (`setpts`) without needing more source frames. |
| `animate_format` | `mp4` | Output container/codec, passed to ffmpeg. |
| `animate_out` | frame prefix | Output video base filename (no extension). Set this to avoid overwriting a previous render when comparing settings (e.g. `animate_out=elr1_test`). |
| `elev_prefix` | `elev` (far) / `elr<N>` (near) | Frame filename prefix. Set automatically by f1cuda/n2cuda from their own scenario keys -- override only for custom setups. |
| `elev_digits` | `5` (far) / `4` (near) | Zero-padded digit count in frame filenames. Set automatically. |
| `elev_interval` | `1` | Steps between snapshots. Set automatically from `snapshot` (far) / `ksnz` (near); used to compute each frame's timestamp label. |

### 2D-only (`animate`)

| Option | Default | Meaning |
| --- | --- | --- |
| `show_bathy` | `true` | Draw a bathymetry/relief background. (Opposite default from `plot-max`'s `show_bathy`, which is off by default -- here the whole point is geographic context for the moving wave.) |
| `bathy_cpt` | `gray` (non-fit) / `globe` (fit) | GMT palette name for the background. |
| `bathy_convention` | unset (standard) | Set to `tunami` if the bathy grid stores positive=ocean depth, negative=land elevation (opposite of GMT's standard geo/globe convention -- this is how tsunamin2cuda's HAT-family scenarios store bathymetry). Verify with `gmt grdtrack` at a known offshore point and a known onland point if unsure -- getting this wrong swaps sea and land colors. |
| `bathy_cpt_fit` | `false` | **Important for nested-grid near-field scenarios.** `false` uses one fixed, real-world-anchored palette (matches far-field's look; correct for ocean-basin/continental-scale grids where the data already spans the palette's full range -- e.g. the coarsest nested grid). `true` locally fits both ocean and land colors to *this specific grid's* own min/max (correct for small-scale/shallow grids -- an inner nested grid covering only a few meters of coastal relief renders as a single flat color under the fixed palette). There is no single right answer across a nested-grid scenario's levels: pass this as a per-invocation CLI override on whichever grid levels need it (e.g. `animate scenario.txt 5 "bathy_cpt_fit=true"`), don't bake it into the scenario file. |
| `plot_coast` | `true` | Draw GSHHS coastlines on top. |
| `coast_resolution` | `f` | GSHHS resolution: `f`/`h`/`i`/`l`/`c`. |

### Globe-only (`animate-globe`, far-field only)

| Option | Default | Meaning |
| --- | --- | --- |
| `animate_bathy_source` | `remote` | `remote` fetches `@earth_relief_<res>` (whole-earth coverage, needed for any real rotation); `file` uses the scenario's own `grid` option instead. |
| `animate_relief_res` | `06m` | Remote relief resolution. `06m` already matches an ~11cm globe rendered at 600dpi -- finer resolutions mean much larger downloads for no visible gain at this render size. |
| `animate_center_lon` / `animate_center_lat` | elevation grid's own extent midpoint | View center at frame 0. |
| `animate_target_lon` / `animate_target_lat` | -- | Rotate from the center towards this site by the last frame, always travelling westward (the same direction the far-field wave itself propagates). Overrides `animate_rotate_deg` when set. |
| `animate_rotate_deg` | `0.0` | Total degrees rotated over the whole movie (fixed view if `0` and no target given). |
| `animate_globe_width` | `11c` | Physical width of the globe on the canvas. |

### Known pitfalls (already fixed, watch for regressions)

- GMT modern-mode sessions are keyed by the *parent* process's PID, not each `gmt` subprocess's own -- rendering frames with `plot_threads > 1` requires a unique `GMT_SESSION_NAME` per frame (handled internally).
- Two concurrent `animate` invocations (e.g. animating two different nested grid levels in parallel) must not share fixed temp filenames for palettes/bathy layers -- namespaced internally by grid path + process id.
- GMT's colorbar `-Dj` anchors *inside* the plot's bounding box; for the globe (a circle inscribed in a square), this cuts across the circle's arc. Use uppercase `-DJ` (outside the frame) for anything drawn on the globe.
- Near-field's `snapz_kernel` writes exactly `-99.0f` for dry/land cells in the elevation snapshot (not an actual NaN) -- masked explicitly (`-Sr-99/NaN`) in addition to the near-zero threshold mask.

## Copyright

MIT License

Copyright (c) 2025 Erwin Meza Vega <emezav@unicauca.edu.co> <emezav@gmail.com>

See LICENSE file for full details.
