# Reproduction notes for the paper artifact

These notes describe source baseline `2f06980860144398887c3658f06cf68f64870257`
and its original instructions. They record static inspection during the 2026-10-22
documentation refresh, not a successful rebuild or a new FPGA experiment.

## Historical environment

| Component | Information recorded in the artifact |
| --- | --- |
| PostgreSQL | 11.2 |
| OS and kernel | Ubuntu 18.04 x64; Linux 5.0.0-23-generic |
| FPGA toolchain | Vitis 2020.2 and an installed XRT runtime |
| Source platform default | `/opt/xilinx/platforms/inspur_f37x_xdma_201920_3/inspur_f37x_xdma_201920_3.xpfm` |
| Kernel connectivity | Eight `pg_filter` compute units, mapped to HBM banks 0-7 |
| Kernel frequency setting | 250 MHz in the middleware/kernel Makefile |
| Historical host sizing guidance | More than 32 GB RAM, 200 GB disk, and 12 CPU cores |

The original English guide names an "Inspur F73X Card (Alveo U280)" while the
Chinese guide names an **Inspur F37X** card and says an Alveo U280 needs configuration.
The Makefile explicitly selects the F37X platform above. These labels have not been
resolved through a new hardware inventory; select the platform against the actual
board, shell, XRT installation, and memory layout before building or loading anything.

The archived `.xclbin` is a historical platform-specific binary. It has not been
revalidated on another machine during this documentation refresh.

## Source and build entry points

- [PostgreSQL module Makefile](../src/hook/pg_seqscan_hook_v2.0/Makefile): builds
  `pg_pathlist_hook_demo`, with PGXS and in-tree `contrib/pg_pathlist_hook_demo` paths.
  Selected headers and `pg_config` must match the target PostgreSQL build.
- [Middleware/kernel Makefile](../src/middleware-and-kernel/pg_seqscan_v3.5.4/Makefile):
  defines `host`, `build`/`xclbin`, and `all` targets. The default `TARGET` is
  `hw_emu`; command-line settings select `sw_emu`, `hw_emu`, or `hw`.
- `DEVICE` selects the `.xpfm` platform. The HBM mapping in
  [config.ini](../src/middleware-and-kernel/pg_seqscan_v3.5.4/config.ini) must match it.
- The historical `do_compile.sh` launches three background builds using `faketime`.
  It is an original development helper, not a verified recipe for a current environment.

These are entry-point descriptions, not confirmation that the original Makefiles
build successfully with a present-day toolchain.

## Archived binary package

The tracked filename is `bin/pg_seqscan_v3.5.4-bin-release.tar.gz`. Its inventory was
inspected without running or installing its contents:

```text
pg_seqscan_v3.5.4-bin-release/
  pg_filter_kernel.xclbin
  pg_filter_host
  pg_pathlist_hook_demo.so
  xrt.ini
  md5sum.txt
```

The original English guide uses `pg_seqscan3.5.4-bin-relaese.tar.gz`, which does not
match that filename. From the repository root, the corresponding archive listing is:

```sh
tar -tzf bin/pg_seqscan_v3.5.4-bin-release.tar.gz
```

The host source [app.cpp](../src/middleware-and-kernel/pg_seqscan_v3.5.4/src/host/app.cpp)
accepts `pg_filter_host -k <xclbin> [-w <worker count>]`, with four workers by default.
Its printed `VERSION` is `v3.5.3` even though the source directory and binary archive
are named `v3.5.4`; the source was left unchanged. Do not infer binary provenance
solely from that banner.

The Makefile `run` and `test` recipes pass the `.xclbin` as a positional argument,
whereas this host source requires `-k`. That mismatch is recorded here and has not
been repaired or runtime-tested.

## Dataset preparation and database configuration

The original guides describe generating TPC-H data with `dbgen`, converting it to
CSV, and importing it with:

```text
tools/create_tpch_demo_db.sh <csv-directory> <data-name> <schema-sql>
```

Inspect [this script](../tools/create_tpch_demo_db.sh) before using it. It copies
`lineitem.csv` to `lineitem.csv.bak`, truncates the input to 5,687,923 rows, stops
the PostgreSQL cluster at `<data-name>`, removes that directory recursively, and
initializes a replacement cluster there. It creates a database with the same name,
imports the supplied SQL file, and stops the cluster at the end.

Use disposable copies of generated data and a new dedicated cluster directory for
any reproduction. The script is not an importer for an existing database. Its
behavior is unchanged in this archive.

When adapting the environment instructions, append PostgreSQL to the search path,
for example `export PATH=/usr/local/postgres/bin:$PATH`; the original English guide's
shorter assignment replaces the existing `PATH`.

The hook requires PostgreSQL configuration and a compatible shared library. Review
existing `shared_preload_libraries` settings rather than replacing unrelated settings.
Database restarts and FPGA loading were not performed during migration.

## Experimental artifacts and validation limits

- [Figure-associated artifacts](../test/pereformance-En/) contain spreadsheets and
  archived measurements from the original research.
- [TPC-H query files](../tools/tpch-sql/) do not establish that every query is
  accelerated, supported, or validated.
- [Original English instructions](legacy-setup-en.md) preserve the previous root
  README byte for byte, including its original commands and inconsistencies.
- [Original Chinese instructions](README-CN.md) are retained unchanged.

Static checks covered documented paths, archive member names, the host argument
parser, Makefile targets, platform configuration, and dataset script behavior.
They do not establish a working build, correct query results, throughput, latency,
CPU load, or compatibility with another FPGA or PostgreSQL version.

For a new reproduction, record the exact source commit, board and platform IDs,
Vitis/XRT versions, PostgreSQL and kernel versions, dataset scale and preparation,
commands, and raw outputs. Keep new results separate from historical measurements.
